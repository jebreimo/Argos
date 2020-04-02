#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-05.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
import argparse
import glob
import os
import re
import sys


def make_dependency_map(filenames):
    regex = re.compile(r"\s*#include\s*\"([^\"]+)\".*")
    result = {}
    for filename in filenames:
        key = os.path.basename(filename)
        includes = []
        for line in open(filename):
            m = regex.match(line)
            if m:
                includes.append(m.group(1))
        result[key] = includes
    return result


def arrange_includes(includes):
    colors = {k: 0 for k in includes}
    depths = {}

    def dfv(name):
        if name not in includes:
            return -1
        if colors[name] == 2:
            return depths[name]
        if colors[name] == 1:
            raise Exception(f"WARNING: Circular dependency involving {name}.")
        neighbors = includes[name]
        if neighbors:
            colors[name] = 1
            depths[name] = depth = 1 + max(dfv(n) for n in neighbors)
        else:
            depths[name] = depth = 0
        colors[name] = 2
        return depth

    for k in includes:
        dfv(k)
    return depths


def get_file_contents(paths):
    output = []
    for path in paths:
        if output and not output[-1].isspace():
            output.append("\n")
        for line in open(path):
            output.append(line)
    return output


def remove_pragma_once(lines, keep_first):
    output = []
    regex = re.compile(r"\s*#pragma\s+once\b.*")
    for line in lines:
        m = regex.match(line)
        if not m:
            output.append(line)
        elif keep_first:
            output.append(line)
            keep_first = False
    return output


def remove_redundant_includes(lines):
    output = []
    known_includes = set()
    regex = re.compile(r"\s*#include\s*([\"<][^\">]+[\">]).*")
    for line in lines:
        m = regex.match(line)
        if not m:
            output.append(line)
        elif m.group(1) not in known_includes:
            output.append(line)
            known_includes.add(m.group(1))
    return output


def remove_matching_lines(lines, regex):
    output = []
    regex = re.compile(regex)
    for line in lines:
        m = regex.match(line)
        if not m:
            output.append(line)
    return output


def remove_successive_empty_lines(lines):
    output = []
    for line in lines:
        if not output or not line.isspace() or not output[-1].isspace():
            output.append(line)
    return output


def make_argument_parser():
    ap = argparse.ArgumentParser(
        description='Generates source files for a C++ command line argument'
                    ' parser.')
    ap.add_argument("files", metavar="C++ files", nargs="+",
                    help="The C++ files that are to be merged.")
    ap.add_argument("-o", "--output", metavar="FILE",
                    help="The output file.")
    ap.add_argument("--no-pragma-once", action="store_const", const=True,
                    default=False,
                    help="Don't insert a pragma once at the beginning of the"
                         " output file.")
    ap.add_argument("-p", "--prepend", metavar="TEXT", action="append",
                    help="Write TEXT at the start of the output file.")
    return ap


def main():
    args = make_argument_parser().parse_args()
    paths = []
    visited = set()
    for path in args.files:
        if not os.path.exists(path):
            tmp = glob.glob(path)
            if not tmp:
                print(f"WARNING: {path} not found.")
            for p in tmp:
                if p not in visited:
                    paths.append(p)
                    visited.add(p)
        elif path not in visited:
            paths.append(path)
            visited.add(path)
        else:
            print(f"WARNING: {path} is listed more than among the input"
                  f" files. All but the first will be ignored.")

    includes = make_dependency_map(paths)
    depths = arrange_includes(includes)
    paths.sort(key=lambda n: depths[os.path.basename(n)])

    lines = get_file_contents(paths)
    lines = remove_pragma_once(lines, not args.no_pragma_once)
    lines = remove_matching_lines(lines, r"\s*#include\s*\"[^\"]+\".*")
    lines = remove_matching_lines(lines, r"\s*#include\s*\<Argos/[^\>]+\>.*")
    lines = remove_redundant_includes(lines)
    lines = remove_successive_empty_lines(lines)

    if args.prepend:
        text = "".join(args.prepend) + "".join(lines)
    else:
        text = "".join(lines)
    if args.output:
        if os.path.exists(args.output) and open(args.output).read() == text:
            return 0
        open(args.output, "w").write(text)
        print(f"Updated {args.output}")
    else:
        sys.stdout.write(text)

    return 0


if __name__ == "__main__":
    sys.exit(main())
