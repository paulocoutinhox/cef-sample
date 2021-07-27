import os
import subprocess

import modules.file as file
import modules.log as log
import modules.runner as runner
import modules.config as config


def run_task_build():
    if check_create_dmg():
        root_dir = file.root_dir()

        run_args = [
            "create-dmg",
            "--volname",
            config.app_name,
            "--icon",
            "build/{0}/{1}.app".format(config.build_type, config.app_name),
            "--hide-extension",
            "build/{0}/{1}.app".format(config.build_type, config.app_name),
            "--hdiutil-quiet",
            "{0}.dmg".format(config.app_name),
            "build/{0}/{1}.app".format(config.build_type, config.app_name),
        ]
        runner.run(run_args, root_dir)


def check_create_dmg():
    """Checks if invoking supplied create-dmg binary works."""
    try:
        subprocess.check_output(["create-dmg", "--version"])
        return True
    except OSError:
        log.error(
            "Create DMG is not installed, check: https://github.com/create-dmg/create-dmg"
        )
        return False
