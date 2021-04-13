//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

/**
 * @file
 * @brief Defines the current Argos version.
 */

/**
 * @brief String representation of the complete version number.
 */
constexpr char ARGOS_VERSION[] = "0.100.2";

/**
 * @brief Incremented if a new version is significantly incompatible
 *      with the previous version.
 */
#define ARGOS_VERSION_MAJOR 0

/**
 * @brief Incremented when Argos's interface is modified without introducing
 *      incompatibilities with previous versions.
 */
#define ARGOS_VERSION_MINOR 100

/**
 * @brief Incremented when Argos's internals are modified without modifying
 *      its interface.
 */
#define ARGOS_VERSION_PATCH 2

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <stdexcept>
#include <string>

/**
 * @file
 * @brief Defines the ArgosException class.
 */

/**
 * @brief The namespace for all Argos classes and functions.
 */
namespace Argos
{
    /**
     * @brief The exception class used throughout Argos.
     */
    class ArgosException : public std::runtime_error
    {
    public:
        /**
         * @brief Passes @a message on to the base class.
         */
        explicit ArgosException(const std::string& message) noexcept
            : std::runtime_error(message)
        {}

        explicit ArgosException(const char* message) noexcept
            : std::runtime_error(message)
        {}
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the enum types used throughout Argos.
 */

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
         * @brief Similar to STOP, but program will exit if
         *  ArgumentParser::autoExit is true.
         *
         * Any callbacks assigned to the option or argument parser will be
         * executed first, then the program exits. Use this option type for
         * "--version" options etc.
         *
         * Identical to STOP if ArgumentParser::autoExit is false.
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

    /**
     * @brief A status code that can be retrieved from ParsedArguments.
     */
    enum class ParserResultCode
    {
        /**
         * @brief The initial status value. Means that all the arguments haven't
         *      been processed yet.
         */
        NONE,
        /**
         * @brief All the arguments and options were successfully processed.
         */
        SUCCESS,
        /**
         * @brief The argument parser encountered an option
         *      of type STOP (or EXIT of autoExit is false).
         */
        STOP,
        /**
         * @brief The argument parser encountered an incorrect option or
         *      argument (and autoExit is false).
         */
        ERROR
    };

    /**
     * @brief Tells which part of the help text (or error text) is assigned.
     */
    enum class TextId
    {
        /**
         * @brief Text that appears before the usage section (empty by default).
         */
        INITIAL_TEXT,
        /**
         * @brief The title of the usage section (default is "USAGE").
         */
        USAGE_TITLE,
        /**
         * @brief The command usage text or synopsis (normally auto-generated).
         */
        USAGE,
        /**
         * @brief Text that appears between the usage section and the lists of
         *      arguments and options (empty by default).
         */
        ABOUT,
        /**
         * @brief The title of the list of arguments (default is "ARGUMENTS").
         *
         * @note This will only be used for arguments without the section
         *      property.
         */
        ARGUMENTS_TITLE,
        /**
         * @brief The title of the list of options (default is "OPTIONS").

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

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the IArgumentView interface class.
 */

namespace Argos
{
    /**
     * @brief Interface class with the functions ArgumentView and OptionView
     *      have in common.
     */
    class IArgumentView
    {
    public:
        virtual ~IArgumentView() = default;

        /**
         * @brief Returns the argument's or option's help text.
         */
        virtual const std::string& text() const = 0;

        /**
         * @brief Returns the argument's or option's section name.
         */
        virtual const std::string& section() const = 0;

        /**
         * @brief Returns the argument's or option's value name.
         */
        virtual const std::string& value() const = 0;

        /**
         * @brief Returns the argument's or option's visibility in
         *      the help text and error messages.
         */
        virtual Visibility visibility() const = 0;

        /**
         * @brief Returns the argument's or option's custom id.
         */
        virtual int id() const = 0;

        /**
         * @brief Returns the numeric id of the value the argument or option
         *  assigns or appends to.
         *
         * This value is created internally in Argos and must not be
         * confused with the customizable value returned by id().
         * If different options or arguments have the same value name, they
         * will also have the same value id.
         *
         * @return options with operation OptionOperation::NONE have
         *   a value of 0, all other options and arguments have a value
         *   greater than 0.
         */
        virtual ValueId valueId() const = 0;

        /**
         * @brief Returns the argument's or option's argumentId().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        virtual ArgumentId argumentId() const = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the ArgumentView class.
 */

namespace Argos
{
    struct ArgumentData;

    /**
     * @brief Provides read-only access to an argument definition.
     */
    class ArgumentView : public IArgumentView
    {
    public:
        /**
         * @private
         * @brief For internal use only.
         *
         * Client code can only receive objects, not construct them.
         */
        explicit ArgumentView(const ArgumentData* data);

        /**
         * @brief Returns the argument's or option's help text.
         */
        const std::string& text() const final;

        /**
         * @brief Returns the argument's section name.
         */
        const std::string& section() const final;

        /**
         * @brief Returns the argument's value name.
         */
        const std::string& value() const final;

        /**
         * @brief Returns the argument's visibility in
         *      the help text and error messages.
         */
        Visibility visibility() const final;

        /**
         * @brief Returns the argument's custom id.
         */
        int id() const final;

        /**
         * @brief Returns the numeric id of the value the argument assigns
         *      or appends to.
         *
         * This value is created internally in Argos and must not be
         * confused with the customizable value returned by id().
         * If different options or arguments have the same value name, they
         * will also have the same value id.
         *
         * @return options with operation OptionOperation::NONE have
         *      a value of 0, all other options and arguments have a value
         *      greater than 0.
         */
        ValueId valueId() const final;

        /**
         * @brief Returns the argument's argumentId().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        ArgumentId argumentId() const final;

        const std::string& name() const;

        bool optional() const;

        std::pair<unsigned, unsigned> count() const;
    private:
        const ArgumentData* m_Argument;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <vector>

/**
 * @file
 * @brief Defines the OptionView class.
 */

namespace Argos
{
    struct OptionData;

    /**
     * @brief Provides read-only access to an option definition.
     */
    class OptionView : public IArgumentView
    {
    public:
        /**
         * @private
         * @brief For internal use only.
         *
         * Client code can only receive objects, not construct them.
         */
        explicit OptionView(const OptionData* data);

        /**
         * @brief Returns the option's or option's help text.
         */
        [[nodiscard]] const std::string& text() const final;

        /**
         * @brief Returns the option's section name.
         */
        [[nodiscard]] const std::string& section() const final;

        /**
         * @brief Returns the option's value name.
         */
        [[nodiscard]] const std::string& value() const final;

        /**
         * @brief Returns the option's visibility in
         *      the help text and error messages.
         */
        [[nodiscard]] Visibility visibility() const final;

        /**
         * @brief Returns the option's custom id.
         */
        [[nodiscard]] int id() const final;

        /**
         * @brief Returns the numeric id of the value the argument assigns
         *      or appends to.
         *
         * This value is created internally in Argos and must not be
         * confused with the customizable value returned by id().
         * If different options or arguments have the same value name, they
         * will also have the same value id.
         *
         * @return options with operation OptionOperation::NONE have
         *      a value of 0, all other options and arguments have a value
         *      greater than 0.
         */
        [[nodiscard]] ValueId valueId() const final;

        /**
         * @brief Returns the option's argumentId().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        [[nodiscard]] ArgumentId argumentId() const final;

        [[nodiscard]] OptionOperation operation() const;

        [[nodiscard]] const std::vector<std::string>& flags() const;

        [[nodiscard]] const std::string& argument() const;

        [[nodiscard]] const std::string& constant() const;

        [[nodiscard]] OptionType type() const;

        [[nodiscard]] bool optional() const;
    private:
        const OptionData* m_Option;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <memory>
#include <optional>

/**
 * @file
 * @brief Defines the ArgumentValue class.
 */

namespace Argos
{
    class ParsedArgumentsImpl;
    class ArgumentValues;

    /**
     * @brief Wrapper class for the value of an argument or option.
     *
     * @a ParsedArguments and @a ArgumentValues returns instances of
     * @a ArgumentValue.
     */
    class ArgumentValue
    {
    public:
        /**
         * @private
         */
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParsedArgumentsImpl> args,
                      ValueId valueId,
                      ArgumentId argumentId);

        /**
         * @private
         */
        ArgumentValue(const ArgumentValue&);

        /**
         * @private
         */
        ArgumentValue(ArgumentValue&&) noexcept;

        /**
         * @private
         */
        ~ArgumentValue();

        /**
         * @private
         */
        ArgumentValue& operator=(const ArgumentValue&);

        /**
         * @private
         */
        ArgumentValue& operator=(ArgumentValue&&) noexcept;

        /**
         * @brief Returns the @a IArgumentView instance which identifies
         *      the argument or option that is the source of this value.
         */
        [[nodiscard]] std::unique_ptr<IArgumentView> argument() const;

        /**
         * @brief Returns true if this argument or option was given on the
         *      command line.
         */
        [[nodiscard]] bool hasValue() const;

        /**
         * @brief Returns the value as it was found on the command line.
         *
         * The optional is empty if the value was not given. The optional
         * will never be empty if the current ArgumentValue instance
         * was returned by ArgumentValues::value(n).
         */
        [[nodiscard]] std::optional<std::string_view> value() const;

        /**
         * @brief Returns defaultValue if the value was not given on the
         *      command line, otherwise it returns true unless the given value
         *      is "0" or "false".
         */
        [[nodiscard]] bool asBool(bool defaultValue = false) const;

        /**
         * @brief Converts the value from the command line to int and returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to int, an error message
         * is displayed and the program either exits (autoExit is true) or
         * the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtol and @a base is passed unmodified
         * to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to int.
         */
        [[nodiscard]] int asInt(int defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned int
         *      and returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned int, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtoul and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to unsigned int.
         */
        [[nodiscard]]
        unsigned asUInt(unsigned defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to long and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to long, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtol and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to long.
         */
        [[nodiscard]] long asLong(long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to long long and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to long long, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtoll and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to long long.
         */
        [[nodiscard]]
        long long asLLong(long long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned long and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned long, an error
         * message is displayed and the program either exits (autoExit is true)
         * or the function throws an exception (autoExit is false). The actual
         * conversion is performed by @a strtoul and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to unsigned long.
         */
        [[nodiscard]] unsigned long
        asULong(unsigned long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned long
         *      long and returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned long long,
         * an error message is displayed and the program either exits (autoExit
         * is true) or the function throws an exception (autoExit is false).
         * The actual conversion is performed by @a strtoul and @a base is
         * passed unmodified to that function.
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to unsigned long.
         */
        [[nodiscard]] unsigned long long
        asULLong(unsigned long long defaultValue = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to float and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to float, an error message
         * is displayed and the program either exits (autoExit is true) or the
         * function throws an exception (autoExit is false).
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to float.
         */
        [[nodiscard]] float asFloat(float defaultValue = 0) const;

        /**
         * @brief Converts the value from the command line to double and
         *      returns it.
         *
         * Returns defaultValue if the value was not given on the command line.
         *
         * If the given value can not be converted to double, an error message
         * is displayed and the program either exits (autoExit is true) or the
         * function throws an exception (autoExit is false).
         *
         * @throw ArgosException if @a autoExit is false and the given value
         *      can not be converted to double.
         */
        [[nodiscard]] double asDouble(double defaultValue = 0) const;

        /**
         * @brief Returns the value from the command line as a string.
         *
         * Returns defaultValue if the value was not given on the command line.
         */
        [[nodiscard]] std::string
        asString(const std::string& defaultValue = {}) const;

        /**
         * @brief Splits the string from the command line on @a separator and
         *      returns the resulting parts.
         *
         * An error message is displayed if the result has less than
         * @a minParts parts (i.e. number of separators is less than
         * minParts - 1). The result will never consist of more than
         * @a maxParts parts, even if there are more occurrences of
         * @a separator in the value, it just means that the final part will
         * contain one or more separators. The only exception is if
         * @a maxParts is zero (the default), which means there is no upper
         * limit to the number of parts.
         *
         * @throw ArgosException if @a autoExit is false and the result
         *      has less than @a minParts parts.
         */
        [[nodiscard]] ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;

        /**
         * Display @a message as if it was an error produced within Argos
         * itself, including a reference to the argument or option this value
         * comes from and the usage section from the help text. If autoExit is
         * true the program will exit after displaying the message.
         */
        void error(const std::string& message) const;

        /**
         * Calls error(message) with a message that says the given value
         * is invalid.
         */
        void error() const;
    private:
        std::optional<std::string_view> m_Value;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId;
        ArgumentId m_ArgumentId;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the ArgumentValues class.
 */

namespace Argos
{
    class ArgumentValue;
    class ParsedArgumentsImpl;

    /**
     * @brief Wrapper class for the values of a multi-value argument
     *      or option.
     *
     * ParsedArguments returns instances of ArgumentValues.
     */
    class ArgumentValues
    {
    public:
        /**
         * @private
         */
        ArgumentValues(std::vector<std::pair<std::string_view, ArgumentId>> values,
                       std::shared_ptr<ParsedArgumentsImpl> args,
                       ValueId valueId);

        ArgumentValues(const ArgumentValues&);

        ArgumentValues(ArgumentValues&&) noexcept;

        ~ArgumentValues();

        ArgumentValues& operator=(const ArgumentValues&);

        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        void error(const std::string& message) const;

        bool empty() const;

        size_t size() const;

        std::vector<ArgumentValue> values() const;

        std::vector<std::string_view> rawValues() const;

        ArgumentValue value(size_t index) const;

        std::vector<int>
        asInts(const std::vector<int>& defaultValue = {},
               int base = 10) const;

        std::vector<unsigned>
        asUInts(const std::vector<unsigned>& defaultValue = {},
               int base = 10) const;

        std::vector<long>
        asLongs(const std::vector<long>& defaultValue = {},
                int base = 10) const;

        std::vector<unsigned long>
        asULongs(const std::vector<unsigned long>& defaultValue = {},
                 int base = 10) const;

        std::vector<long long>
        asLLongs(const std::vector<long long>& defaultValue = {},
                 int base = 10) const;

        std::vector<unsigned long long>
        asULLongs(const std::vector<unsigned long long>& defaultValue = {},
                  int base = 10) const;

        std::vector<float> asFloats(
                const std::vector<float>& defaultValue = {}) const;

        std::vector<double> asDoubles(
                const std::vector<double>& defaultValue = {}) const;

        std::vector<std::string> asStrings(
                const std::vector<std::string>& defaultValue = {}) const;

        ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;
    private:
        std::vector<std::pair<std::string_view, ArgumentId>> m_Values;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the ParsedArgumentsBuilder class.
 */

namespace Argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief An interface to ParsedArguments that lets argument and option
     *      callbacks query and modify the parsed argument values
     */
    class ParsedArgumentsBuilder
    {
    public:
        /**
         * @private
         */
        explicit ParsedArgumentsBuilder(
                std::shared_ptr<ParsedArgumentsImpl> impl);

        ParsedArgumentsBuilder& append(const std::string& name,
                                       const std::string& value);

        ParsedArgumentsBuilder& append(const IArgumentView& arg,
                                       const std::string& value);

        ParsedArgumentsBuilder& assign(const std::string& name,
                                       const std::string& value);

        ParsedArgumentsBuilder& assign(const IArgumentView& arg,
                                       const std::string& value);

        ParsedArgumentsBuilder& clear(const std::string& name);

        ParsedArgumentsBuilder& clear(const IArgumentView& arg);

        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        [[nodiscard]] bool has(const std::string& name) const;

        [[nodiscard]] bool has(const IArgumentView& arg) const;

        void error(const std::string& errorMessage);

        void error(const std::string& errorMessage, const IArgumentView& arg);

        /**
         * @brief Returns the stream that was assigned to the
         *  ArgumentParser.
         *
         * Returns a reference to the default stream (std::cout) if none
         * has been assigned.
         */
        [[nodiscard]] std::ostream& stream() const;

        /**
         * @brief Returns the program name that was assigned to the
         *  ArgumentParser.
         */
        [[nodiscard]] const std::string& programName() const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <functional>

/**
 * @file
 * @brief Defines ArgumentCallback and OptionCallback.
 */

namespace Argos
{
    /**
     * @brief A callback that is called each time given arguments appear
     *      on the command line.
     *
     * The three parameters are:
     * - ArgumentView: the argument that was encountered (particularly
     *   useful if the same function has been registered with multiple
     *   arguments).
     * - std::string_view: the raw value of the argument. Note that this
     *   value can also be retrieved via the ParsedArgumentsBuilder.
     * - ParsedArgumentsBuilder: this object can be used to read or modify
     *   the values of arguments and options.
     */
    using ArgumentCallback = std::function<bool(ArgumentView,
                                                std::string_view,
                                                ParsedArgumentsBuilder)>;

    /**
     * @brief A callback that is called each time given options appear
     *      on the command line.
     *
     * The three parameters are:
     * - OptionView: the option that was encountered (particularly
     *   useful if the same function has been registered with multiple
     *   options).
     * - std::string_view: the raw value of the option if the option actually
     *   has one. Note that this value can also be retrieved via the
     *   ParsedArgumentsBuilder if the option operation is ASSIGN or APPEND.
     * - ParsedArgumentsBuilder: this object can be used to read or modify
     *   the values of arguments and options.
     */
    using OptionCallback = std::function<bool(OptionView,
                                              std::string_view,
                                              ParsedArgumentsBuilder)>;
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the Argument class.
 */

namespace Argos
{
    struct ArgumentData;

    /**
     * @brief Class for defining command line arguments.
     *
     * The minimum requirement for a command line argument is that it has a
     * name. Once the argument has been defined it must be *added* to the
     * ArgumentParser with ArgumentParser::add.
     */
    class Argument
    {
    public:
        /**
         * @brief Creates an unnamed argument.
         */
        Argument();

        /**
         * @brief Creates an argument with name @a name.
         * @param name The name that will be displayed in the help text as
         *      well as the name used when retrieving the argument's value from
         *      ParsedArguments.
         */
        explicit Argument(const std::string& name);

        /**
         * @brief Creates a complete copy of the given argument.
         */
        Argument(const Argument&);

        /**
         * @brief Moves the innards of the given argument to the new object.
         *
         * Attempts to use the old object will result in an exception.
         */
        Argument(Argument&&) noexcept;

        ~Argument();

        /**
         * @brief Copies everything in the given argument.
         */
        Argument& operator=(const Argument&);

        /**
         * @brief Moves the innards of the given argument to the current
         * object.
         *
         * Attempts to use the old object will result in an exception.
         */
        Argument& operator=(Argument&&) noexcept;

        /**
         * @brief Set the argument's help text.
         * @param text The text will be automatically divided into multiple
         *      lines if it doesn't fit fit inside the terminal window.
         *      Text formatting using newlines, spaces and tabs is possible.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& text(const std::string& text);

        /**
         * @brief Specifies under which heading the argument will appear
         *      in the help text.
         *
         * The default heading for arguments is "ARGUMENTS".
         * @param name All arguments and options with the same section name
         *      will be listed under the same heading.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& section(const std::string& name);

        /**
         * @brief Set an alternative name for the value this argument
         *      assigns to.
         *
         * The value or values of the argument can be retrieved from
         * ParsedArgument using the displayed name, but sometimes this
         * is inconvenient, for instance if the same argument has different
         * names in different languages.
         * @param id An alternative name that can be used to retrieve the
         *      argument's value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& value(const std::string& id);

        /**
         * @brief Set a callback that will be called when this argument is
         *      encountered.
         * @param callback A function pointer or callable object accepting the
         *      parameters (ArgumentView, string_view, ParsedArgumentsBuilder).
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& callback(ArgumentCallback callback);

        /**
         * @brief Set restrictions for where this argument is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *      to quickly distinguish between different arguments.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& id(int id);

        /**
         * @brief Set the name of the argument.
         *
         * An alternative to supplying the name to the constructor.
         *
         * @param name The name that will be displayed in the help text as
         *      well as the name used when retrieving the argument's value from
         *      ParsedArguments.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& name(const std::string& name);

        /**
         * @brief Make this argument optional (or mandatory).
         *
         * All arguments are mandatory by default.
         *
         * This function is a convenience function that affects the argument's
         * minimum count.
         * @param optional
         *      @arg true The argument's minimum count is set to 0.
         *      @arg false The argument's minimum count is set to 1
         *          if it currently is 0.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& optional(bool optional);

        /**
         * @brief Set the number of times this argument must appear on the
         *      command line.
         * @param n This is both the minimum and the maximum count.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& count(unsigned n);

        /**
         * @brief Set the number of times this argument must appear on the
         *      command line.
         * @param minCount Can be any value.
         * @param maxCount Must be greater than or equal to @a minCount.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& count(unsigned minCount, unsigned maxCount);

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         * @return Pointer to the argument implementation.
         */
        std::unique_ptr<ArgumentData> release();
    private:
        void checkArgument() const;

        std::unique_ptr<ArgumentData> m_Argument;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the ParsedArguments class.
 */

namespace Argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief The result of the ArgumentParser. Gives access to all argument
     *      and option values.
     */
    class ParsedArguments
    {
    public:
        ParsedArguments();

        /**
         * @private
         */
        explicit ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl);

        /**
         * @private
         */
        ParsedArguments(const ParsedArguments&) = delete;

        /**
         * @private
         */
        ParsedArguments(ParsedArguments&&) noexcept;

        /**
         * @private
         */
        ~ParsedArguments();

        /**
         * @private
         */
        ParsedArguments& operator=(const ParsedArguments&) = delete;

        ParsedArguments& operator=(ParsedArguments&&) noexcept;

        [[nodiscard]] bool has(const std::string& name) const;

        [[nodiscard]] bool has(const IArgumentView& arg) const;

        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        [[nodiscard]]
        std::vector<std::unique_ptr<ArgumentView>> allArguments() const;

        [[nodiscard]]
        std::vector<std::unique_ptr<OptionView>> allOptions() const;

        [[nodiscard]] ParserResultCode resultCode() const;

        [[nodiscard]] OptionView stopOption() const;

        [[nodiscard]]
        const std::vector<std::string>& unprocessedArguments() const;

        void filterParsedArguments(int& argc, char**& argv);
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the ArgumentIterator class.
 */
namespace Argos
{
    class ArgumentIteratorImpl;
    struct ParserData;

    /**
     * @brief Iterator class created by ArgumentParser that lets client code
     *      process one argument or option at a time.
     */
    class ArgumentIterator
    {
    public:
        /**
         * @brief Constructs a new instance of ArgumentIterator.
         *
         * Client code must use ArgumentParser::makeIterator().
         */
        ArgumentIterator(std::vector<std::string_view> args,
                         std::shared_ptr<ParserData> parserData);

        /**
         * @private
         */
        ArgumentIterator(const ArgumentIterator&) = delete;

        /**
         * @brief Moves the innards of the old object to the new one.
         *
         * Any attempt to use the old object will result in an exception.
         */
        ArgumentIterator(ArgumentIterator&&) noexcept;

        ~ArgumentIterator();

        /**
         * @private
         */
        ArgumentIterator& operator=(const ArgumentIterator&) = delete;

        /**
         * @brief Moves the innards of the object on the right hand side
         *      to the one on the left hand side.
         *
         * Any attempt to use the old object will result in an exception.
         */
        ArgumentIterator& operator=(ArgumentIterator&&) noexcept;

        /**
         * @brief Process the next argument or option and return the result.
         *
         * ParsedArguments has been updated when the function returns.
         *
         * @param arg The definition (ArgumentView or OptionView) of the
         *      processed argument or option. If ignoreUndefinedArguments
         *      or ignoreUndefinedOptions is true, this pointer can be empty.
         * @param value If @a arg is an argument then this is the argument's
         *      value. If @a arg is an option that take an argument then this
         *      is the option's value. If @a arg is empty (i.e. this is an
         *      undefined argument or option) then this is the unrecognized
         *      argument value or option flag. Otherwise @a value is empty.
         * @return true If an argument or option was processed successfully,
         *      false if there were no more arguments or an error was
         *      encountered.
         */
        bool next(std::unique_ptr<IArgumentView>& arg,
                  std::string_view& value);

        /**
         * @brief Gives access to all the arguments and options processed
         *      so far.
         *
         * The returned object is "live" in the sense that it is updated
         * behind the scenes each time next() is called. It is therefore
         * sufficient to only call this function once and keep a copy of
         * the returned object.
         */
        [[nodiscard]]
        ParsedArguments parsedArguments() const;
    private:
        ArgumentIteratorImpl& impl();

        const ArgumentIteratorImpl& impl() const;

        std::unique_ptr<ArgumentIteratorImpl> m_Impl;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <initializer_list>

/**
 * @file
 * @brief Defines the Option class.
 */

namespace Argos
{
    struct OptionData;

    /**
     * @brief Class for defining command line arguments.
     *
     * The minimum requirement for a command line argument is that it has a
     * name. Once the argument has been defined it must be *added* to the
     * ArgumentParser with ArgumentParser::add.
     */
    class Option
    {
    public:
        Option();

        explicit Option(std::initializer_list<std::string> flags);

        /**
         * @private
         */
        Option(const Option&);

        /**
         * @private
         */
        Option(Option&&) noexcept;

        /**
         * @private
         */
        ~Option();

        /**
         * @private
         */
        Option& operator=(const Option&);

        /**
         * @private
         */
        Option& operator=(Option&&) noexcept;

        /**
         * @brief Set the option's help text.
         * @param text The text will be automatically divided into multiple
         *      lines if it doesn't fit fit inside the terminal window.
         *      Text formatting using newlines, spaces and tabs is possible.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& text(const std::string& text);

        /**
         * @brief Specifies under which heading the option will appear
         *      in the help text.
         *
         * The default heading for options is "OPTIONS".
         * @param name All arguments and options with the same section name
         *      will be listed under the same heading.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& section(const std::string& name);

        /**
         * @brief Set an alternative name for the value this option
         *      assigns to.
         *
         * The value or values of the option can be retrieved from
         * ParsedArgument using one of its flags, but sometimes this
         * is inconvenient, for instance if the same option has different
         * names in different languages, or multiple options share the same
         * value.
         * @param id An alternative name that can be used to retrieve the
         *      option's value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& value(const std::string& id);

        /**
         * @brief Set a callback that will be called when this option is
         *      encountered.
         * @param callback A function pointer or callable object accepting the
         *      parameters (OptionView, string_view, ParsedArgumentsBuilder).
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& callback(OptionCallback callback);

        /**
         * @brief Set restrictions for where this option is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *      to quickly distinguish between different options.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& id(int id);

        Option& operation(OptionOperation operation);

        Option& flag(const std::string& f);

        Option& flags(std::vector<std::string> f);

        Option& argument(const std::string& name);

        Option& constant(const char* value);

        Option& constant(const std::string& value);

        Option& constant(bool value);

        Option& constant(int value);

        Option& constant(long long value);

        Option& type(OptionType type);

        Option& optional(bool optional);

        const OptionData& data() const;

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         * @return Pointer to the option implementation.
         */
        std::unique_ptr<OptionData> release();
    private:
        void checkOption() const;

        std::unique_ptr<OptionData> m_Option;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iosfwd>

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
                                bool extractFileName = true);

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
        ArgumentParser& about(std::string text);

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
        void writeHelpText() const;

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

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-05-01.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Include this file to make all of Argos's public interface available.
 */
