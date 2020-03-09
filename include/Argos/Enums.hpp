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
    /**
     * @brief The different option styles supported by Argos.
     */
    enum class OptionStyle
    {
        /**
         * @brief Options starts with either one dash (`-`) followed by
         *  exactly one character (short) or two dashes (`--`) followed by
         *  one or more characters (long).
         *
         * Short options can be concatenated making `-pq` and `-p -q`
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

    /**
     * @brief The different value operations an option can perform.
     */
    enum class OptionOperation
    {
        /**
         * @brief The option will not affect any value.
         */
        NONE,
        /**
         * @brief The option will assign a value.
         *
         * If the option is used more than once, the previous value is
         * replaced by the new one. If multiple options share the same value
         * and some have operation ASSIGN and some have operation APPEND, all
         * values that have been appended will be replaced when an ASSIGN
         * option is encountered.
         */
        ASSIGN,
        /**
         * @brief The option will append a value.
         */
        APPEND,
        /**
         * @brief The option will clear a value.
         *
         * Operation CLEAR only makes sense when it shares its value with
         * options that ASSIGNs or APPENDs. It removes the current value or
         * values from ParsedArguments, which can be useful in certain
         * situations where the program is run via a shell alias or script.
         *
         * An example of how this operation can be used:
         *
         * ```
         *  ArgumentParser()
         *      ...
         *      .add(Option({"--include="}).argument("FILE")
         *          .operation(OptionOperation::APPEND)
         *          .text("Add FILE to the list of included files."))
         *      .add(Option({"--include"}).operation(OptionOperation::CLEAR)
         *          .text("Clear the list of included files.")
         *      ...
         * ```
         */
        CLEAR
    };

    /**
     * @brief The OptionType affects how subsequent options and arguments are
     *      processed.
     */
    enum class OptionType
    {
        /**
         * @brief This is just a normal option.
         */
        NORMAL,
        /**
         * @brief Argos will display the help text and not process any
         *      subsequent arguments or options.
         *
         * If ArgumentParser::autoExit is true the program will exit after
         * displaying the help text, if it's not, all remaining arguments and
         * options on the command line are available in ParsedArgument's
         * unprocessedArguments.
         */
        HELP,
        /**
         * @brief The last option that will be treated as a normal
         *  argument or option.
         *
         * Missing arguments and mandatory options will not be treated
         * as errors if this option is given. An example of how this option
         * type is used is for instance a "--version" option where the program
         * displays its version and ignores all other arguments.
         *
         * All remaining arguments and options on the command line are
         * available in ParsedArgument's unprocessedArguments.
         */
        STOP,
        /**
         * @brief The last argument that will be treated as a normal
         *  argument or option.
         *
         * Unlike STOP, missing arguments and mandatory options will be
         * treated as errors when this option type is used.
         *
         * All remaining arguments and options on the command line are
         * available in ParsedArgument's unprocessedArguments. The flag for
         * this option type is typically '--'.
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
        INITIAL_TEXT,
        USAGE_TITLE,
        USAGE,
        TEXT,
        ARGUMENTS_TITLE,
        OPTIONS_TITLE,
        FINAL_TEXT,
        ERROR_USAGE
    };

    /**
     * @brief Controls where in the auto-generated help text an argument or
     *      option is displayed.
     */
    enum class Visibility : unsigned
    {
        /**
         * The argument or option will not be displayed anywhere in the
         *  auto-generated help text.
         */
        HIDDEN,
        /**
         * The argument or option will only be displayed in the
         * auto-generated usage.
         */
        USAGE,
        /**
         * The argument or option will only be displayed in the list of
         * arguments and option.
         */
        TEXT,
        /**
         * The argument or option will be displayed both in the auto-generated
         * usage and the list of arguments and option.
         */
        NORMAL
    };

    /**
     * @brief ands two Visibility values.
     */
    constexpr Visibility operator&(Visibility a, Visibility b)
    {
        return Visibility(unsigned(a) & unsigned(b));
    }
}
