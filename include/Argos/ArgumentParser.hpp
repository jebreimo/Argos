//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include "ArgosException.hpp"
#include "ArgosVersion.hpp"
#include "Argument.hpp"
#include "ArgumentIterator.hpp"
#include "Option.hpp"

/**
 * @file
 * @brief Defines the ArgumentParser class.
 */

namespace Argos
{
    struct ParserData;

    /**
     * @brief This main class of Argos.
     *
     * Use this class to define the program's command line interface and
     * parse the actual command line arguments.
     */
    class ArgumentParser
    {
    public:
        ArgumentParser();

        /**
         * @brief Creates a new argument parser.
         * @param programName The name of the program that will be displayed
         *      in the help text and error messages.
         */
        explicit ArgumentParser(const std::string& programName);

        /**
         * @brief Moves the innards of the old object to the new one.
         *
         * Any attempt to use the old object will result in an exception.
         */
        ArgumentParser(ArgumentParser&&) noexcept;

        /**
         * @private
         */
        ArgumentParser(const ArgumentParser&) = delete;

        ~ArgumentParser();

        /**
         * @brief Moves the innards of the object on the right hand side
         *      to the one on the left hand side.
         *
         * Any attempt to use the old object will result in an exception.
         */
        ArgumentParser& operator=(ArgumentParser&&) noexcept;

        /**
         * @private
         */
        ArgumentParser& operator=(const ArgumentParser&) = delete;

        /**
         * @brief Add a new argument definition to the ArgumentParser.
         *
         * @throw ArgosException if the argument doesn't have a name.
         */
        ArgumentParser& add(Argument argument);

        /**
         * @brief Add a new option definition to the ArgumentParser.
         *
         * @throw ArgosException if the option doesn't have any flags
         *      or the flags don't match the current option style.
         * @throw ArgosException if certain meaningless combinations of
         *      option operation and properties are found:
         *      - an option with operation NONE is mandatory or has value
         *        or valueName.
         *      - an option with operation CLEAR is mandatory.
         *      - an option
         */
        ArgumentParser& add(Option option);

        /**
         * @brief Parses the arguments and options in argv.
         *
         * @a argc and @a argv are the same arguments that the @a main
         * function receives: @a argv is a list of zero-terminated
         * strings and @a argc is the number of strings in @a argv. @a argv
         * must have at least one value (i.e. the name of the program itself).
         *
         * @note After this non-const version of parse() has been called, the
         * ArgumentParser is no longer valid.
         */
        ParsedArguments parse(int argc, char* argv[]);

        /**
         * @brief Parses the arguments and options in @a argv.
         *
         * @a argc and @a argv are the same arguments that the @a main
         * function receives: @a argv is a list of zero-terminated
         * strings and @a argc is the number of strings in @a argv. @a argv
         * must have at least one value (i.e. the name of the program itself).
         */
        ParsedArguments parse(int argc, char* argv[]) const;

        /**
         * @brief Parses the arguments and options in @a args.
         *
         * Unlike when parse is invoked with @a argc and @a argv, @a args
         * should not have the name of the program itself as its first value.
         *
         * @note After this non-const version of parse() has been called, the
         * ArgumentParser is no longer valid.
         */
        ParsedArguments parse(std::vector<std::string_view> args);

        /**
         * @brief Parses the arguments and options in @a args.
         *
         * Unlike when parse is invoked with @a argc and @a argv, @a args
         * should not have the name of the program itself as its first value.
         */
        ParsedArguments parse(std::vector<std::string_view> args) const;

        ArgumentIterator makeIterator(int argc, char* argv[]);

        ArgumentIterator makeIterator(int argc, char* argv[]) const;

        ArgumentIterator makeIterator(std::vector<std::string_view> args);

        ArgumentIterator makeIterator(std::vector<std::string_view> args) const;

        bool allowAbbreviatedOptions() const;

        ArgumentParser& allowAbbreviatedOptions(bool value);

        bool autoExit() const;

        ArgumentParser& autoExit(bool value);

        bool caseInsensitive() const;

        ArgumentParser& caseInsensitive(bool value);

        /**
         * @brief Returns whether or not a help option will be auto-generated
         *      if none has been added.
         */
        bool generateHelpOption() const;

        /**
         * @brief Turn auto-generation of help option on or off.
         *
         * If this property is true and no help option has been added by the
         * client code, the ArgumentParser will add one itself. Depending on
         * the option style, the help flag will be either --help, /? or -help.
         * No help flag will be added if the ArgumentParser has a conflicting
         * flag.
         *
         * By default this is on.
         */
        ArgumentParser& generateHelpOption(bool value);

        OptionStyle optionStyle() const;

        ArgumentParser& optionStyle(OptionStyle value);

        bool ignoreUndefinedArguments() const;

        ArgumentParser& ignoreUndefinedArguments(bool value);

        bool ignoreUndefinedOptions() const;

        ArgumentParser& ignoreUndefinedOptions(bool value);

        const ArgumentCallback& argumentCallback() const;

        ArgumentParser& argumentCallback(ArgumentCallback callback);

        const OptionCallback& optionCallback() const;

        ArgumentParser& optionCallback(OptionCallback callback);

        std::ostream* outputStream() const;

        ArgumentParser& outputStream(std::ostream* stream);

        const std::string& programName() const;

        ArgumentParser& programName(const std::string& name);

        ArgumentParser& text(std::string text);

        ArgumentParser& text(TextId textId, std::string text);

        ArgumentParser&& move();

        void writeHelpText();
    private:
        void checkData() const;

        ArgumentId nextArgumentId() const;

        std::unique_ptr<ParserData> m_Data;
    };
}
