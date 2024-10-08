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
#include "ArgumentIterator.hpp"
#include "Command.hpp"

/**
 * @file
 * @brief Defines the ArgumentParser class.
 */

namespace argos
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
         * @param program_name The name of the program that will be displayed
         *      in the help text and error messages.
         * @param extract_file_name Set this to true if @a program_name is a
         *      path that may contain directories, but the help text should
         *      only use the file name part. This is particularly useful if
         *      @a argv[0] is used as the program_name.
         */
        explicit ArgumentParser(std::string_view program_name,
                                bool extract_file_name = true);

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
        ArgumentParser& add(Argument& argument);

        ArgumentParser& add(Argument&& argument);

        /**
         * @brief Add a new option definition to the ArgumentParser.
         *
         * @throw ArgosException if the option doesn't have any flags
         *      or any of the flags doesn't match the current option style.
         * @throw ArgosException if certain meaningless combinations of
         *      option operation and properties are found:
         *      - an option with operation NONE has constant
         *        or alias.
         *      - an option with operation CLEAR is mandatory.
         *      - an option with operation APPEND has neither argument nor
         *        constant.
         */
        ArgumentParser& add(Option& option);
        ArgumentParser& add(Option&& option);

        ArgumentParser& add(Command& command);
        ArgumentParser& add(Command&& command);

        ArgumentParser& copy_from(const Command& command);

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
         *      the non-const version of make_iterator(). All method calls on an
         *      invalid ArgumentParser will throw an exception.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]] ArgumentIterator make_iterator(int argc, char* argv[]);

        /**
         * @brief Creates an ArgumentIterator to iterate over the arguments
         *      in argv.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]]
        ArgumentIterator make_iterator(int argc, char* argv[]) const;

        /**
         * @brief Creates an ArgumentIterator to iterate over the arguments
         *      in argv.
         *
         * @note @a args should not have the name of the program itself as its
         *      first value, unlike when parse is called with argc and argv.
         *
         * @note The ArgumentParser instance is no longer valid after calling
         *      the non-const version of make_iterator(). All method calls on an
         *      invalid ArgumentParser will throw an exception.
         *
         * @throw ArgosException if there are two or more options that use
         *      the same flag.
         */
        [[nodiscard]]
        ArgumentIterator make_iterator(std::vector<std::string_view> args);

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
        ArgumentIterator make_iterator(std::vector<std::string_view> args) const;

        /**
         * @brief Returns true if the ArgumentParser allows abbreviated options.
         */
        [[nodiscard]] bool allow_abbreviated_options() const;

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
        ArgumentParser& allow_abbreviated_options(bool value);

        /**
         * @brief Returns true if the program automatically exits if the
         *      command line has invalid options or arguments, or the help
         *      option is given.
         */
        [[nodiscard]] bool auto_exit() const;

        /**
         * @brief Enable or disable automatic exit when the command line has
         *      invalid options or arguments, or the help option is given.
         *
         * Automatic exit is on by default.
         */
        ArgumentParser& auto_exit(bool value);

        /**
         * @brief Returns true if option flags are case-insensitive.
         */
        [[nodiscard]] bool case_insensitive() const;

        /**
         * @brief Enable or disable case-insensitive option flags.
         *
         * @note Case-insensitivity will only work for ASCII-letters (i.e.
         *      a-z and A-Z).
         */
        ArgumentParser& case_insensitive(bool value);

        /**
         * @brief Returns whether a help option will be auto-generated
         *      if none has been added explicitly.
         */
        [[nodiscard]] bool generate_help_option() const;

        /**
         * @brief Turn auto-generation of help option on or off.
         *
         * If this property is true and no help option has been added by the
         * client code, the ArgumentParser will add one itself. Depending on
         * the option style, the help flag will be either --help, /? or -help.
         * No help flag will be added if the ArgumentParser has a conflicting
         * flag.
         *
         * By default, this is on.
         */
        ArgumentParser& generate_help_option(bool value);

        /**
         * @brief Returns the current option style.
         */
        [[nodiscard]] OptionStyle option_style() const;

        /**
         * @brief Set the option style.
         *
         * @note The option style can no longer be changed once options
         *      have been added.
         */
        ArgumentParser& option_style(OptionStyle value);

        [[nodiscard]] std::optional<bool> require_command() const;

        ArgumentParser& require_command(bool value);

        // add functions for multi_command:
        [[nodiscard]] bool multi_command() const;

        ArgumentParser& multi_command(bool value);

        /**
         * @brief Returns true if undefined arguments on the command line
         *      will not be treated as errors.
         */
        [[nodiscard]] bool ignore_undefined_arguments() const;

        /**
         * @brief Enable or disable treating undefined arguments on the command
         *      line as errors.
         *
         * Ignoring undefined arguments can be useful when more than one
         * function are interpreting the command line.
         */
        ArgumentParser& ignore_undefined_arguments(bool value);

        /**
         * @brief Returns true if undefined options on the command line
         *      will not be treated as errors.
         */
        [[nodiscard]] bool ignore_undefined_options() const;

        /**
         * @brief Enable or disable treating undefined options on the command
         *      line as errors.
         *
         * Ignoring undefined options can be useful when more than one function
         * are interpreting the command line.
         */
        ArgumentParser& ignore_undefined_options(bool value);

        /**
         * @brief Returns the callback function that will be called for every
         *      argument.
         *
         * By default, this is an empty function object.
         */
        [[nodiscard]] const ArgumentCallback& argument_callback() const;

        /**
         * @brief Set the callback function that will be called for every
         *      argument.
         */
        ArgumentParser& argument_callback(ArgumentCallback callback);

        /**
         * @brief Returns the callback function that will be called for every
         *      option.
         *
         * By default, this is an empty function object.
         */
        [[nodiscard]] const OptionCallback& option_callback() const;

        /**
         * @brief Set the callback function that will be called for every
         *      option.
         */
        ArgumentParser& option_callback(OptionCallback callback);

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
         * By default, std::cout is used for the help text and std::cerr for
         * error messages.
         */
        ArgumentParser& stream(std::ostream* stream);

        /**
         * @brief Return the program name.
         */
        [[nodiscard]] const std::string& program_name() const;

        /**
         * @brief Set the program name.
         *
         * @note The program name can also be set with the constructor.
         */
        ArgumentParser& program_name(const std::string& name);

        /**
         * @brief Set the help text that will appear between the usage section
         *      and the argument and option sections.
         */
        ArgumentParser& about(std::string text);

        /**
         * @brief Sets the program version and enables the --version option.
         * @param version The version is typically 2-4 numbers separated
         *  by periods, e.g. 1.0.3.
         */
        ArgumentParser& version(const std::string& version);

        /**
         * @brief Sets a section (or heading) that is automatically assigned
         *   to arguments and options when they are added.
         *
         * This value is only applied to arguments and options that have not
         * been assigned a section with Argument::section or Option::section.
         * If this value is an empty string, the values from
         * TextId::ARGUMENTS_TITLE and TextId::OPTIONS_TITLE are used.
         *
         * @param name All arguments and options with the same section name
         *  will be listed under the same heading.
         */
        ArgumentParser& section(const std::string& name);

        /**
         * @brief Set the given part of the help text.
         *
         * With this function it is possible to override otherwise
         * auto-generated parts of the text, e.g. TextId::USAGE, or
         * add additional text, e.g. TextId::INITIAL_TEXT and
         * TextId::FINAL_TEXT.
         */
        ArgumentParser& text(TextId textId, std::string text);

        /**
         * @brief Set a function that will produce the given part of
         *  the help text.
         *
         * With this function it is possible to override otherwise
         * auto-generated parts of the text, e.g. TextId::USAGE, or
         * add additional text, e.g. TextId::INITIAL_TEXT and
         * TextId::FINAL_TEXT.
         */
        ArgumentParser& text(TextId textId, std::function<std::string()> callback);

        /**
         * @brief Sets the line width for help text and error messages.
         *
         * The line width defaults to the width of the console or terminal
         * window.
         */
        ArgumentParser& line_width(unsigned line_width);

        /**
         * @brief Inform Argos how a long word is to be split over multiple
         *  lines.
         *
         * By default, Argos will not split words in the help text if it is at
         * all possible to fit them on a single line. In some cases this can
         * make the help text appear untidy. Use this function to tell Argos
         * how a particular word can be split by writing the word with spaces
         * at each potential split point. For instance, calling this function
         * with argument "compre hen sive" will inform Argos that
         * it can split the word "comprehensive" as either "compre-" "hensive"
         * or "comprehen-" "sive" (or even "compre-" "hen-" "sive").
         */
        ArgumentParser& add_word_splitting_rule(std::string str);

        /**
         * @brief Set the codes Argos will use when it calls exit.
         *
         * These values are the values that the program returns to the shell
         * or other program that started it.
         *
         * @note The values assigned here are only used if auto_exit is true.
         *
         * @param error the program's return value if there are incorrect
         *  arguments or options. The default value is EX_USAGE on platforms
         *  that provide that constant, 64 on those that don't.
         * @param normal_exit the program's return value when the help option
         *  or an option with type OptionType::EXIT is encountered. The
         *  default value is 0.
         */
        ArgumentParser& set_exit_codes(int error, int normal_exit);

        /**
         * @brief Write the help text.
         *
         * @note The help text is displayed automatically when a help option
         *      is used.
         */
        void write_help_text() const;

        /**
         * @brief Makes it possible to construct an ArgumentParser with chained
         *      method calls and assign it to a variable without invoking
         *      the copy constructor.
         */
        ArgumentParser&& move();
    private:
        void check_data() const;

        void update_and_validate_option(OptionData& od);

        [[nodiscard]] ArgumentId next_argument_id() const;

        std::unique_ptr<ParserData> m_data;
    };
}
