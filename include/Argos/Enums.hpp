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
    enum class OptionOperation
    {
        NONE,
        ASSIGN,
        APPEND,
        CLEAR
    };

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
        SLASH,
        /**
         * @brief Optons starts with a dash (`-`) followed by one or more
         *  characters.
         */
        DASH
    };

    enum class OptionType
    {
        NORMAL,
        HELP,
        /**
         * @brief The last argument that will be treated as a normal
         *  argument or option.
         *
         * Missing arguments and mandatory options will not be treated
         * as errors if this option is given. An example of how this option
         * type is used is for instance a "--version" option where the program
         * displays its version and ignores all other arguments.
         *
         * All remaining arguments and options on the command line are
         * available in unprocessed arguments.
         */
        STOP,
        /**
         * @brief The last argument that will be treated as a normal
         *  argument or option.
         *
         * Unlike STOP, missing arguments and mandatory options will be
         * treated when this option type is used.
         *
         * All remaining arguments and options on the command line are
         * available in unprocessed arguments. The flag for this option
         * type is typically '--'.
         */
        LAST_ARGUMENT,
        /**
         * @brief The last argument that will be treated as an option.
         *
         * Subsequent arguments will not be considered options even if they
         * start with a '-' (or '/' when using SLASH options). The flag for
         * this option type is typically '--'.
         */
        LAST_OPTION
    };

    enum class ParserResultCode
    {
        NONE,
        NORMAL,
        STOP,
        ERROR
    };

    enum class TextId
    {
        USAGE_TITLE,
        USAGE,
        USAGE_DESCRIPTION,
        ARGUMENTS_SECTION,
        ARGUMENTS,
        OPTIONS_SECTION,
        OPTIONS,
        END_TEXT
    };
}
