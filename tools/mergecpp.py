#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-05.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
import os
import re
import sys

LOCAL_INCLUDE_RE = re.compile(r"\s*#include\s*\"([^\"]+)\".*")
INCLUDE_RE = re.compile(r"\s*#include\s*([\"<][^\">]+[\">]).*")
PRAGMA_ONCE_RE = re.compile(r"\s*#pragma\s+once\b.*")

def make_dependency_map(filenames):
    result = {}
    for filename in filenames:
        key = os.path.basename(filename)
        includes = []
        for line in open(filename):
            m = LOCAL_INCLUDE_RE.match(line)
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
            raise Exception(f"Circular dependency involving {name}.")
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


def write_file_contents(outfile, paths, known_includes):
    ignore_pragma_once = False
    for path in paths:
        for line in open(path):
            m = INCLUDE_RE.match(line)
            if m:
                if m.group(1) not in known_includes:
                    outfile.write(line)
                    known_includes.add(m.group(1))
            else:
                m = PRAGMA_ONCE_RE.match(line)
                if not m:
                    outfile.write(line)
                elif not ignore_pragma_once:
                    outfile.write(line)
                    ignore_pragma_once = True


def main(args):
    includes = make_dependency_map(args)
    for key in includes:
        for inc in includes[key]:
            print(f"{key} -> {inc}")
    depths = arrange_includes(includes)
    for name in depths:
        print(name, depths[name])
    args.sort(key=lambda n: depths[os.path.basename(n)])
    known_includes = {f'"{n}"' for n in includes}
    write_file_contents(sys.stdout, args, known_includes)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
