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
         *
         * @param programName The name of the program that will be displayed
         *      in the help text and error messages.
         * @param extractFileName Set this to true if @a programName is a
         *      path that may contain directories, but the help text should
         *      only use the file name part. This is particularly useful if
         *      @a argv[0] is used as the programName.
         */
        explicit ArgumentParser(std::string_view programName,
                                bool extractFileName = false);

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
         * @note The ArgumentParser instance is no longer valid after calling
         *      the non-const version of parse(). All method calls on an invalid
         *      ArgumentParser will throw an exception.
         *
         * @throw ArgosException if argc is 0 or if there are two or more
         *      options that use the same flag.
         */
        [[nodiscard]] ParsedArguments parse(int argc, char* argv[]);

        /**
         * @brief Parses the arguments and options in @a argv.
         *
         * @a argc and @a argv are the same arguments that the @a main
         * function receives: @a argv is a list of zero-terminated
         * strings and @a argc is the number of strings in @a argv. @a argv
         * must have at least one value (i.e. the name of the program itself).
         *
         * @throw ArgosException if argc is 0 or if there are two or more
         *      options that use the same flag.
         */
        [[nodiscard]] ParsedArguments parse(int argc, char* argv[]) const;

        /**
         * @brief Parses the arguments and options in @a args.
         *
         * @note @a args should not have the name of the program itself as its
         *      first value, unlike when parse is called with argc and argv.
         *
         * @note The ArgumentParser instance is no longer valid after calling
         *      the non-const version of parse(). All method calls on an invalid
         *      ArgumentParser will throw an exception.
         *
         * @throw ArgosException if argc is 0 or if there are two or more
         *      options that use the same flag.
         */
        [[nodiscard]] ParsedArguments parse(std::vector<std::string_view> args);

        /**
         * @brief Parses the arguments and options in @a args.
         *
         * @note @a args should not have the name of the program itself as its
         *      first value, unlike when parse is called with argc and argv.
         *
         * @throw ArgosException if argc is 0 or if there are two or more
         *      options that use the same flag.
         */
        [[nodiscard]]
        ParsedArguments parse(std::vector<std::string_view> args) const;

        /**
         * @brief Creates an ArgumentIterator to iterate over the arguments
         *      in argv.
         *
         * @note The ArgumentParser instance is no longer valid after calling
         *      the non-const version of makeIterator(). All method calls on an
         *      invalid ArgumentParser will throw an exception.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]] ArgumentIterator makeIterator(int argc, char* argv[]);

        /**
         * @brief Creates an ArgumentIterator to iterate over the arguments
         *      in argv.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]]
        ArgumentIterator makeIterator(int argc, char* argv[]) const;

        /**
         * @brief Creates an ArgumentIterator to iterate over the arguments
         *      in argv.
         *
         * @note @a args should not have the name of the program itself as its
         *      first value, unlike when parse is called with argc and argv.
         *
         * @note The ArgumentParser instance is no longer valid after calling
         *      the non-const version of makeIterator(). All method calls on an
         *      invalid ArgumentParser will throw an exception.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]]
        ArgumentIterator makeIterator(std::vector<std::string_view> args);

        /**
         * @brief Creates an ArgumentIterator to iterate over the arguments
         *      in argv.
         *
         * @note @a args should not have the name of the program itself as its
         *      first value, unlike when parse is called with argc and argv.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]]
        ArgumentIterator makeIterator(std::vector<std::string_view> args) const;

        /**
         * @brief Returns true if the ArgumentParser allows abbreviated options.
         */
        [[nodiscard]] bool allowAbbreviatedOptions() const;

        /**
         * @brief Enable or disable abbreviated options.
         *
         * Abbreviated options means that for options with flags that consist of
         * multiple characters (e.g. "--verbose" or "/file") it is not necessary
         * to type the whole flag, but only the number of leading characters
         * that is sufficient to uniquely identify the flag.
         * If the program has only three options, --file, --foo and --bar, it is
         * sufficient to write "--b" on the command line to enable the latter,
         * and "--fo" and "--fi" for the first two.
         */
        ArgumentParser& allowAbbreviatedOptions(bool value);

        /**
         * @brief Returns true if the program automatically exits if the
         *      command line has invalid options or arguments, or the help
         *      option is given.
         */
        [[nodiscard]] bool autoExit() const;

        /**
         * @brief Enable or disable automatic exit when the command line has
         *      invalid options or arguments, or the help option is given.
         *
         * Automatic exit is on by default.
         */
        ArgumentParser& autoExit(bool value);

        /**
         * @brief Returns true if option flags are case insensitive.
         */
        [[nodiscard]] bool caseInsensitive() const;

        /**
         * @brief Enable or disable case insensitive option flags.
         *
         * @note Case-insensitivity will only work for ASCII-letters (i.e.
         *      a-z and A-Z).
         */
        ArgumentParser& caseInsensitive(bool value);

        /**
         * @brief Returns whether or not a help option will be auto-generated
         *      if none has been added explicitly.
         */
        [[nodiscard]] bool generateHelpOption() const;

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

        /**
         * @brief Returns the current option style.
         */
        [[nodiscard]] OptionStyle optionStyle() const;

        /**
         * @brief Set the option style.
         *
         * @note The option style can not be changed once any options have been
         *      added.
         */
        ArgumentParser& optionStyle(OptionStyle value);

        /**
         * @brief Returns true if undefined arguments on the command line
         *      will not be treated as errors.
         */
        [[nodiscard]] bool ignoreUndefinedArguments() const;

        /**
         * @brief Enable or disable treating undefined arguments on the command
         *      line as errors.
         *
         * Ignoring undefined arguments can be useful when more than one
         * function are interpreting the command line.
         */
        ArgumentParser& ignoreUndefinedArguments(bool value);

        /**
         * @brief Returns true if undefined options on the command line
         *      will not be treated as errors.
         */
        [[nodiscard]] bool ignoreUndefinedOptions() const;

        /**
         * @brief Enable or disable treating undefined options on the command
         *      line as errors.
         *
         * Ignoring undefined options can be useful when more than one function
         * are interpreting the command line.
         */
        ArgumentParser& ignoreUndefinedOptions(bool value);

        /**
         * @brief Returns the callback function that will be called for every
         *      argument.
         *
         * By default this is an empty function object.
         */
        [[nodiscard]] const ArgumentCallback& argumentCallback() const;

        /**
         * @brief Set the callback function that will be called for every
         *      argument.
         */
        ArgumentParser& argumentCallback(ArgumentCallback callback);

        /**
         * @brief Returns the callback function that will be called for every
         *      option.
         *
         * By default this is an empty function object.
         */
        [[nodiscard]] const OptionCallback& optionCallback() const;

        /**
         * @brief Set the callback function that will be called for every
         *      option.
         */
        ArgumentParser& optionCallback(OptionCallback callback);

        /**
         * @brief Returns the stream that the help text and error messages are
         *      written to.
         *
         * The default value is nullptr.
         */
        [[nodiscard]] std::ostream* stream() const;

        /**
         * @brief Set the stream that the help text and error messages are
         *      written to.
         *
         * By default std::cout is used for the help text and std::cerr for
         * error messages.
         */
        ArgumentParser& stream(std::ostream* stream);

        /**
         * @brief Return the program name.
         */
        [[nodiscard]] const std::string& programName() const;

        /**
         * @brief Set the program name.
         *
         * @note The program name can also be set by the constructor.
         */
        ArgumentParser& programName(const std::string& name);

        /**
         * @brief Set the help text that will appear between the usage section
         *      and the argument and option sections.
         */
        ArgumentParser& text(std::string text);

        /**
         * @brief Set the given part of the help text.
         */
        ArgumentParser& text(TextId textId, std::string text);

        /**
         * @brief Write the help text.
         *
         * @note The help text is displayed automatically when a help option
         *      is used.
         */
        void writeHelpText();

        /**
         * @brief Inform Argos how a long word can be split over multiple lines.
         *
         * By default, Argos will not split a word if it is at all possible to
         * make it fit on a single line. In some cases this can make the help
         * text appear "untidy". Use this function to tell Argos how a
         * particular word can be split, by writing the word with spaces at each
         * potential split point, e.g. "compre hen sive"  will allow Argos to
         * split the word "comprehensive" as either "compre-" "hensive"
         * or "comprehen-" "sive".
         */
        ArgumentParser& addWordSplittingRule(std::string str);

        /**
         * @brief Makes it possible to construct an ArgumentParser with chained
         *      method calls and assign it to a variable.
         */
        ArgumentParser&& move();
    private:
        void checkData() const;

        [[nodiscard]] ArgumentId nextArgumentId() const;

        std::unique_ptr<ParserData> m_Data;
    };
}
