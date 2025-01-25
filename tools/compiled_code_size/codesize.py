#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-20.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================

# This script is used to calculate the size of compiled functions in a
# binary file. It uses the nm command to extract the addresses of the
# functions and then calculates the size of each function by subtracting
# the address of the current function from the address of the next function.
# The nm command must be available in the system's PATH.

import argparse
import re
import subprocess
import sys


def make_arg_parser():
    ap = argparse.ArgumentParser()
    ap.add_argument("FILE", help="Any file supported by the nm command")
    return ap


def main():
    args = make_arg_parser().parse_args()
    output = subprocess.run(["nm", "-n", "--demangle", args.FILE],
                            capture_output=True)
    if output.stderr:
        sys.stderr.write(output.stderr.decode("utf-8"))
        return 1

    start_addr = 0
    func_name = None
    nm_re = re.compile(r"^([0-9a-f]*)\s+([a-zA-Z?])\s(.+)$")
    for line in output.stdout.decode("utf-8").splitlines():
        m = nm_re.match(line)
        if not m:
            func_name = None
            print("ERROR")
            continue
        if not m.group(1):
            func_name = None
            continue
        addr = int(m.group(1), 16)
        if func_name and addr > start_addr:
            print("%6d %s" % (addr - start_addr, func_name))
        func_name = m.group(3) if m.group(2) in "tT" else None
        start_addr = addr
    return 0


if __name__ == "__main__":
    sys.exit(main())
