import os
import subprocess

import modules.file as file
import modules.log as log
import modules.runner as runner
import modules.config as config


def run_task_build_default():
    if check_cmake():
        root_dir = file.root_dir()
        build_dir = os.path.join(root_dir, "build")

        file.remove_dir(build_dir)
        file.create_dir(build_dir)

        run_args = [
            "cmake",
            "-S",
            ".",
            "-B",
            "build",
            "-DCMAKE_BUILD_TYPE={0}".format(config.build_type),
        ]
        runner.run(run_args, root_dir)

        run_args = [
            "cmake",
            "--build",
            "build",
            "--target",
            config.app_name,
            "--config",
            config.build_type,
            "-v",
        ]
        runner.run(run_args, root_dir)


def run_task_build_ninja():
    if check_cmake():
        root_dir = file.root_dir()
        build_dir = os.path.join(root_dir, "build")

        file.remove_dir(build_dir)
        file.create_dir(build_dir)

        run_args = [
            "cmake",
            "-S",
            ".",
            "-B",
            "build",
            "-DCMAKE_BUILD_TYPE={0}".format(config.build_type),
            "-G",
            "Ninja",
        ]
        runner.run(run_args, root_dir)

        run_args = [
            "cmake",
            "--build",
            "build",
            "--target",
            config.app_name,
            "--config",
            config.build_type,
            "-v",
        ]
        runner.run(run_args, root_dir)


def run_task_build_xcode():
    if check_cmake():
        root_dir = file.root_dir()
        build_dir = os.path.join(root_dir, "build")

        file.remove_dir(build_dir)
        file.create_dir(build_dir)

        run_args = [
            "cmake",
            "-S",
            ".",
            "-B",
            "build",
            "-DCMAKE_BUILD_TYPE={0}".format(config.build_type),
            "-G",
            "Xcode",
        ]
        runner.run(run_args, root_dir)

        run_args = [
            "cmake",
            "--build",
            "build",
            "--target",
            config.app_name,
            "--config",
            config.build_type,
            "-v",
        ]
        runner.run(run_args, root_dir)


def run_task_build_linux_arm():
    if check_docker():
        target_name = "linux-arm"
        platform_name = "linux/arm/v7"
        platform_arch = "arm"
        cef_platform = "linuxarm"

        root_dir = file.root_dir()
        build_dir = os.path.join(root_dir, "build")
        docker_dir = os.path.join(root_dir, "docker", target_name)

        # build docker image
        log.info("Building docker image...")

        run_args = [
            "docker",
            "build",
            "--platform",
            platform_name,
            "-f",
            "Dockerfile",
            "-t",
            target_name,
            ".",
        ]
        runner.run(run_args, docker_dir)

        # remove old files
        log.info("Removing old files...")

        file.remove_dir(build_dir)
        file.create_dir(build_dir)

        # config project
        log.info("Configuring project...")

        run_args = [
            "docker",
            "run",
            "--platform",
            platform_name,
            "-v",
            "{0}:{1}".format(root_dir, "/workdir"),
            "-w",
            "/workdir",
            target_name,
            "cmake",
            "-Bbuild",
            "-H.",
            "-GNinja",
            "-DCMAKE_BUILD_TYPE={0}".format(config.build_type),
            "-DCEF_PLATFORM={0}".format(cef_platform),
            "-DPROJECT_ARCH={0}".format(platform_arch),
        ]
        runner.run(run_args, root_dir)

        # build project
        log.info("Building project...")

        run_args = [
            "docker",
            "run",
            "--platform",
            platform_name,
            "-v",
            "{0}:{1}".format(root_dir, "/workdir"),
            "-w",
            "/workdir",
            target_name,
            "cmake",
            "--build",
            "build",
            "--target",
            config.app_name,
            "--config",
            config.build_type,
        ]
        runner.run(run_args, root_dir)


def run_task_build_linux_arm64():
    if check_docker():
        target_name = "linux-arm64"
        platform_name = "linux/arm64"
        platform_arch = "arm64"
        cef_platform = "linuxarm64"

        root_dir = file.root_dir()
        build_dir = os.path.join(root_dir, "build")
        docker_dir = os.path.join(root_dir, "docker", target_name)

        # build docker image
        log.info("Building docker image...")

        run_args = [
            "docker",
            "build",
            "--platform",
            platform_name,
            "-f",
            "Dockerfile",
            "-t",
            target_name,
            ".",
        ]
        runner.run(run_args, docker_dir)

        # remove old files
        log.info("Removing old files...")

        file.remove_dir(build_dir)
        file.create_dir(build_dir)

        # config project
        log.info("Configuring project...")

        run_args = [
            "docker",
            "run",
            "--platform",
            platform_name,
            "-v",
            "{0}:{1}".format(root_dir, "/workdir"),
            "-w",
            "/workdir",
            target_name,
            "cmake",
            "-Bbuild",
            "-H.",
            "-GNinja",
            "-DCMAKE_BUILD_TYPE={0}".format(config.build_type),
            "-DCEF_PLATFORM={0}".format(cef_platform),
            "-DPROJECT_ARCH={0}".format(platform_arch),
        ]
        runner.run(run_args, root_dir)

        # build project
        log.info("Building project...")

        run_args = [
            "docker",
            "run",
            "--platform",
            platform_name,
            "-v",
            "{0}:{1}".format(root_dir, "/workdir"),
            "-w",
            "/workdir",
            target_name,
            "cmake",
            "--build",
            "build",
            "--target",
            config.app_name,
            "--config",
            config.build_type,
        ]
        runner.run(run_args, root_dir)


def check_cmake():
    """Checks if invoking supplied CMake binary works."""
    try:
        subprocess.check_output(["cmake", "--version"])
        return True
    except OSError:
        log.error("CMake is not installed, check: https://www.cmake.org/")
        return False


def check_docker():
    """Checks if invoking supplied docker binary works."""
    try:
        subprocess.check_output(["docker", "--version"])
        return True
    except OSError:
        log.error("Docker is not installed, check: https://www.docker.com/")
        return False
