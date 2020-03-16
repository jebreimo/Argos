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

        /**
         * @brief Passes @a message on to the base class.
         */
        explicit ArgosException(const std::string& message) noexcept
            : std::runtime_error(message)
        {}

        /**
         * @brief Prefixes @a message with source file, line number
         * and function name.
         * @param message The error message.
         * @param fileName Typically the value of __FILE__ where the
         *      exception was thrown.
         * @param lineno Typically the value of __LINE__ where the
         *      exception was thrown.
         * @param funcName Typically the value of __func__ where the
         *      exception was thrown.
         */
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

/**
 * @brief String representation of the complete version number.
 */
constexpr char ARGOS_VERSION[] = "0.99.1";

/**
 * @brief Incremented if a new version is significantly incompatible
 *      with the previous version.
 */
constexpr unsigned ARGOS_VERSION_MAJOR = 0;

/**
 * @brief Incremented when Argos's interface is modified without introducing
 *      incompatibilities with previous versions.
 */
constexpr unsigned ARGOS_VERSION_MINOR = 99;

/**
 * @brief Incremented when Argos's internals are modified without modifying
 *      its interface.
 */
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
        virtual const std::string& valueName() const = 0;

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
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <memory>
#include <optional>
#include <vector>

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
     * ParsedArguments returns instances of ArgumentValue.
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
        const std::string& valueName() const final;

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
        const std::string& text() const final;

        /**
         * @brief Returns the option's section name.
         */
        const std::string& section() const final;

        /**
         * @brief Returns the option's value name.
         */
        const std::string& valueName() const final;

        /**
         * @brief Returns the option's visibility in
         *      the help text and error messages.
         */
        Visibility visibility() const final;

        /**
         * @brief Returns the option's custom id.
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
         * @brief Returns the option's argumentId().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
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
        ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl);

        /**
         * @private
         */
        ParsedArguments(const ParsedArguments&) = delete;

        ParsedArguments(ParsedArguments&&) noexcept;

        ~ParsedArguments();

        /**
         * @private
         */
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
     *   ParsedArgumentsBuilder.
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

        Option& value(const std::string& value);

        Option& value(bool value);

        Option& value(int value);

        Option& value(double value);

        Option& type(OptionType type);

        Option& mandatory(bool mandatory);

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
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
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
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
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
