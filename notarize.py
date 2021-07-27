import sys
import time
import argparse
import asyncio
import subprocess
import logging
from shutil import which
from time import gmtime, strftime

LOG_FMT_CI = (
    "%(asctime)s %(levelname)s:%(filename)s:%(lineno)d(%(process)d): %(message)s"
)
log = logging.getLogger("Notarizer")
log.setLevel(logging.INFO)

# Unify format of all messages
handler = logging.StreamHandler()
formatter = logging.Formatter(LOG_FMT_CI)
handler.setFormatter(formatter)
log.addHandler(handler)


class NotarizationError(Exception):
    pass


def parseValueFromData(key, data):
    for line in data.split("\n"):
        if line.strip().startswith(key):
            return line.split(key)[-1].strip()
    return ""


async def requestCmd(args, cmd):
    p = await asyncio.create_subprocess_exec(
        *cmd, stdout=asyncio.subprocess.PIPE, stderr=subprocess.STDOUT
    )
    attempts = 3

    while attempts:
        try:
            data = await asyncio.wait_for(p.communicate(), timeout=args.timeout)
            break
        except (asyncio.TimeoutError, subprocess.TimeoutExpired):
            log.warning("Timeout (%ss)", str(args.timeout))
            attempts -= 1
            if attempts:
                log.info("Waiting a bit before next attempt..")
                await asyncio.sleep(60)
        except subprocess.CalledProcessError as commandErr:
            log.critical("Failed to run command: %s", str(commandErr))
            raise
        except Exception as e:
            log.critical("Something failed: %s", str(e))
            raise

    return data[0].decode("utf-8")


async def requestNotarization(args):
    # long lasting command, it uploads the binary to Apple server
    cmd = [
        "xcrun",
        "altool",
        "-u",
        args.user,
        "-p",
        args.passwd,
        "--notarize-app",
        "-t",
        "osx",
    ]

    if args.asc_provider:
        cmd += ["--asc-provider", args.asc_provider]

    cmd += ["--primary-bundle-id", args.bundle_id, "-f", args.dmg]

    data = await requestCmd(args, cmd)
    requestUUID = parseValueFromData("RequestUUID", data)
    if not requestUUID:
        raise NotarizationError("Failed to notarize app:\n\n{0}".format(data))
    return requestUUID.split("=")[-1].strip()


async def pollNotarizationCompleted(args, uuid):
    cmd = [
        "xcrun",
        "altool",
        "-u",
        args.user,
        "-p",
        args.passwd,
        "--notarization-info",
        uuid,
    ]

    if args.asc_provider:
        cmd += ["--asc-provider", args.asc_provider]

    attempts = 180
    pollInterval = 60  # attempts * pollInterval = 3h
    while attempts:
        data = await requestCmd(args, cmd)
        statusCode = parseValueFromData("Status Code:", data)

        if statusCode == "0":
            log.info("Notarization succeeded for: %s", args.dmg)
            log.info("%s", data)
            return True
        elif statusCode == "2":
            log.info("Notarization failed for: %s", args.dmg)
            raise NotarizationError("Notarization failed:\n\n{0}".format(data))
        else:
            log.info("Notarization not ready yet for: %s", args.dmg)
            log.info("%s", data)

        attempts -= 1
        log.info(
            "Sleeping %is before next poll attempt (attempts left: %i)",
            pollInterval,
            attempts,
        )
        await asyncio.sleep(pollInterval)

    log.warning("Notarization poll timeout..")
    return False


async def embedNotarization(args):
    # Embed the notarization in the dmg package
    cmd = ["xcrun", "stapler", "staple", args.dmg]
    retry_count = 5
    delay = 60
    while retry_count:
        retry_count -= 1
        data = await requestCmd(args, cmd)
        status = parseValueFromData("The staple and validate action", data)

        if status.lower().startswith("worked"):
            log.info("The [%s] was notirized successfully!", args.dmg)
            break

        log.error("Failed to 'staple' the %s - Reason:\n\n%s", args.dmg, data)

        if retry_count:
            log.warning(f"Trying again after {delay}s")
            time.sleep(delay)
            delay = delay + delay / 2  # 60, 90, 135, 202, 303
        else:
            log.critical(f"Execution of the remote script probably failed!")
            raise NotarizationError("Failed to 'staple' the: {0}".format(args.dmg))


async def main(args):
    uuid = await requestNotarization(args)
    if not await pollNotarizationCompleted(args, uuid):
        raise NotarizationError("Notarization failed for: {0}".format(args.dmg))
    await embedNotarization(args)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="Helper script to notarize given macOS disk image (.dmg)"
    )
    parser.add_argument(
        "--dmg",
        dest="dmg",
        required=True,
        type=str,
        help=".dmg file",
    )
    parser.add_argument(
        "--user",
        dest="user",
        type=str,
        default="",
        help="App Store Connect Username",
    )
    parser.add_argument(
        "--passwd",
        dest="passwd",
        type=str,
        default="",
        help="App Store Connect Password",
    )
    parser.add_argument(
        "--bundle-id",
        dest="bundle_id",
        default=strftime("%Y-%m-%d-%H-%M-%S", gmtime()),
        type=str,
        help="Give unique id for this bundle",
    )
    parser.add_argument(
        "--asc-provider",
        dest="asc_provider",
        default="",
        type=str,
        help="Give the ProviderShortname used for notarization",
    )
    parser.add_argument(
        "--timeout",
        dest="timeout",
        type=int,
        default=60 * 60 * 3,
        help="Timeout value for the remote requests",
    )
    args = parser.parse_args(sys.argv[1:])

    if not which("xcrun"):
        log.error(
            "Could not find 'xcrun' from the system. This tool is needed for notarization. Aborting.."
        )
        sys.exit(1)

    loop = asyncio.get_event_loop()
    loop.run_until_complete(main(args))
