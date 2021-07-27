import os

import modules.file as file
import modules.log as log
import modules.runner as runner
import modules.config as config


def run_task_run():
    root_dir = file.root_dir()
    build_dir = os.path.join(root_dir, "build", config.build_type)

    macos_path = os.path.join(
        build_dir,
        "{0}.app".format(config.app_name),
    )

    posix_path = os.path.join(
        build_dir,
        "{0}".format(config.app_name),
    )

    windows_path = os.path.join(
        build_dir,
        "{0}.exe".format(config.app_name),
    )

    if os.path.exists(macos_path):
        run_args = ["open", macos_path]
        runner.run(run_args, build_dir)
    elif os.path.exists(windows_path):
        run_args = [windows_path]
        runner.run(run_args, build_dir)
    elif os.path.exists(posix_path):
        run_args = [posix_path]
        runner.run(run_args, build_dir)
    else:
        log.error("Binary not found! Did you build it before run?")
