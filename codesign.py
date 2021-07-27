import os, sys, subprocess, argparse

CURRENT_DIRECTORY_PATH = "."

# Most directories are from Table 3 of https://developer.apple.com/library/mac/technotes/tn2206/_index.html
# Library/QuickLook was missing from the documentation, but it's probably needed. Hopefully nothing else is missing, but you never know.
# These paths are searched deeply (with the exception of CURRENT_DIRECTORY_PATH), so there can be sub-directories in them that may be traversed
# You may wonder, if that's the case, if we should just supply "Library" instead of particular directories inside of Library -- and that would be a good question...
# The order is intentional: everything except the main executable and root-level items are done first to ensure nested code gets signed first
VALID_SIGNING_PATHS = [
    "Frameworks",
    "Libraries",
    "PlugIns",
    "XPCServices",
    "Helpers",
    "Library/QuickLook",
    "Library/Automator",
    "Library/Spotlight",
    "Library/LoginItems",
    "Library/LaunchServices",
    "MacOS",
    CURRENT_DIRECTORY_PATH,
]


def log_message(message, newline=True):
    sys.stderr.write(message)
    if newline:
        sys.stderr.write("\n")


def log_message_bytes(message):
    try:
        log_message(message.decode("utf-8"), newline=False)
    except UnicodeDecodeError:
        pass


# Note: do not check for Mach-O binaries specifically; technically other types of files can be signed (via xattr) like executable marked scripts, even though it's bad practice
# If allow_ordinary_files is True, we allow signing non-executable permission marked files
# If we are in root bundle directories excepting Info.plist, PkgInfo, etc, then allow_ordinary_files should be passed as False
# Otherwise if are in a directory expecting signed files like MacOS, then allow_ordinary_files should be passed as True
def executable_candidate(path, allow_ordinary_files):
    return (
        not os.path.islink(path)
        and not os.path.isdir(path)
        and (allow_ordinary_files or os.access(path, os.X_OK))
    )


def bundle_candidate(path):
    # Does the directory have a file extension - could it be a bundle?
    # Don't check for "Versions" or "Contents" - poorly structured bundles could be missing both
    filename = os.path.basename(os.path.normpath(path))

    return (
        not os.path.islink(path)
        and os.path.isdir(path)
        and len(os.path.splitext(filename)[1][1:]) > 0
    )


# DMG files need be signed alone
def dmg_candidate(path):
    extension = os.path.splitext(path)[1]
    if extension.lower() == ".dmg":
        return True


def codesign_file(path, identity, entitlements_path, verbose):
    if verbose:
        log_message("Signing: %s" % (path))

    cmd = [
        "codesign",
        "-fs",
        identity,
    ]

    if os.access(path, os.X_OK):
        cmd += [
            "--options",
            "runtime",
        ]

    if entitlements_path:
        cmd += [
            "--entitlements",
            os.path.abspath(entitlements_path),
        ]

    cmd += [path]

    if verbose:
        log_message("Command: %s" % (cmd))

    process = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    output, error = process.communicate()

    if verbose and output is not None:
        log_message_bytes(output)
    if verbose and error is not None:
        log_message_bytes(error)
    if process.returncode != 0:
        if not verbose and error is not None:
            log_message_bytes(error)

        log_message("Error: Failed to codesign %s" % (path))
        sys.exit(1)


def codesign_files_in(
    directory_path, identity, entitlements_path, verbose, from_root=True
):
    bundles = []
    executables = []

    # Iterate through the relative valid locations where code can be placed and expected to be signed at
    for signing_directory in VALID_SIGNING_PATHS:
        signing_directory_filename = os.path.join(directory_path, signing_directory)

        if os.path.exists(signing_directory_filename):
            is_root = signing_directory == CURRENT_DIRECTORY_PATH
            should_recurse = not is_root or not from_root

            for filename in os.listdir(signing_directory_filename):
                filepath = os.path.join(signing_directory_filename, filename)

                if bundle_candidate(filepath):
                    bundles.append(filepath)
                elif executable_candidate(
                    filepath, allow_ordinary_files=should_recurse
                ):
                    # If we are in a root directory, we should only sign executable permission marked files
                    # However, if we are in a directory where codesign only expects signed files, we should sign them anyway
                    # Eg: VLC's poor bundle structure has Contents/MacOS/share/vlc512x512.png which *should* be signed
                    executables.append(filepath)
                elif should_recurse and os.path.isdir(filepath):
                    # Another directory we should try to recurse into
                    # For example: Contents/PlugIns/moo/foo.plugin is OK.
                    codesign_files_in(
                        filepath, identity, entitlements_path, verbose, from_root=False
                    )

    # Make sure we sign bundles before we sign executables because top-level executables may require
    # the bundles sitting right next to it to be signed first
    for bundle in bundles:
        codesign_bundle(bundle, identity, entitlements_path, verbose)

    for executable in executables:
        codesign_file(executable, identity, entitlements_path, verbose)


def codesign_versions(versions_path, identity, entitlements_path):
    # Most likely we're in a framework bundle, but we could be in 'Contents/Versions/' from an app bundle too (although that is bad practice)
    # Find and sign all the versions, not just the 'default' version (if one even exists)
    # i.e, do not assume there is a "Current" symbolic link available, because it doesn't have to exist
    for filename in os.listdir(versions_path):
        filepath = os.path.join(versions_path, filename)

        if not os.path.islink(filepath):
            codesign_files_in(filepath, identity, entitlements_path, verbose)


def codesign_bundle(bundle_path, identity, entitlements_path, verbose):
    contents_path = os.path.join(bundle_path, "Contents")
    versions_path = os.path.join(bundle_path, "Versions")

    if os.path.exists(contents_path):
        # A normal bundle (.app, .xpc, plug-in, etc)

        # See if there's any 'Versions' to deal with first
        # Eg: Chrome includes a 'Versions' directory inside 'Contents'
        # Even though it is bad practice, standard codesign validation will pick it up.
        inner_versions_path = os.path.join(contents_path, "Versions")

        if os.path.exists(inner_versions_path):
            codesign_versions(inner_versions_path, identity, entitlements_path)

        codesign_files_in(contents_path, identity, entitlements_path, verbose)
    elif os.path.exists(versions_path):
        # A framework bundle
        codesign_versions(versions_path, identity, entitlements_path)
    else:
        # A "bad" bundle that doesn't include a Versions or Contents directory, but is checked by standard codesign validation
        # Eg: Chrome includes a frameworks bundle that does this
        codesign_files_in(bundle_path, identity, entitlements_path, verbose)

    # Don't forget to sign the bundle, which from my testing may be needed
    codesign_file(bundle_path, identity, entitlements_path, verbose)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create code signatures for bundles recursively."
    )
    parser.add_argument(
        "--verbose",
        "-v",
        help="Enable verbosity",
        action="store_true",
    )
    parser.add_argument(
        "signing_identity",
        help="Identity used when signing code. Same as in codesign(1)",
    )
    parser.add_argument(
        "bundle_path",
        help="Path to the bundle to sign recursively.",
    )
    parser.add_argument(
        "entitlements_path",
        help="Path to the entitlements file.",
        default="",
    )

    args = parser.parse_args()

    verbose = args.verbose
    signing_identity = args.signing_identity
    sign_path = args.bundle_path
    entitlements_path = args.entitlements_path

    if not os.path.exists(sign_path):
        log_message("Error: %s does not exist" % (sign_path))
        sys.exit(1)

    if dmg_candidate(sign_path):
        log_message("Codesign DMG file")
        codesign_file(sign_path, signing_identity, entitlements_path, verbose)
    elif executable_candidate(sign_path, allow_ordinary_files=True):
        log_message("Codesign EXECUTABLE file")
        codesign_file(sign_path, signing_identity, entitlements_path, verbose)
    elif bundle_candidate(sign_path):
        log_message("Codesign BUNDLE file")
        codesign_bundle(sign_path, signing_identity, entitlements_path, verbose)
    else:
        log_message(
            "Error: Path provided is not suitable for being signed: %s" % (sign_path)
        )
        sys.exit(1)
