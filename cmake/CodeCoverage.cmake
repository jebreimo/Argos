# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-15.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)

function(TargetEnableCodeCoverage target isEnabled)
    if (${isEnabled})
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|.*Clang")
            target_compile_options(${target} PRIVATE --coverage)
            target_link_options(${target} PUBLIC --coverage)
        endif ()
    endif ()
endfunction()
