import os
import subprocess

import modules.file as file
import modules.log as log
import modules.runner as runner


def run_task_build():
    if check_npm():
        root_dir = file.root_dir()
        webapp_dir = os.path.join(root_dir, "webapp")

        run_args = ["npm", "run", "build"]
        runner.run(run_args, webapp_dir)


def run_task_serve():
    if check_npm():
        root_dir = file.root_dir()
        webapp_dir = os.path.join(root_dir, "webapp")

        run_args = ["npm", "run", "serve"]
        runner.run(run_args, webapp_dir)


def run_task_install():
    if check_npm():
        root_dir = file.root_dir()
        webapp_dir = os.path.join(root_dir, "webapp")

        run_args = ["npm", "install"]
        runner.run(run_args, webapp_dir)


def check_npm():
    """Checks if invoking supplied NPM binary works."""
    try:
        subprocess.check_output(["npm", "--version"])
        return True
    except OSError:
        log.error("NPM is not installed, check: https://www.npmjs.com/")
        return False
