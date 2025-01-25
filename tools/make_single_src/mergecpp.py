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


INCLUDE_FILE_REGEX = re.compile(r"\s*#include\s*(?:\"([^\"]+)\"|<([^>]+)>).*")


def get_all_files_names(dir_name):
    result = {}
    for top_dir, subdirs, file_names in os.walk(dir_name):
        for file in file_names:
            path = os.path.join(top_dir, file)
            relpath = os.path.relpath(path, dir_name)
            abspath = os.path.realpath(path)
            if relpath not in result:
                result[relpath.replace("\\", "/")] = abspath
    return result


def get_includes(file_path):
    includes = []
    for line in open(file_path):
        m = INCLUDE_FILE_REGEX.match(line)
        if m:
            includes.append(m.group(1) or m.group(2))
    return includes


def get_dependency_map(files, known_files):
    dependencies = {}

    next_files_to_check = [os.path.realpath(f) for f in files]

    while next_files_to_check:
        files_to_check = next_files_to_check
        next_files_to_check = []

        for file_path in files_to_check:
            real_file_path = os.path.realpath(file_path)
            if real_file_path in dependencies:
                continue

            current_known_files = known_files.copy()
            dir_name = os.path.dirname(file_path)
            if not dir_name:
                dir_name = os.getcwd()
            current_known_files.update(get_all_files_names(dir_name))

            dependencies[real_file_path] = []
            visited = set()

            for include in get_includes(file_path):
                real_include_path = current_known_files.get(include)
                if not real_include_path or real_include_path in visited:
                    continue

                visited.add(real_include_path)
                dependencies[real_file_path].append(real_include_path)
                if real_include_path not in dependencies:
                    next_files_to_check.append(real_include_path)
    return dependencies


def get_file_inclusion_order_rec(result, file_path, dependencies, visited):
    visited.add(file_path)
    for dependency in dependencies[file_path]:
        if dependency not in visited:
            get_file_inclusion_order_rec(result, dependency,
                                         dependencies, visited)
    result.append(file_path)


def get_file_inclusion_order(file_paths, dependencies, ignore_files):
    visited = set(ignore_files)
    result = []
    for file_path in file_paths:
        get_file_inclusion_order_rec(result, os.path.realpath(file_path),
                                     dependencies, visited)
    return result


def get_file_contents(file_path, known_files):
    file_path = os.path.realpath(file_path)

    output = []
    current_known_files = known_files.copy()
    dir_name = os.path.dirname(file_path)
    if not dir_name:
        dir_name = os.getcwd()
    current_known_files.update(get_all_files_names(dir_name))

    for line in open(file_path, encoding="utf-8-sig"):
        m = INCLUDE_FILE_REGEX.match(line)
        if not m or not current_known_files.get(m.group(1) or m.group(2)):
            output.append(line)
    return output


def get_all_file_contents(file_paths, known_files):
    output = []
    for file_path in file_paths:
        if output and not output[-1].isspace():
            output.append("\n")
        output.extend(get_file_contents(file_path, known_files))
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
    for line in lines:
        m = INCLUDE_FILE_REGEX.match(line)
        if not m:
            output.append(line)
        elif (m.group(1) or m.group(2)) not in known_includes:
            output.append(line)
            known_includes.add(m.group(1) or m.group(2))
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
    ap.add_argument("-i", "--include", metavar="DIR", action="append",
                    help="Add DIR to the list of include and source dirs.")
    ap.add_argument("-f", "--filter", metavar="FILE", action="append",
                    help="Filter out FILE, or files in FILE if FILE is a directory.")
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
            print(f"WARNING: {path} is listed more than once among the input"
                  f" files. All but the first will be ignored.")

    known_files = {}
    if args.include:
        for dir_name in args.include[::-1]:
            known_files.update(get_all_files_names(dir_name))

    ignore_files = set()
    if args.filter:
        for name in args.filter:
            if os.path.isdir(name):
                ignore_files.update(get_all_files_names(name).values())
            elif os.path.isfile(name):
                ignore_files.add(os.path.realpath(name))

    dependencies = get_dependency_map(paths, known_files)
    file_order = get_file_inclusion_order(paths, dependencies, ignore_files)

    lines = get_all_file_contents(file_order, known_files)
    lines = remove_pragma_once(lines, not args.no_pragma_once)
    lines = remove_redundant_includes(lines)
    lines = remove_successive_empty_lines(lines)

    if args.prepend:
        text = "".join(args.prepend) + "".join(lines)
    else:
        text = "".join(lines)
    if args.output:
        try:
            if os.path.exists(args.output) and open(args.output, encoding="utf-8").read() == text:
                return 0
        except Exception:
            pass
        if not os.path.exists(os.path.dirname(args.output)):
            os.makedirs(os.path.dirname(args.output))
        open(args.output, "w", encoding="utf-8").write(text)
        print(f"Updated {args.output}")
    else:
        sys.stdout.write(text)

    return 0


if __name__ == "__main__":
    sys.exit(main())
