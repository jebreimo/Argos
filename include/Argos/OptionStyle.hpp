//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace Argos
{
    enum class OptionStyle
    {
        /**
         * @brief Options starts with either one dash (`-`) followed by
         *  exactly one character (short) or two dashes (`--`) followed by
         *  one or more characters (long).
         *
         * Short options can be concatenated so `-pq` and `-p -q` are
         * equivalent.
         */
        STANDARD,
        /**
         * @brief Options starts with a slash (`/`) followed by one or more
         * characters.
         */
        WINDOWS,
        /**
         * @brief Optons starts with a dash (`-`) followed by one or more
         *  characters.
         */
        DASH
    };
}
