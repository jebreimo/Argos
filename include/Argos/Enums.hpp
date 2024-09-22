//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

/**
 * @file
 * @brief Defines the enum types used throughout Argos.
 */

namespace argos
{
    /**
     * @brief The different option styles supported by Argos.
     */
    enum class OptionStyle
    {
        /**
         * @brief Options starts with either one dash (`-`) followed by
         *  exactly one character (short option) or two dashes (`--`) followed
         *  by one or more characters (long option).
         *
         * Short options can be concatenated making `-pq` and `-p -q`
         * equivalent as long as `-p` doesn't expect an argument.
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
         *
         * A NONE option can make sense if the option has a callback function.
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
         *
         * If the option doesn't have either a value or an argument, its
         * value automatically becomes @a true (or 1).
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
         * situations when the program is run via a shell alias or script.
         *
         * An example of how this operation can be used:
         *
         * ~~~{.cpp}
         *  ArgumentParser()
         *      ...
         *      .add(Option({"--include="}).argument("FILE")
         *          .operation(OptionOperation::APPEND)
         *          .text("Add FILE to the list of included files."))
         *      .add(Option({"--include"}).operation(OptionOperation::CLEAR)
         *          .text("Clear the list of included files.")
         *      ...
         * ~~~
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
         * If ArgumentParser::auto_exit is true the program will exit after
         * displaying the help text, if it's not, all remaining arguments and
         * options on the command line are available in ParsedArgument's
         * unprocessed_arguments.
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
         * available in ParsedArgument's unprocessed_arguments.
         */
        STOP,
        /**
         * @brief Similar to STOP, but program will exit if
         *  ArgumentParser::auto_exit is true.
         *
         * Any callbacks assigned to the option or argument parser will be
         * executed first, then the program exits. Use this option type for
         * "--version" options etc.
         *
         * Identical to STOP if ArgumentParser::auto_exit is false.
         */
        EXIT,
        /**
         * @brief The last argument that will be treated as a normal
         *  argument or option.
         *
         * Unlike STOP, missing arguments and mandatory options will be
         * treated as errors when this option type is used.
         *
         * All remaining arguments and options on the command line are
         * available in ParsedArgument's unprocessed_arguments. The flag for
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

    /**
     * @brief A status code that can be retrieved from ParsedArguments.
     */
    enum class ParserResultCode
    {
        /**
         * @brief The initial value. Means that the arguments haven't
         *      been processed yet.
         */
        NONE,
        /**
         * @brief All the arguments and options were successfully processed.
         */
        SUCCESS,
        /**
         * @brief The argument parser encountered an option
         *      of type STOP (or EXIT if auto_exit is false).
         */
        STOP,
        /**
         * @brief The argument parser encountered an incorrect option or
         *      argument (and auto_exit is false).
         *
         * This value was originally called ERROR, but that name clashes
         * with a macro in Windows.h.
         */
        FAILURE
    };

    /**
     * @brief Tells which part of the help text (or error text) is assigned.
     *
     * TextId identifies which part of the help text is being assigned in
     * calls to ArgumentParser::text.
     */
    enum class TextId
    {
        /**
         * @brief Text that appears before the usage section (empty
         *  by default).
         */
        INITIAL_TEXT,
        /**
         * @brief The title of the usage section (default is "USAGE").
         */
        USAGE_TITLE,
        /**
         * @brief The command usage text or synopsis (normally
         *  auto-generated).
         */
        USAGE,
        /**
         * @brief Text that appears between the usage section and the lists of
         *      arguments and options (empty by default).
         */
        ABOUT,
        /**
         * @brief The title of the list of commands (default is "COMMANDS").
         *
         * @note This will only be used for commands where the section
         *      property has not been set.
         */
        COMMANDS_TITLE,
        /**
         * @brief The title of the list of arguments (default is "ARGUMENTS").
         *
         * @note This will only be used for arguments where the section
         *      property has not been set.
         */
        ARGUMENTS_TITLE,
        /**
         * @brief The title of the list of options (default is "OPTIONS").
         *
         * @note This will only be used for options without the section
         *      property.
         */
        OPTIONS_TITLE,
        /**
         * @brief Text that appears at the end of the help text (empty
         *      by default).
         */
        FINAL_TEXT,
        /**
         * @brief Custom usage text for error messages (default is to use
         *      the same text as USAGE).
         */
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

    /**
     * @brief Strong integer type for value ids.
     */
    enum ValueId : int;

    /**
     * @brief Strong integer type for argumnet ids.
     */
    enum ArgumentId : int;
}
