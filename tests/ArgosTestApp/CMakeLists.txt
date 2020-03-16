# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-02-04.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.15)

add_executable(ArgosTestApp
    ArgosTestAppMain.cpp
    )

target_link_libraries(ArgosTestApp
    PRIVATE
        Argos::Argos
    )

TargetEnableAllWarnings(ArgosTestApp)
