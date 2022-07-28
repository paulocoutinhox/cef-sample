#! /usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Make tool

Usage:
  make.py <task-name>
  make.py [options]
  make.py -h | --help

Options:
  -h --help                         Show this screen.
  -d --debug                        Enable debug mode.
  --version                         Show version.

Examples:
  python make.py -h

Tasks:
  - format
  - clean

  - webapp-build
  - webapp-serve
  - webapp-install

  - build
  - build-ninja
  - build-xcode
  - build-linux-arm
  - build-linux-arm64

  - dmg-build

  - run

"""

from docopt import docopt

import modules.tasks.common as common
import modules.tasks.webapp as webapp
import modules.tasks.build as build
import modules.tasks.dmg as dmg
import modules.tasks.run as run
import modules.log as log
import modules.config as c


def main(options):
    # show all params for debug
    if ("--debug" in options and options["--debug"]) or (
        "-d" in options and options["-d"]
    ):
        c.make_debug = True

    if c.make_debug:
        log.normal("You have executed with options:")
        log.normal(str(options))
        log.normal("")

    # bind options
    if "<task-name>" in options:
        make_task = options["<task-name>"]

    # validate data
    log.info("Validating data...")

    # validate task
    if not make_task:
        log.error("Task is invalid")

    # format
    if make_task == "format":
        common.run_task_format()

    # clean
    elif make_task == "clean":
        common.run_task_clean()

    #######################
    # webapp
    #######################

    # webapp - build
    elif make_task == "webapp-build":
        webapp.run_task_build()

    # webapp - serve
    elif make_task == "webapp-serve":
        webapp.run_task_serve()

    # webapp - install
    elif make_task == "webapp-install":
        webapp.run_task_install()

    #######################
    # build
    #######################

    # build - default
    elif make_task == "build":
        build.run_task_build_default()

    # build - ninja
    elif make_task == "build-ninja":
        build.run_task_build_ninja()

    # build - xcode
    elif make_task == "build-xcode":
        build.run_task_build_xcode()

    # build - linux - arm
    elif make_task == "build-linux-arm":
        build.run_task_build_linux_arm()

    # build - linux - arm64
    elif make_task == "build-linux-arm64":
        build.run_task_build_linux_arm64()

    #######################
    # dmg
    #######################

    # dmg - build
    elif make_task == "dmg-build":
        dmg.run_task_build()

    #######################
    # run
    #######################

    # run
    elif make_task == "run":
        run.run_task_run()

    #######################
    # Invalid
    #######################

    # invalid
    else:
        log.error("Task is invalid")

    log.info("FINISHED!")


if __name__ == "__main__":
    # main CLI entrypoint
    args = docopt(__doc__, version="1.0.0")
    main(args)
