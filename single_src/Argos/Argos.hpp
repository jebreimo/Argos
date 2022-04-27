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
constexpr char ARGOS_VERSION[] = "1.1.262";

/**
 * @brief Incremented when a new version contains significant changes. It
 *  might be necessary to update code that has been compiled with previous
 *  the version of Argos.
 */
#define ARGOS_VERSION_MAJOR 1

/**
 * @brief Incremented when Argos's interface is modified in ways that are
 *  compatible with existing client code.
 */
#define ARGOS_VERSION_MINOR 1

/**
 * @brief Incremented when the changes does not affect the interface.
 */
#define ARGOS_VERSION_PATCH 260

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
namespace argos
{
    /**
     * @brief The exception class used throughout Argos.
     */
    class ArgosException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
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

namespace argos
{
    /**
     * @brief The different option styles supported by Argos.
     */
    enum class OptionStyle
    {
        /**
         * @brief Options starts with either one dash (`-`) followed by
         *  exactly one character (short option) or two dashes (`--`) followed by
         *  one or more characters (long option).
         *
         * Short options can be concatenated making `-pq` and `-p -q`
         * equivalent as long as neither of them take an argument.
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

namespace argos
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
        virtual const std::string& help() const = 0;

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
        virtual ValueId value_id() const = 0;

        /**
         * @brief Returns the argument's or option's argument_id().
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        virtual ArgumentId argument_id() const = 0;
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

namespace argos
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
        [[nodiscard]] const std::string& help() const final;

        /**
         * @brief Returns the argument's section name.
         */
        [[nodiscard]] const std::string& section() const final;

        /**
         * @brief Returns the argument's value name.
         */
        [[nodiscard]] const std::string& value() const final;

        /**
         * @brief Returns the argument's visibility in
         *      the help text and error messages.
         */
        [[nodiscard]] Visibility visibility() const final;

        /**
         * @brief Returns the argument's custom id.
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
        [[nodiscard]] ValueId value_id() const final;

        /**
         * @brief Returns the argument's argument id.
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        [[nodiscard]] ArgumentId argument_id() const final;

        /**
         * @brief Returns the argument's name.
         */
        [[nodiscard]] const std::string& name() const;

        /**
         * @brief Returns true if the argument is optional (i.e. its minimum
         *  count is 0).
         */
        [[nodiscard]] bool optional() const;

        /**
         * @brief Returns the argument's minimum and maximum counts.
         *
         * Normal arguments have both set to 1.
         */
        [[nodiscard]] std::pair<unsigned, unsigned> count() const;
    private:
        const ArgumentData* m_argument;
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

namespace argos
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
        [[nodiscard]] const std::string& help() const final;

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
        [[nodiscard]] ValueId value_id() const final;

        /**
         * @brief Returns the option's argument id.
         *
         * This id is assigned and used internally to uniquely identify
         * each argument and option.
         */
        [[nodiscard]] ArgumentId argument_id() const final;

        /**
         * @brief Returns the option's operation.
         */
        [[nodiscard]] OptionOperation operation() const;

        /**
         * @brief Returns the option's flags.
         */
        [[nodiscard]] const std::vector<std::string>& flags() const;

        /**
         * @brief Returns the option's argument.
         */
        [[nodiscard]] const std::string& argument() const;

        /**
         * @brief Returns the option's initial value.
         */
        [[nodiscard]] const std::string& initial_value() const;

        /**
         * @brief Returns the option's constant.
         *
         * @note The constant is stored as a string internally, even if the
         *  option was assigned an integer or boolean value.
         */
        [[nodiscard]] const std::string& constant() const;

        /**
         * @brief Returns the option's type.
         */
        [[nodiscard]] OptionType type() const;

        /**
         * @brief Returns false if the option is mandatory.
         */
        [[nodiscard]] bool optional() const;
    private:
        const OptionData* m_option;
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

namespace argos
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
        ArgumentValue();

        /**
         * @private
         */
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParsedArgumentsImpl> args,
                      ValueId value_id,
                      ArgumentId argument_id);

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
         * @brief Returns true if this argument or option was given on the
         *      command line.
         */
        [[nodiscard]]
        explicit operator bool() const;

        /**
         * @brief Returns the @a IArgumentView instance which identifies
         *      the argument or option that is the source of this value.
         */
        [[nodiscard]] std::unique_ptr<IArgumentView> argument() const;

        /**
         * @brief Returns the value as it was found on the command line.
         *
         * The optional is empty if the value was not given. The optional
         * will never be empty if the current ArgumentValue instance
         * was returned by ArgumentValues::value(n).
         */
        [[nodiscard]] std::optional<std::string_view> value() const;

        /**
         * @brief Returns default_value if the value was not given on the
         *      command line, otherwise it returns true unless the given value
         *      is "0" or "false".
         */
        [[nodiscard]] bool as_bool(bool default_value = false) const;

        /**
         * @brief Converts the value from the command line to int and returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to int, an error message
         * is displayed and the program either exits (auto_exit is true) or
         * the function throws an exception (auto_exit is false). The actual
         * conversion is performed by @a strtol and @a base is passed unmodified
         * to that function.
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to int.
         */
        [[nodiscard]] int as_int(int default_value = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned int
         *      and returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned int, an error
         * message is displayed and the program either exits (auto_exit is true)
         * or the function throws an exception (auto_exit is false). The actual
         * conversion is performed by @a strtoul and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to unsigned int.
         */
        [[nodiscard]]
        unsigned as_uint(unsigned default_value = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to long and
         *      returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to long, an error
         * message is displayed and the program either exits (auto_exit is true)
         * or the function throws an exception (auto_exit is false). The actual
         * conversion is performed by @a strtol and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to long.
         */
        [[nodiscard]] long as_long(long default_value = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to long long and
         *      returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to long long, an error
         * message is displayed and the program either exits (auto_exit is true)
         * or the function throws an exception (auto_exit is false). The actual
         * conversion is performed by @a strtoll and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to long long.
         */
        [[nodiscard]]
        long long as_llong(long long default_value = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned long and
         *      returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned long, an error
         * message is displayed and the program either exits (auto_exit is true)
         * or the function throws an exception (auto_exit is false). The actual
         * conversion is performed by @a strtoul and @a base is passed
         * unmodified to that function.
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to unsigned long.
         */
        [[nodiscard]] unsigned long
        as_ulong(unsigned long default_value = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to unsigned long
         *      long and returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to unsigned long long,
         * an error message is displayed and the program either exits (auto_exit
         * is true) or the function throws an exception (auto_exit is false).
         * The actual conversion is performed by @a strtoul and @a base is
         * passed unmodified to that function.
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to unsigned long.
         */
        [[nodiscard]] unsigned long long
        as_ullong(unsigned long long default_value = 0, int base = 10) const;

        /**
         * @brief Converts the value from the command line to float and
         *      returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to float, an error message
         * is displayed and the program either exits (auto_exit is true) or the
         * function throws an exception (auto_exit is false).
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to float.
         */
        [[nodiscard]] float as_float(float default_value = 0) const;

        /**
         * @brief Converts the value from the command line to double and
         *      returns it.
         *
         * Returns default_value if the value was not given on the command line.
         *
         * If the given value can not be converted to double, an error message
         * is displayed and the program either exits (auto_exit is true) or the
         * function throws an exception (auto_exit is false).
         *
         * @throw ArgosException if @a auto_exit is false and the given value
         *      can not be converted to double.
         */
        [[nodiscard]] double as_double(double default_value = 0) const;

        /**
         * @brief Returns the value from the command line as a string.
         *
         * Returns default_value if the value was not given on the command line.
         */
        [[nodiscard]] std::string
        as_string(const std::string& default_value = {}) const;

        /**
         * @brief Splits the string from the command line on @a separator and
         *      returns the resulting parts.
         *
         * An error message is displayed if the result has less than
         * @a min_parts parts (i.e. number of separators is less than
         * min_parts - 1). The result will never consist of more than
         * @a max_parts parts, even if there are more occurrences of
         * @a separator in the value, it just means that the final part will
         * contain one or more separators. The only exception is if
         * @a max_parts is zero (the default), which means there is no upper
         * limit to the number of parts.
         *
         * @throw ArgosException if @a auto_exit is false and the result
         *      has less than @a min_parts parts.
         */
        [[nodiscard]] ArgumentValues
        split(char separator, size_t min_parts = 0, size_t max_parts = 0) const;

        /**
         * Display @a message as if it was an error produced within Argos
         * itself, including a reference to the argument or option this value
         * comes from and the usage section from the help text. If auto_exit is
         * true the program will exit after displaying the message.
         */
        void error(const std::string& message) const;

        /**
         * Calls error(message) with a message that says the given value
         * is invalid.
         */
        void error() const;
    private:
        std::optional<std::string_view> m_value;
        std::shared_ptr<ParsedArgumentsImpl> m_args;
        ValueId m_value_id;
        ArgumentId m_argument_id;
    };
}

//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iterator>
#include <string_view>

/**
 * @file
 * Defines ArgumentValueIterator, an input iterator suitable for range-based
 * for loops over the values in instances of ArgumentValues.
 */

namespace argos
{
    class ArgumentValue;
    class ParsedArgumentsImpl;

    /**
     * @brief Iterator for the values in an instance of ArgumentValues.
     *
     * Direct use of this iterator should be avoided, it is intended to
     * be used in range-based for loops. For algorithms etc. it is recommended
     * to use the vector returned by ArgumentValues::values.
     */
    class ArgumentValueIterator
    {
    public:
        /**
         * @private
         */
        using It = typename std::vector<
            std::pair<std::string_view, ArgumentId>
            >::const_iterator;

        /**
         * @brief Construct an empty iterator.
         *
         * The iterator has to be assigned the result of ArgumentValues::begin
         * or ArgumentValues::end before it can be used.
         */
        ArgumentValueIterator();

        /**
         * @private
         * Only called from ArgumentValues
         */
        ArgumentValueIterator(const It& internal_iterator,
                              std::shared_ptr<ParsedArgumentsImpl> args,
                              ValueId value_id);

        /**
         * @brief Prefix increment operator.
         */
        ArgumentValueIterator& operator++();

        /**
         * @brief Postfix increment operator.
         */
        ArgumentValueIterator operator++(int);

        /**
         * @brief Returns the current value.
         *
         * @note The returned value is not a reference.
         */
        ArgumentValue operator*() const;

        /**
         * @private
         */
        It internal_iterator() const;
    private:
        It m_iterator = {};
        std::shared_ptr<ParsedArgumentsImpl> m_args;
        ValueId m_value_id = {};
    };

    /**
     * @brief Returns true @a a and @a b point to the same argument.
     */
    bool operator==(const ArgumentValueIterator& a,
                    const ArgumentValueIterator& b);

    /**
     * @brief Returns false unless @a a and @a b point to the same argument.
     */
    bool operator!=(const ArgumentValueIterator& a,
                    const ArgumentValueIterator& b);
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

namespace argos
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
                       ValueId value_id);

        /**
         * @private
         */
        ArgumentValues(const ArgumentValues&);

        /**
         * @private
         */
        ArgumentValues(ArgumentValues&&) noexcept;

        /**
         * @private
         */
        ~ArgumentValues();

        /**
         * @private
         */
        ArgumentValues& operator=(const ArgumentValues&);

        /**
         * @private
         */
        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        /**
         * @brief Returns true if there is at least one value.
         */
        [[nodiscard]]
        explicit operator bool() const;

        /**
         * @brief Returns instances of IArgumentView that identifies the
         *  command line arguments that produced these values.
         */
        std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        /**
         * Display @a message as if it was an error produced within Argos
         * itself, including a reference to the argument or option this value
         * comes from and the usage section from the help text. If auto_exit is
         * true the program will exit after displaying the message, an
         * exception is thrown if it's not.
         */
        [[noreturn]]
        void error(const std::string& message) const;

        /**
         * @brief Returns true if there are no values.
         */
        bool empty() const;

        /**
         * @brief Returns the number of values.
         */
        size_t size() const;

        /**
         * @brief Returns a vector with all the values.
         */
        std::vector<ArgumentValue> values() const;

        /**
         * @brief Returns a vector with all the raw string_view values.
         */
        std::vector<std::string_view> raw_values() const;

        /**
         * @brief Returns the value with the given index.
         *
         * If @a index is to great, an error message is written to stderr, the
         * program also automatically exits if auto_exit is true.
         *
         * @throw ArgosException if @a index is too great and auto_exit
         *  is false.
         */
        ArgumentValue value(size_t index) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to int.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<int>
        as_ints(const std::vector<int>& default_value = {},
                int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to unsigned.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<unsigned>
        as_uints(const std::vector<unsigned>& default_value = {},
                 int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to long.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<long>
        as_longs(const std::vector<long>& default_value = {},
                 int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to unsigned long.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<unsigned long>
        as_ulongs(const std::vector<unsigned long>& default_value = {},
                  int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to long long.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @param base See the documentation for std::strtoint for details.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<long long>
        as_llongs(const std::vector<long long>& default_value = {},
                  int base = 10) const;

        /**
        * @brief Returns a vector where every argument value has been
        *  converted to unsigned long long.
        *
        * If any of the argument values can't be converted, an error
        * message is written to stderr, the program also automatically exits
        * if auto_exit is true.
        *
        * @param default_value This vector is returned if there are no values.
        * @param base See the documentation for std::strtoint for details.
        * @throw ArgosException if the conversion fails for any value and
        *  auto_exit is false.
        */
        std::vector<unsigned long long>
        as_ullongs(const std::vector<unsigned long long>& default_value = {},
                   int base = 10) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to float.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<float>
        as_floats(const std::vector<float>& default_value = {}) const;

        /**
         * @brief Returns a vector where every argument value has been
         *  converted to double.
         *
         * If any of the argument values can't be converted, an error
         * message is written to stderr, the program also automatically exits
         * if auto_exit is true.
         *
         * @param default_value This vector is returned if there are no values.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<double>
        as_doubles(const std::vector<double>& default_value = {}) const;

        /**
         * @brief Returns a vector with the argument values.
         *
         * @param default_value This vector is returned if there are no values.
         * @throw ArgosException if the conversion fails for any value and
         *  auto_exit is false.
         */
        std::vector<std::string>
        as_strings(const std::vector<std::string>& default_value = {}) const;

        /**
         * @brief Splits each value on @a separator and returns the parts in
         *  a single list.
         * @param separator The separator.
         * @param min_parts The minimum number of parts each value must
         *  consist of.
         * @param max_parts The maximum number of parts any value can
         *  consist of. The final part will retain all excessive separators.
         * @throw ArgosException if any value consists of less than
         *  @a min_parts parts.
         */
        ArgumentValues
        split(char separator, size_t min_parts = 0,
              size_t max_parts = 0) const;

        /**
         * @brief Returns an iterator pointing to the first value.
         */
        ArgumentValueIterator begin() const;

        /**
         * @brief Returns an iterator pointing to the end of the last value.
         */
        ArgumentValueIterator end() const;
    private:
        std::vector<std::pair<std::string_view, ArgumentId>> m_values;
        std::shared_ptr<ParsedArgumentsImpl> m_args;
        ValueId m_value_id;
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

namespace argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief An interface to ParsedArguments that lets argument and option
     *      callbacks query and modify the parsed argument values
     *
     * Eaxmple:
     * ~~~{.cpp}
     * ArgumentParser()
     *     .add(Option{"--foo"}...)
     *     .add(Option{"--bar"}...)
     *     .add(Option{"--baz"}.argument("NUMBER")...)
     *     .add(Option{"--all"}
     *         .text("Enables --foo and --bar, and sets --baz to 11.")
     *         .callback([](auto, auto, ParsedArguments pa)
     *             {
     *                 pa.assign("--foo", "true").assign("--bar", "true")
     *                   .assign("--baz", "11");
     *                 return true;
     *             }))
     *         ...
     * ~~~
     */
    class ParsedArgumentsBuilder
    {
    public:
        /**
         * @private
         */
        explicit ParsedArgumentsBuilder(
                std::shared_ptr<ParsedArgumentsImpl> impl);

        /**
         * @brief Add @a value to the named argument or option.
         * @param name The name (flag, alias etc.) of an argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" or "false" for *false* and "1" or "true" for *true*.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        ParsedArgumentsBuilder& append(const std::string& name,
                                       const std::string& value);

        /**
         * @brief Add @a value to the given argument or option.
         * @param arg An argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" for *false* and "1" for *true*.
         */
        ParsedArgumentsBuilder& append(const IArgumentView& arg,
                                       const std::string& value);

        /**
         * @brief Set the value of the named argument or option.
         *
         * Any previous value or values are replaced by @a value.
         *
         * @param name The name (flag, alias etc.) of an argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" for *false* and "1" for *true*.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        ParsedArgumentsBuilder& assign(const std::string& name,
                                       const std::string& value);

        /**
         * @brief Set the value of the given argument or option.
         *
         * Any previous value or values are replaced by @a value.
         *
         * @param arg An argument or option.
         * @param value Even if the values will be read as integers or floats,
         *  they must be given as strings. Boolean values must be given as
         *  "0" for *false* and "1" for *true*.
         */
        ParsedArgumentsBuilder& assign(const IArgumentView& arg,
                                       const std::string& value);

        /**
         * @brief Removes the value or values of the named argument or option.
         *
         * After it's been cleared, the argument or value will be treated as
         * if it hasn't been given any value at all, i.e. the default value
         * will be returned when its value is retrieved with
         * ArgumentValue::as_string et al.
         *
         * @param name The name (flag, alias etc.) of an argument or option.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        ParsedArgumentsBuilder& clear(const std::string& name);

        /**
         * @brief Removes the value or values of the named argument or option.
         *
         * After it's been cleared, the argument or value will be treated as
         * if it hasn't been given any value at all, i.e. the default value
         * will be returned when its value is retrieved with
         * ArgumentValue::as_string et al.
         */
        ParsedArgumentsBuilder& clear(const IArgumentView& arg);

        /**
         * @brief Returns the value of the named argument or option.
         * @throw ArgosException if the argument or option has multiple
         *  values, or the name doesn't match any of names, flags or aliases
         *  of the arguments and options added to ArgumentParser.
         */
        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        /**
         * @brief Returns the value of the given argument or option.
         * @throw ArgosException if the argument or option has multiple
         *  values.
         */
        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        /**
         * @brief Returns the value of the named argument or option.
         * @throw ArgosException if the name doesn't match any of names,
         *  flags or aliases of the arguments and options added
         *  to ArgumentParser.
         */
        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        /**
         * @brief Returns the values of the given argument or option.
         */
        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        /**
         * @brief Returns true if the named argument or option has been
         *  assigned a value.
         */
        [[nodiscard]] bool has(const std::string& name) const;

        /**
         * @brief Returns true if the @a arg has been assigned a value.
         */
        [[nodiscard]] bool has(const IArgumentView& arg) const;

        /**
         * @brief Print @a msg along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::auto_exit is false.
         */
        [[noreturn]] void error(const std::string& msg);

        /**
         * @brief Print @a msg prefixed by the argument's name or option's
         *  flags along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::auto_exit is false.
         */
        [[noreturn]]
        void error(const std::string& msg, const IArgumentView& arg);

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
        [[nodiscard]] const std::string& program_name() const;
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_impl;
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

namespace argos
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
    using ArgumentCallback = std::function<void(ArgumentView,
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
    using OptionCallback = std::function<void(OptionView,
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

namespace argos
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
        Argument& help(const std::string& text);

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
        Argument& alias(const std::string& id);

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
         * @param min_count Can be any value.
         * @param max_count Must be greater than or equal to @a min_count.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Argument& count(unsigned min_count, unsigned max_count);

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
        void check_argument() const;

        std::unique_ptr<ArgumentData> m_argument;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <ostream>

/**
 * @file
 * @brief Defines the ParsedArguments class.
 */

namespace argos
{
    class ParsedArgumentsImpl;

    /**
     * @brief The result of the ArgumentParser. Gives access to all argument
     *      and option values.
     *
     * Instances of this class is returned by ArgumentParser::parse and
     * ArgumentIterator::parsed_arguments.
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

        /**
         * @private
         */
        ParsedArguments& operator=(ParsedArguments&&) noexcept;

        /**
         * @brief Returns true if the argument or option named @a name
         *  was given on command line.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] bool has(const std::string& name) const;

        /**
         * @brief Returns true if the given argument instance was given a
         *  value on command line.
         */
        [[nodiscard]] bool has(const IArgumentView& arg) const;

        /**
         * @brief Returns the value of the argument with the given name.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] ArgumentValue value(const std::string& name) const;

        /**
         * @brief Returns the value of the given argument.
         */
        [[nodiscard]] ArgumentValue value(const IArgumentView& arg) const;

        /**
         * @brief Returns the values of the argument with the given name.
         *
         * @throw ArgosException if @a name doesn't match the name of any
         *  argument or option.
         */
        [[nodiscard]] ArgumentValues values(const std::string& name) const;

        /**
         * @brief Returns the value of the given argument.
         */
        [[nodiscard]] ArgumentValues values(const IArgumentView& arg) const;

        /**
         * @brief Returns all argument definitions that were registered with
         *  ArgumentParser.
         *
         * Intended for testing and debugging, for instance to list all
         * defined arguments along with their given values.
         */
        [[nodiscard]]
        std::vector<std::unique_ptr<ArgumentView>> all_arguments() const;

        /**
         * @brief Returns all option definitions that were registered with
         *  ArgumentParser.
         *
         * Intended for testing and debugging, for instance to list all
         * defined options along with their given values.
         */
        [[nodiscard]]
        std::vector<std::unique_ptr<OptionView>> all_options() const;

        /**
         * @brief Returns the parser result code.
         */
        [[nodiscard]] ParserResultCode result_code() const;

        /**
         * @brief If the parser stopped early because it encountered an option
         *  of type, this function returns that option.
         */
        [[nodiscard]] OptionView stop_option() const;

        /**
         * @brief Returns the command line arguments that were ignored by the
         *  argument parser.
         *
         * This function will always return an empty vector unless at least
         * one of the following is true:
         *
         * - ArgumentParser::ignore_undefined_arguments is true.
         * - ArgumentParser::ignore_undefined_options is true.
         * - ArgumentParser::auto_exit is false and there are options with type
         *   set to OptionType::STOP.
         */
        [[nodiscard]]
        const std::vector<std::string>& unprocessed_arguments() const;

        /**
         * @brief Removes parsed arguments and options from @a argv and
         *  decrements @a argc correspondingly.
         *
         * The first value in @a argv is assumed to be the program name and
         * is ignored, the remainder should be identical to the command line
         * given to ArgumentParser::parse or ArgumentParser::make_iterator.
         *
         * @note No memory is freed, the function only rearranges the pointers
         *  @a in argv.
         */
        void filter_parsed_arguments(int& argc, char**& argv);

        /**
         * @brief Print @a msg along with a brief help text and exit.
         *
         * @throw ArgosException if ArgumentParser::auto_exit is false.
         */
        [[noreturn]]
        void error(const std::string& msg);
    private:
        std::shared_ptr<ParsedArgumentsImpl> m_impl;
    };

    /**
     * @brief Write a list of all arguments and options along with their
     *  values to stdout.
     *
     * This function is intended for testing and debugging.
     */
    void print(const ParsedArguments& args);

    /**
     * @brief Write a list of all arguments and options along with their
     *  values to @a stream.
     *
     * This function is intended for testing and debugging.
     */
    void print(const ParsedArguments& args, std::ostream& stream);
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
namespace argos
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
         * @private
         * @brief Constructs a new instance of ArgumentIterator.
         *
         * Client code must use ArgumentParser::make_iterator().
         */
        ArgumentIterator(std::vector<std::string_view> args,
                         std::shared_ptr<ParserData> parser_data);

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
         *      processed argument or option. If ignore_undefined_arguments
         *      or ignore_undefined_options is true, this pointer can be empty.
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
        ParsedArguments parsed_arguments() const;
    private:
        ArgumentIteratorImpl& impl();

        const ArgumentIteratorImpl& impl() const;

        std::unique_ptr<ArgumentIteratorImpl> m_impl;
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

namespace argos
{
    struct OptionData;

    /**
     * @brief Class for defining command line options.
     *
     * The minimum requirement for a command line option is that it has at
     * least one flag. Once the option has been defined it must be *added* to
     * the ArgumentParser with ArgumentParser::add.
     */
    class Option
    {
    public:
        /**
         * @brief Creates a new option without any flags.
         *
         * An option created this way must be assigned at least one flag with
         * Option::flag or Option::flags before it can be added to the
         * ArgumentParser.
         */
        Option();

        /**
         * @brief Creates a new option with the given flags.
         */
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
         *  lines if it doesn't fit fit inside the terminal window.
         *  Text formatting using newlines, spaces and tabs is possible.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& help(const std::string& text);

        /**
         * @brief Specifies under which heading the option will appear
         *  in the help text.
         *
         * The default heading for options is "OPTIONS".
         * @param name All arguments and options with the same section name
         *  will be listed under the same heading.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& section(const std::string& name);

        /**
         * @brief Set an alternative name for the value this option
         *  assigns to.
         *
         * The value or values of the option can be retrieved from
         * ParsedArgument using one of its flags, but sometimes this
         * is inconvenient, for instance if the same option has different
         * names in different languages, or multiple options share the same
         * value.
         *
         *
         * The alias can be any string, including a flag or the name of
         * an argument. If two options, or an option and and argument, refer to
         * each other through an alias, their values become linked. For instance
         * if there are two options
         * --verbose and --quiet that negates each other, one of them, but not
         * both, should have the other option as an alias (e.g. --quiet has
         * an alias("--verbose")).
         *
         * ~~~{.cpp}
         * ArgumentParser()
         *     .add(Option{"-q", "--quiet"}.alias("--verbose").constant(false))
         *     .add(Option{"-v", "--verbose"})
         *     ...
         * ~~~
         *
         * The same effect is also produced if they have a common alias:
         * ~~~{.cpp}
         * ArgumentParser()
         *     .add(Option{"-q", "--quiet"}.alias("Verbose").constant(false))
         *     .add(Option{"-v", "--verbose"}.alias("Verbose"))
         *     ...
         * ~~~
         *
         * @param id An alternative name that can be used to retrieve the
         *  option's value.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& alias(const std::string& id);

        /**
         * @brief Set a callback that will be called when this option is
         *  encountered.
         * @param callback A function pointer or callable object accepting the
         *  parameters (OptionView, string_view, ParsedArgumentsBuilder).
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& callback(OptionCallback callback);

        /**
         * @brief Set restrictions for where this option is displayed in the
         *  auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *  to quickly distinguish between different options.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value, but makes it available through IArgumentView.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& id(int id);

        /**
         * @brief Specify which operation the option performs. The default
         *  operations is ASSIGN.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& operation(OptionOperation operation);

        /**
         * @brief Set the flag of a single-flag option.
         * @param f a flag with one or two leading dashes or a slash depending
         *  on the option style (ArgumentParser::option_style).
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& flag(const std::string& f);

        /**
         * @brief Set the flag of a multi-flag option.
         * @param f flags with one or two leading dashes or a slash depending
         *  on the option style (ArgumentParser::option_style).
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& flags(std::vector<std::string> f);

        /**
         * @brief Set the name of the option's argument (a value given on
         *  the command line).
         *
         * The argument is displayed in the help text and error messages.
         * Setting the argument is how one informs the ArgumentParser that
         * this option takes an argument from the command line rather than
         * assign a constant.
         *
         * @param name a string that will appear in the help text, typically
         *  something like FILE or NUMBER.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& argument(const std::string& name);

        /**
         * @brief Sets a value that will be assigned to the option before
         *  arguments are parsed.
         *
         * @note This is not the recommended way to specify a default value,
         *  prefer instead to use the defaultValue argument to ArgumentValue's
         *  as_string() et al. ParsedArguments can not distinguish between
         *  values assigned through initial_value() and actual command line
         *  arguments. APPEND-options will not overwrite the initial value,
         *  but add its values after it.
         *
         *  The initial_value can be useful if for instance one wants to use
         *  ArgumentValue's split function on a default value read from a
         *  file or an environment variable, or want to prepend
         *  a value to a list built by options of operation "APPEND".
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& initial_value(const std::string& value);

        /**
         * @brief Sets the value that this option will assign to the
         *  corresponding value in ParsedArguments.
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& constant(const char* value);

        /**
         * @brief Sets the value that this option will assign to the
         *  corresponding value in ParsedArguments.
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& constant(const std::string& value);

        /**
         * @brief Sets the value that this option will assign to the
         *  corresponding value in ParsedArguments.
         *
         * Options that have no argument and no explicit constant will
         * automatically have the constant *true*.

         * @note All values are stored as strings internally, true and false are
         *  converted to "1" and "0" respectively..
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& constant(bool value);

        /**
         * @brief Sets the value that this option will assign to the
         *  corresponding value in ParsedArguments.
         *
         * @note All values are stored as strings internally, i.e. constant(123) and
         *  constant("123") are equivalent.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& constant(int value);

        /**
         * @brief Sets the value that this option will assign to the underlying value.
         *
         * @note All values are stored as strings internally, i.e. constant(123LL) and
         *  constant("123") are equivalent.
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& constant(long long value);

        /**
         * @brief Sets the option type.
         *
         * All options have the type OptionType::NORMAL by default.
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& type(OptionType type);

        /**
         * @brief Set whether this option is optional or mandatory.
         *
         * @return Reference to itself. This makes it possible to chain
         *  method calls.
         */
        Option& optional(bool optional);

        /**
         * @private
         * @brief Used internally in Argos.
         */
        [[nodiscard]]
        const OptionData& data() const;

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         *
         * @return Pointer to the option implementation.
         */
        std::unique_ptr<OptionData> release();
    private:
        void check_option() const;

        std::unique_ptr<OptionData> m_option;
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
        ArgumentParser& add(Argument argument);

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
         * @brief Returns true if option flags are case insensitive.
         */
        [[nodiscard]] bool case_insensitive() const;

        /**
         * @brief Enable or disable case insensitive option flags.
         *
         * @note Case-insensitivity will only work for ASCII-letters (i.e.
         *      a-z and A-Z).
         */
        ArgumentParser& case_insensitive(bool value);

        /**
         * @brief Returns whether or not a help option will be auto-generated
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
         * By default this is on.
         */
        ArgumentParser& generate_help_option(bool value);

        /**
         * @brief Returns the current option style.
         */
        [[nodiscard]] OptionStyle option_style() const;

        /**
         * @brief Set the option style.
         *
         * @note The option style can not be changed once any options have been
         *      added.
         */
        ArgumentParser& option_style(OptionStyle value);

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
         * By default this is an empty function object.
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
         * By default this is an empty function object.
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
         * By default std::cout is used for the help text and std::cerr for
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
         * @note The program name can also be set by the constructor.
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
         * @brief Sets the line width for help text and error messages.
         *
         * The line width defaults to the width of the console or terminal
         * window.
         */
        ArgumentParser& line_width(unsigned line_width);

        /**
         * @brief Write the help text.
         *
         * @note The help text is displayed automatically when a help option
         *      is used.
         */
        void write_help_text() const;

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
         * @brief Makes it possible to construct an ArgumentParser with chained
         *      method calls and assign it to a variable.
         */
        ArgumentParser&& move();
    private:
        void check_data() const;

        [[nodiscard]] ArgumentId next_argument_id() const;

        std::unique_ptr<ParserData> m_data;
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
