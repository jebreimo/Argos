//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

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
        ArgosException() noexcept
            : std::runtime_error("Unspecified error.")
        {}

        explicit ArgosException(const std::string& message) noexcept
            : std::runtime_error(message)
        {}

        ArgosException(const std::string& message,
                       const std::string& fileName,
                       int lineno,
                       const std::string& funcName)
            : std::runtime_error(funcName + "() in " + fileName + ":"
                                 + std::to_string(lineno) + ": " + message)
        {}
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-03-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

/**
 * @file
 * @brief Defines the current Argos version.
 */

constexpr char ARGOS_VERSION[] = "0.99.1";
constexpr unsigned ARGOS_VERSION_MAJOR = 0;
constexpr unsigned ARGOS_VERSION_MINOR = 99;
constexpr unsigned ARGOS_VERSION_PATCH = 1;

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
         *      of type STOP (or HELP of autoExit is false).
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
        TEXT,
        /**
         * @brief The title of the list of arguments (default is "ARGUMENTS").
         */
        ARGUMENTS_TITLE,
        /**
         * @brief The title of the list of options (default is "OPTIONS").
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

    enum ValueId : int;

    enum ArgumentId : int;
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class IArgumentView
    {
    public:
        virtual ~IArgumentView() = default;

        virtual const std::string& text() const = 0;

        virtual const std::string& section() const = 0;

        virtual const std::string& valueName() const = 0;

        virtual Visibility visibility() const = 0;

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

        virtual ArgumentId argumentId() const = 0;
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
#include <vector>

namespace Argos
{
    class ParsedArgumentsImpl;
    class ArgumentValues;

    class ArgumentValue
    {
    public:
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParsedArgumentsImpl> args,
                      ValueId valueId,
                      ArgumentId argumentId);

        ArgumentValue(const ArgumentValue&);

        ArgumentValue(ArgumentValue&&) noexcept;

        ~ArgumentValue();

        ArgumentValue& operator=(const ArgumentValue&);

        ArgumentValue& operator=(ArgumentValue&&) noexcept;

        std::unique_ptr<IArgumentView> argument() const;

        bool hasValue() const;

        std::optional<std::string_view> value() const;

        bool asBool(bool defaultValue = false) const;

        int8_t asInt8(int8_t defaultValue = 0) const;

        int16_t asInt16(int16_t defaultValue = 0) const;

        int32_t asInt32(int32_t defaultValue = 0) const;

        int64_t asInt64(int64_t defaultValue = 0) const;

        uint8_t asUint8(uint8_t defaultValue = 0) const;

        uint16_t asUint16(uint16_t defaultValue = 0) const;

        uint32_t asUint32(uint32_t defaultValue = 0) const;

        uint64_t asUint64(uint64_t defaultValue = 0) const;

        float asFloat(float defaultValue = 0) const;

        double asDouble(double defaultValue = 0) const;

        long double asLongDouble(long double defaultValue = 0) const;

        std::string asString(const std::string& defaultValue = {}) const;

        ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;

        void error(const std::string& message) const;
    private:
        template <typename T>
        T getValue(const T& defaultValue) const;

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

namespace Argos
{
    class ArgumentValue;
    class ParsedArgumentsImpl;

    class ArgumentValues
    {
    public:
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

        std::vector<int8_t> asInt8s(
                const std::vector<int8_t>& defaultValue = {}) const;

        std::vector<int16_t> asInt16s(
                const std::vector<int16_t>& defaultValue = {}) const;

        std::vector<int32_t> asInt32s(
                const std::vector<int32_t>& defaultValue = {}) const;

        std::vector<int64_t> asInt64s(
                const std::vector<int64_t>& defaultValue = {}) const;

        std::vector<uint8_t> asUint8s(
                const std::vector<uint8_t>& defaultValue = {}) const;

        std::vector<uint16_t> asUint16s(
                const std::vector<uint16_t>& defaultValue = {}) const;

        std::vector<uint32_t> asUint32s(
                const std::vector<uint32_t>& defaultValue = {}) const;

        std::vector<uint64_t> asUint64s(
                const std::vector<uint64_t>& defaultValue = {}) const;

        std::vector<float> asFloats(
                const std::vector<float>& defaultValue = {}) const;

        std::vector<double> asDoubles(
                const std::vector<double>& defaultValue = {}) const;

        std::vector<long double> asLongDoubles(
                const std::vector<long double>& defaultValue = {}) const;

        std::vector<std::string> asStrings(
                const std::vector<std::string>& defaultValue = {}) const;

        ArgumentValues
        split(char separator, size_t minParts = 0, size_t maxParts = 0) const;
    private:
        template <typename T>
        std::vector<T> getValues(const std::vector<T>& defaultValue) const;

        std::vector<std::pair<std::string_view, ArgumentId>> m_Values;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        ValueId m_ValueId;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    struct ArgumentData;

    class ArgumentView : public IArgumentView
    {
    public:
        explicit ArgumentView(const ArgumentData* data);

        const std::string& text() const final;

        const std::string& section() const final;

        const std::string& valueName() const final;

        Visibility visibility() const final;

        int id() const final;

        ValueId valueId() const final;

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

namespace Argos
{
    struct OptionData;

    class OptionView : public IArgumentView
    {
    public:
        explicit OptionView(const OptionData* data);

        const std::string& text() const final;

        const std::string& section() const final;

        const std::string& valueName() const final;

        Visibility visibility() const final;

        int id() const final;

        ValueId valueId() const final;

        ArgumentId argumentId() const final;

        OptionOperation operation() const;

        const std::vector<std::string>& flags() const;

        const std::string& argument() const;

        const std::string& value() const;

        OptionType type() const;

        bool mandatory() const;
    private:
        const OptionData* m_Option;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class ParsedArgumentsImpl;

    class ParsedArguments
    {
    public:
        ParsedArguments();

        ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl);

        ParsedArguments(const ParsedArguments&) = delete;

        ParsedArguments(ParsedArguments&&) noexcept;

        ~ParsedArguments();

        ParsedArguments& operator=(const ParsedArguments&) = delete;

        ParsedArguments& operator=(ParsedArguments&&) noexcept;

        bool has(const std::string& name) const;

        bool has(const IArgumentView& arg) const;

        ArgumentValue value(const std::string& name) const;

        ArgumentValue value(const IArgumentView& arg) const;

        ArgumentValues values(const std::string& name) const;

        ArgumentValues values(const IArgumentView& arg) const;

        std::vector<std::unique_ptr<ArgumentView>> allArguments() const;

        std::vector<std::unique_ptr<OptionView>> allOptions() const;

        ParserResultCode resultCode() const;

        OptionView stopOption() const;

        const std::vector<std::string>& unprocessedArguments() const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_Impl;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class ParsedArgumentsImpl;

    class ParsedArgumentsBuilder
    {
    public:
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

        ArgumentValue value(const std::string& name);

        ArgumentValue value(const IArgumentView& arg);

        ArgumentValues values(const std::string& name);

        ArgumentValues values(const IArgumentView& arg);

        bool has(const std::string& name);

        bool has(const IArgumentView& arg);

        void error(const std::string& errorMessage);

        void error(const std::string& errorMessage,
                   const IArgumentView& arg);
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

        ArgumentIterator(const ArgumentIterator&) = delete;

        ArgumentIterator(ArgumentIterator&&) noexcept;

        ~ArgumentIterator();

        ArgumentIterator& operator=(const ArgumentIterator&) = delete;

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
// Created by Jan Erik Breimo on 2020-02-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <functional>

namespace Argos
{
    using ArgumentCallback = std::function<bool(ArgumentView,
                                                std::string_view,
                                                ParsedArgumentsBuilder)>;

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
         * Creates an argument with name @a name.
         * @param name The name that will be displayed in the help text as
         *      well as the name used when retrieving the argument's value from
         *      ParsedArguments.
         */
        explicit Argument(const std::string& name);

        Argument(const Argument&);

        Argument(Argument&&) noexcept;

        ~Argument();

        Argument& operator=(const Argument&);

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
        Argument& valueName(const std::string& id);

        /**
         * @brief Set a callback that will be called when this argument is
         *      encountered.
         * @param callback A function pointer or callable object.
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
         * @param optional @arg true The argument's minimum count is set to 0.
         *                 @arg false The argument's minimum count is set to 1
         *                 if it currently is 0.
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
         * @brief Used internally in Argos.
         *
         * The Argument instance is no longer usable after this function has
         * been called.
         * @return Pointer to the argument implementation.
         */
        std::unique_ptr<ArgumentData> release();
    private:
        std::unique_ptr<ArgumentData> m_Argument;
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

        Option(const Option&);

        Option(Option&&) noexcept;

        ~Option();

        Option& operator=(const Option&);

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
        Option& valueName(const std::string& id);

        /**
         * @brief Set a callback that will be called when this option is
         *      encountered.
         * @param callback A function pointer or callable object.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& callback(OptionCallback callback);

        Option& operation(OptionOperation operation);

        /**
         * @brief Set restrictions for where this option is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Option& visibility(Visibility visibility);

        Option& id(int id);

        Option& flag(const std::string& f);

        Option& flags(std::vector<std::string> f);

        Option& argument(const std::string& name);

        Option& value(const std::string& value);

        Option& value(bool value);

        Option& value(int value);

        Option& value(double value);

        Option& type(OptionType type);

        Option& mandatory(bool mandatory);

        const OptionData& data() const;

        std::unique_ptr<OptionData> release();
    private:
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

        explicit ArgumentParser(const std::string& programName);

        ArgumentParser(ArgumentParser&&) noexcept;

        ArgumentParser(const ArgumentParser&) = delete;

        ~ArgumentParser();

        ArgumentParser& operator=(ArgumentParser&&) noexcept;

        ArgumentParser& operator=(const ArgumentParser&) = delete;

        ArgumentParser& add(Argument argument);

        ArgumentParser& add(Option option);

        ParsedArguments parse(int argc, char* argv[]);

        ParsedArguments parse(int argc, char* argv[]) const;

        ParsedArguments parse(std::vector<std::string_view> args);

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
        const ParserData& data() const;

        ParserData& data();

        ArgumentId nextArgumentId() const;

        std::unique_ptr<ParserData> m_Data;
    };
}
