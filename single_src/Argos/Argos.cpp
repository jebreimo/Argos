#include "Argos.hpp"
//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#define _ARGOS_THROW_3(file, line, msg) \
    throw ::argos::ArgosException(file ":" #line ": " msg)

#define _ARGOS_THROW_2(file, line, msg) \
    _ARGOS_THROW_3(file, line, msg)

#define ARGOS_THROW(msg) \
    _ARGOS_THROW_2(__FILE__, __LINE__, msg)

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <string>

namespace argos
{
    struct ArgumentData
    {
        std::string name;
        std::string help;
        std::string section;
        std::string value;
        ArgumentCallback callback;
        unsigned min_count = 1;
        unsigned max_count = 1;
        Visibility visibility = Visibility::NORMAL;
        int id = 0;
        ValueId value_id = {};
        ArgumentId argument_id = {};
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    Argument::Argument()
        : m_argument(std::make_unique<ArgumentData>())
    {}

    Argument::Argument(const std::string& name)
        : m_argument(std::make_unique<ArgumentData>())
    {
        m_argument->name = name;
    }

    Argument::Argument(const Argument& rhs)
        : m_argument(rhs.m_argument
                     ? std::make_unique<ArgumentData>(*rhs.m_argument)
                     : std::unique_ptr<ArgumentData>())
    {}

    Argument::Argument(Argument&& rhs) noexcept
        : m_argument(std::move(rhs.m_argument))
    {}

    Argument::~Argument() = default;

    Argument& Argument::operator=(const Argument& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_argument)
                m_argument = std::make_unique<ArgumentData>(*rhs.m_argument);
            else
                m_argument = {};
        }
        return *this;
    }

    Argument& Argument::operator=(Argument&& rhs) noexcept
    {
        m_argument = std::move(rhs.m_argument);
        return *this;
    }

    Argument& Argument::help(const std::string& text)
    {
        check_argument();
        m_argument->help = text;
        return *this;
    }

    Argument& Argument::section(const std::string& name)
    {
        check_argument();
        m_argument->section = name;
        return *this;
    }

    Argument& Argument::alias(const std::string& id)
    {
        check_argument();
        m_argument->value = id;
        return *this;
    }

    Argument& Argument::callback(ArgumentCallback callback)
    {
        check_argument();
        m_argument->callback = move(callback);
        return *this;
    }

    Argument& Argument::visibility(Visibility visibility)
    {
        check_argument();
        m_argument->visibility = visibility;
        return *this;
    }

    Argument& Argument::id(int id)
    {
        check_argument();
        m_argument->id = id;
        return *this;
    }

    Argument& Argument::name(const std::string& name)
    {
        check_argument();
        m_argument->name = name;
        return *this;
    }

    Argument& Argument::optional(bool optional)
    {
        check_argument();
        if (optional)
            m_argument->min_count = 0;
        else if (m_argument->min_count == 0)
            m_argument->min_count = 1;
        return *this;
    }

    Argument& Argument::count(unsigned n)
    {
        if (n <= 0)
            ARGOS_THROW("Argument's count must be greater than 0.");
        check_argument();
        m_argument->min_count = m_argument->max_count = n;
        return *this;
    }

    Argument& Argument::count(unsigned min_count, unsigned max_count)
    {
        if (max_count == 0)
            ARGOS_THROW("Argument's max count must be greater than 0.");
        if (max_count < min_count)
            ARGOS_THROW("Argument's max count cannot be less than its min count.");
        check_argument();
        m_argument->min_count = min_count;
        m_argument->max_count = max_count;
        return *this;
    }

    std::unique_ptr<ArgumentData> Argument::release()
    {
        check_argument();
        return std::move(m_argument);
    }

    void Argument::check_argument() const
    {
        if (!m_argument)
            ARGOS_THROW("Cannot use Argument instance after"
                        " release() has been called.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <memory>
#include <vector>

namespace argos
{
    class ArgumentCounter
    {
    public:
        ArgumentCounter();

        explicit ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                size_t argument_count);

        const ArgumentData* next_argument();

        size_t count() const;

        bool is_complete() const;

        static std::pair<size_t, size_t> get_min_max_count(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        static bool requires_argument_count(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);
    private:
        using Counter = std::pair<size_t, const ArgumentData*>;
        std::vector<Counter> m_counters;
        size_t m_index = 0;
        size_t m_first_optional = 0;
        size_t m_counter = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <optional>

namespace argos
{
    namespace
    {
        size_t find_first_optional(
            const std::vector<std::unique_ptr<ArgumentData>>& arguments)
        {
            size_t result = 0;
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                if (arguments[i]->min_count > 0)
                    result = i + 1;
            }
            return result;
        }

        void make_argument_counters(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                std::vector<std::pair<size_t, const ArgumentData*>>& counters,
                size_t& first_optional)
        {
            first_optional = find_first_optional(arguments);
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                auto& a = arguments[i];
                if (i + 1 == first_optional && a->min_count != a->max_count)
                {
                    counters.emplace_back(a->min_count, a.get());
                    counters.emplace_back(a->max_count - a->min_count, a.get());
                }
                else
                {
                    counters.emplace_back(a->max_count, a.get());
                }
            }
        }

        std::vector<std::pair<size_t, const ArgumentData*>>
        make_argument_counters(
            const std::vector<std::unique_ptr<ArgumentData>>& arguments,
            size_t n)
        {
            auto minmax = ArgumentCounter::get_min_max_count(arguments);
            if (n < minmax.first)
                n = 0;
            else if (n > minmax.second)
                n = minmax.second - minmax.first;
            else
                n -= minmax.first;

            std::vector<std::pair<size_t, const ArgumentData*>> result;
            for (auto& arg : arguments)
            {
                if (n == 0 || arg->min_count == arg->max_count)
                {
                    result.emplace_back(arg->min_count, arg.get());
                }
                else if (arg->min_count + n <= arg->max_count)
                {
                    result.emplace_back(arg->min_count + n, arg.get());
                    n = 0;
                }
                else
                {
                    result.emplace_back(arg->max_count, arg.get());
                    n -= arg->max_count - arg->min_count;
                }
            }
            return result;
        }
    }

    ArgumentCounter::ArgumentCounter()
        : m_counters()
    {}

    ArgumentCounter::ArgumentCounter(
        const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        make_argument_counters(arguments, m_counters, m_first_optional);
    }

    ArgumentCounter::ArgumentCounter(
        const std::vector<std::unique_ptr<ArgumentData>>& arguments,
        size_t argument_count)
        : m_counters(make_argument_counters(arguments, argument_count)),
          m_first_optional(m_counters.size())
    {}

    const ArgumentData* ArgumentCounter::next_argument()
    {
        while (m_index != m_counters.size() && m_counters[m_index].first == 0)
            ++m_index;

        if (m_index == m_counters.size())
            return nullptr;

        ++m_counter;
        --m_counters[m_index].first;
        return m_counters[m_index].second;
    }

    size_t ArgumentCounter::count() const
    {
        return m_counter;
    }

    bool ArgumentCounter::is_complete() const
    {
        return m_index >= m_first_optional
               || (m_index + 1 == m_first_optional
                   && m_counters[m_index].first == 0);
    }

    std::pair<size_t, size_t>
    ArgumentCounter::get_min_max_count(
        const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        size_t lo = 0, hi = 0;
        for (auto& arg : arguments)
        {
            lo += arg->min_count;
            if (hi != SIZE_MAX)
            {
                if (arg->max_count > SIZE_MAX - hi)
                    hi = SIZE_MAX;
                else
                    hi += arg->max_count;
            }
        }
        return {lo, hi};
    }

    bool ArgumentCounter::requires_argument_count(
            const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        bool deterministic = true;
        for (auto& arg : arguments)
        {
            if (!deterministic)
                return true;
            if (arg->min_count != arg->max_count)
                deterministic = false;
        }
        return false;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    struct OptionData
    {
        std::vector<std::string> flags;
        std::string help;
        std::string section;
        std::string alias;
        std::string argument;
        std::string constant;
        std::string initial_value;
        OptionCallback callback;
        OptionOperation operation = OptionOperation::ASSIGN;
        OptionType type = OptionType::NORMAL;
        Visibility visibility = Visibility::NORMAL;
        bool optional = true;
        int id = 0;
        ArgumentId argument_id = {};
        ValueId value_id = {};
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iosfwd>
#include <string_view>

namespace argos
{
    class TextWriter
    {
    public:
        explicit TextWriter(unsigned line_width = 80);

        std::ostream* stream() const;

        void set_stream(std::ostream* stream);

        unsigned indentation() const;

        bool set_indentation(unsigned indent);

        bool write(std::string_view str, bool force = false);

        void newline();

        void flush();

        void tab();

        unsigned spaces() const;

        void set_spaces(unsigned n);

        unsigned current_width() const;

        unsigned remaining_width() const;

        bool is_current_line_empty() const;

        unsigned line_width() const;

        void set_line_width(unsigned width);

        std::string_view currentLine() const;
    private:
        std::ostream* m_stream;
        std::string m_line;
        unsigned m_line_width;
        unsigned m_current_line_width = 0;
        unsigned m_tab_size = 4;
        unsigned m_indent = 0;
        unsigned m_spaces = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <list>
#include <map>

namespace argos
{
    class WordSplitter
    {
    public:
        void add_word(std::string word_rule);

        std::tuple<std::string_view, char, std::string_view>
        split(std::string_view word, size_t start_index, size_t max_length,
              bool must_split) const;
    private:
        std::tuple<std::string_view, char, std::string_view>
        default_rule(std::string_view word, size_t max_length) const;

        struct Split
        {
            unsigned index;
            char separator;
        };
        std::map<std::string_view, std::vector<Split>> m_splits;
        std::list<std::string> m_strings;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <climits>

namespace argos
{
    class TextFormatter
    {
    public:
        TextFormatter();

        explicit TextFormatter(std::ostream* stream);

        TextFormatter(std::ostream* stream, unsigned line_width);

        WordSplitter& word_splitter();

        std::ostream* stream() const;

        void set_stream(std::ostream* stream);

        unsigned line_width() const;

        void set_line_width(unsigned line_width);

        unsigned current_line_width() const;

        bool is_current_line_empty() const;

        static constexpr unsigned CURRENT_COLUMN = UINT_MAX;

        void push_indentation(unsigned indent);

        void pop_indentation();

        void write_words(std::string_view text);

        void write_lines(std::string_view text);

        void newline();

        void flush();
    private:
        void append_word(std::string_view word);

        void begin_alignment();

        void update_alignment(const std::string_view& token);

        void end_alignment();

        TextWriter m_writer;
        std::vector<unsigned> m_indents;
        WordSplitter m_word_splitter;
        enum class State
        {
            NO_ALIGNMENT,
            ALIGNMENT,
            UNALIGNED_MARKER,
            ALIGNED_MARKER
        };
        State m_state = State::NO_ALIGNMENT;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <variant>

#ifndef ARGOS_EX_USAGE
    #ifdef EX_USAGE
        #define ARGOS_EX_USAGE EX_USAGE
    #else
        #define ARGOS_EX_USAGE 64
    #endif
#endif

namespace argos
{
    struct ParserSettings
    {
        ArgumentCallback argument_callback;
        OptionCallback option_callback;
        OptionStyle option_style = OptionStyle::STANDARD;
        bool auto_exit = true;
        bool allow_abbreviated_options = false;
        bool ignore_undefined_options = false;
        bool ignore_undefined_arguments = false;
        bool case_insensitive = false;
        bool generate_help_option = true;
        int normal_exit_code = 0;
        int error_exit_code = ARGOS_EX_USAGE;
    };

    struct HelpSettings
    {
        std::string program_name;
        std::string version;
        std::map<TextId, std::string> texts;
        std::ostream* output_stream = nullptr;
    };

    struct ParserData
    {
        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;

        ParserSettings parser_settings;
        HelpSettings help_settings;

        TextFormatter text_formatter;

        std::string current_section;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class IOptionIterator
    {
    public:
        virtual ~IOptionIterator() = default;

        virtual std::optional<std::string> next() = 0;

        virtual std::optional<std::string> next_value() = 0;

        virtual std::string_view current() const = 0;

        virtual std::vector<std::string_view> remaining_arguments() const = 0;

        virtual IOptionIterator* clone() const = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class StandardOptionIterator : public IOptionIterator
    {
    public:
        StandardOptionIterator();

        explicit StandardOptionIterator(std::vector<std::string_view> args);

        StandardOptionIterator(const StandardOptionIterator& rhs);

        std::optional<std::string> next() final;

        std::optional<std::string> next_value() final;

        std::string_view current() const final;

        std::vector<std::string_view> remaining_arguments() const final;

        IOptionIterator* clone() const final;
    private:
        std::vector<std::string_view> m_args;
        std::vector<std::string_view>::const_iterator m_args_it;
        size_t m_pos = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        explicit ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        bool has(ValueId value_id) const;

        const std::vector<std::string>& unprocessed_arguments() const;

        void add_unprocessed_argument(const std::string& arg);

        std::string_view assign_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id);

        std::string_view append_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id);

        void clear_value(ValueId value_id);

        ValueId get_value_id(std::string_view value_name) const;

        std::optional<std::pair<std::string_view, ArgumentId>>
        get_value(ValueId value_id) const;

        std::vector<std::pair<std::string_view, ArgumentId>>
        get_values(ValueId value_id) const;

        std::vector<std::unique_ptr<IArgumentView>>
        get_argument_views(ValueId value_id) const;

        std::unique_ptr<IArgumentView>
        get_argument_view(ArgumentId argument_id) const;

        const std::shared_ptr<ParserData>& parser_data() const;

        ParserResultCode result_code() const;

        void set_result_code(ParserResultCode result_code);

        const OptionData* stop_option() const;

        void set_breaking_option(const OptionData* option);

        [[noreturn]]
        void error(const std::string& message);

        [[noreturn]]
        void error(const std::string& message, ArgumentId argument_id);
    private:
        std::multimap<ValueId, std::pair<std::string, ArgumentId>> m_values;
        std::vector<std::tuple<std::string_view, ValueId, ArgumentId>> m_ids;
        std::vector<std::string> m_unprocessed_arguments;
        std::shared_ptr<ParserData> m_data;
        ParserResultCode m_result_code = ParserResultCode::NONE;
        const OptionData* m_stop_option = nullptr;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    enum class IteratorResultCode
    {
        ARGUMENT,
        OPTION,
        DONE,
        UNKNOWN,
        ERROR
    };

    using IteratorResult = std::tuple<
            IteratorResultCode,
            const void*,
            std::string_view>;

    class ArgumentIteratorImpl
    {
    public:
        ArgumentIteratorImpl(std::vector<std::string_view> args,
                             std::shared_ptr<ParserData> data);

        IteratorResult next();

        static std::shared_ptr<ParsedArgumentsImpl>
        parse(std::vector<std::string_view> args,
              const std::shared_ptr<ParserData>& data);

        const std::shared_ptr<ParsedArgumentsImpl>& parsed_arguments() const;
    private:
        enum class OptionResult
        {
            NORMAL,
            LAST_ARGUMENT,
            STOP,
            EXIT,
            ERROR
        };

        std::pair<OptionResult, std::string_view>
        process_option(const OptionData& opt, const std::string& flag);

        IteratorResult process_option(const std::string& flag);

        IteratorResult process_argument(const std::string& name);

        void copy_remaining_arguments_to_parser_result();

        size_t count_arguments() const;

        bool check_argument_and_option_counts();

        void error(const std::string& message = {});

        std::shared_ptr<ParserData> m_data;
        std::vector<std::pair<std::string_view, const OptionData*>> m_options;
        std::shared_ptr<ParsedArgumentsImpl> m_parsed_args;
        std::unique_ptr<IOptionIterator> m_iterator;
        ArgumentCounter m_argument_counter;
        enum class State
        {
            ARGUMENTS_AND_OPTIONS,
            ARGUMENTS_ONLY,
            DONE,
            ERROR
        };
        State m_state = State::ARGUMENTS_AND_OPTIONS;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args,
                                       std::shared_ptr<ParserData> parser_data)
        : m_impl(std::make_unique<ArgumentIteratorImpl>(move(args),
                                                        move(parser_data)))
    {}

    ArgumentIterator::ArgumentIterator(ArgumentIterator&& rhs) noexcept
        : m_impl(move(rhs.m_impl))
    {}

    ArgumentIterator::~ArgumentIterator() = default;

    ArgumentIterator&
    ArgumentIterator::operator=(ArgumentIterator&& rhs) noexcept
    {
        m_impl = move(rhs.m_impl);
        return *this;
    }

    bool ArgumentIterator::next(std::unique_ptr<IArgumentView>& arg,
                                std::string_view& value)
    {
        auto res = impl().next();
        switch (std::get<0>(res))
        {
        case IteratorResultCode::ARGUMENT:
            arg = std::make_unique<ArgumentView>(
                    static_cast<const ArgumentData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::OPTION:
            arg = std::make_unique<OptionView>(
                    static_cast<const OptionData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::UNKNOWN:
            arg = {};
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::DONE:
        case IteratorResultCode::ERROR:
            break;
        }
        arg = {};
        value = {};
        return false;
    }

    ParsedArguments ArgumentIterator::parsed_arguments() const
    {
        return ParsedArguments(impl().parsed_arguments());
    }

    ArgumentIteratorImpl& ArgumentIterator::impl()
    {
        if (!m_impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_impl;
    }

    const ArgumentIteratorImpl& ArgumentIterator::impl() const
    {
        if (!m_impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_impl;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    void write_help_text(ParserData& data);

    void write_error_message(ParserData& data, const std::string& msg);

    void write_error_message(ParserData& data,
                             const std::string& msg,
                             ArgumentId argument_id);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    bool are_equal_ci(std::string_view str1, std::string_view str2);

    bool are_equal(std::string_view str1, std::string_view str2,
                   bool case_insensitive);

    bool starts_with(std::string_view str, std::string_view prefix);

    bool starts_with_ci(std::string_view str, std::string_view prefix);

    bool starts_with(std::string_view str, std::string_view prefix,
                     bool case_insensitive);

    bool is_less_ci(std::string_view str1, std::string_view str2);

    bool is_less(std::string_view str1, std::string_view str2,
                 bool case_insensitive);

    std::vector<std::string_view>
    split_string(std::string_view s, char delimiter, size_t max_split);

    std::string_view get_base_name(std::string_view str);

    size_t count_code_points(std::string_view str);

    size_t find_nth_code_point(std::string_view str, size_t n);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class OptionIterator : public IOptionIterator
    {
    public:
        OptionIterator();

        explicit OptionIterator(std::vector<std::string_view> args,
                                char prefix);

        OptionIterator(const OptionIterator& rhs);

        std::optional<std::string> next() final;

        std::optional<std::string> next_value() final;

        std::string_view current() const final;

        std::vector<std::string_view> remaining_arguments() const final;

        OptionIterator* clone() const final;
    private:
        std::vector<std::string_view> m_args;
        std::vector<std::string_view>::const_iterator m_args_it;
        size_t m_pos = 0;
        char m_prefix = '-';
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <algorithm>
#include <cassert>

namespace argos
{
    namespace
    {
        using OptionTable = std::vector<std::pair<std::string_view, const OptionData*>>;

        OptionTable make_option_index(
                const std::vector<std::unique_ptr<OptionData>>& options,
                bool case_insensitive)
        {
            OptionTable index;
            for (auto& option : options)
            {
                for (auto& flag : option->flags)
                    index.emplace_back(flag, option.get());
            }

            sort(index.begin(), index.end(), [&](const auto& a, const auto& b)
            {
                return is_less(a.first, b.first, case_insensitive);
            });

            auto it = adjacent_find(index.begin(), index.end(),
                                    [&](const auto& a, const auto& b)
            {
                return are_equal(a.first, b.first, case_insensitive);
            });

            if (it == index.end())
                return index;

            if (it->first == next(it)->first)
            {
                ARGOS_THROW("Multiple definitions of flag "
                            + std::string(it->first));
            }
            else
            {
                ARGOS_THROW("Conflicting flags: " + std::string(it->first)
                            + " and " + std::string(next(it)->first));
            }
        }

        const OptionData* find_option_impl(const OptionTable& options,
                                           std::string_view arg,
                                           bool allow_abbreviations,
                                           bool case_insensitive)
        {
            auto it = std::lower_bound(
                options.begin(), options.end(),
                OptionTable::value_type(arg, nullptr),
                [&](auto& a, auto& b)
                {return is_less(a.first, b.first, case_insensitive);});
            if (it == options.end())
                return nullptr;
            if (it->first == arg)
                return it->second;
            if (case_insensitive && are_equal_ci(it->first, arg))
                return it->second;
            if (!allow_abbreviations)
                return nullptr;
            if (!starts_with(it->first, arg, case_insensitive))
                return nullptr;
            auto nxt = next(it);
            if (nxt != options.end()
                && starts_with(nxt->first, arg, case_insensitive))
                return nullptr;
            return it->second;
        }

        const OptionData* find_option(const OptionTable& options,
                                      std::string_view arg,
                                      bool allow_abbreviations,
                                      bool case_insensitive)
        {
            auto opt = find_option_impl(options, arg, allow_abbreviations,
                                        case_insensitive);
            if (opt == nullptr && arg.size() > 2 && arg.back() == '=')
            {
                arg = arg.substr(0, arg.size() - 1);
                opt = find_option_impl(options, arg, allow_abbreviations,
                                       case_insensitive);
                if (opt && opt->argument.empty())
                    opt = nullptr;
            }
            return opt;
        }

        bool is_option(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            return s[0] == (style == OptionStyle::SLASH ? '/' : '-');
        }

        std::unique_ptr<IOptionIterator>
        make_option_iterator(OptionStyle style,
                             std::vector<std::string_view> args)
        {
            switch (style)
            {
            case OptionStyle::SLASH:
                return std::make_unique<OptionIterator>(move(args), '/');
            case OptionStyle::DASH:
                return std::make_unique<OptionIterator>(move(args), '-');
            default:
                return std::make_unique<StandardOptionIterator>(move(args));
            }
        }
    }

    ArgumentIteratorImpl::ArgumentIteratorImpl(std::vector<std::string_view> args,
                                               std::shared_ptr<ParserData> data)
        : m_data(move(data)),
          m_options(make_option_index(m_data->options,
                                      m_data->parser_settings.case_insensitive)),
          m_parsed_args(std::make_shared<ParsedArgumentsImpl>(m_data)),
          m_iterator(make_option_iterator(m_data->parser_settings.option_style,
                                          move(args)))
    {
        for (const auto& option : m_data->options)
        {
            if (!option->initial_value.empty())
            {
                m_parsed_args->append_value(option->value_id,
                                            option->initial_value,
                                            option->argument_id);
            }
        }

        if (!ArgumentCounter::requires_argument_count(m_data->arguments))
            m_argument_counter = ArgumentCounter(m_data->arguments);
        else
            m_argument_counter = ArgumentCounter(m_data->arguments,
                                                 count_arguments());
    }

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(move(args), data);
        while (true)
        {
            auto code = std::get<0>(iterator.next());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.m_parsed_args;
    }

    IteratorResult ArgumentIteratorImpl::next()
    {
        if (m_state == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_state == State::DONE)
            return {IteratorResultCode::DONE, nullptr, {}};

        auto arg = m_state == State::ARGUMENTS_AND_OPTIONS
                   ? m_iterator->next()
                   : m_iterator->next_value();
        if (!arg)
        {
            if (check_argument_and_option_counts())
                return {IteratorResultCode::DONE, nullptr, {}};
            else
                return {IteratorResultCode::ERROR, nullptr, {}};
        }

        if (m_state == State::ARGUMENTS_AND_OPTIONS
            && is_option(*arg, m_data->parser_settings.option_style))
        {
            return process_option(*arg);
        }
        else
        {
            return process_argument(*arg);
        }
    }

    const std::shared_ptr<ParsedArgumentsImpl>&
    ArgumentIteratorImpl::parsed_arguments() const
    {
        return m_parsed_args;
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::process_option(const OptionData& opt,
                                         const std::string& flag)
    {
        std::string_view arg;
        switch (opt.operation)
        {
        case OptionOperation::ASSIGN:
            if (!opt.constant.empty())
            {
                m_parsed_args->assign_value(opt.value_id, opt.constant,
                                            opt.argument_id);
            }
            else if (auto value = m_iterator->next_value())
            {
                arg = m_parsed_args->assign_value(opt.value_id, *value,
                                                  opt.argument_id);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::APPEND:
            if (!opt.constant.empty())
            {
                m_parsed_args->append_value(opt.value_id, opt.constant,
                                            opt.argument_id);
            }
            else if (auto value = m_iterator->next_value())
            {
                arg = m_parsed_args->append_value(opt.value_id, *value,
                                                  opt.argument_id);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            m_parsed_args->clear_value(opt.value_id);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (opt.callback)
        {
            opt.callback(OptionView(&opt), arg,
                         ParsedArgumentsBuilder(m_parsed_args));
        }
        if (m_data->parser_settings.option_callback)
        {
            m_data->parser_settings.option_callback(
                OptionView(&opt), arg,
                ParsedArgumentsBuilder(m_parsed_args));
        }

        switch (opt.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            write_help_text(*m_data);
            [[fallthrough]];
        case OptionType::EXIT:
            m_state = State::DONE;
            m_parsed_args->set_breaking_option(&opt);
            return {OptionResult::EXIT, arg};
        case OptionType::STOP:
            m_state = State::DONE;
            m_parsed_args->set_breaking_option(&opt);
            return {OptionResult::STOP, arg};
        case OptionType::LAST_ARGUMENT:
            m_state = State::DONE;
            return {OptionResult::LAST_ARGUMENT, arg};
        case OptionType::LAST_OPTION:
            m_state = State::ARGUMENTS_ONLY;
            return {OptionResult::NORMAL, arg};
        }
        return {};
    }

    IteratorResult
    ArgumentIteratorImpl::process_option(const std::string& flag)
    {
        auto option = find_option(
            m_options, flag,
            m_data->parser_settings.allow_abbreviated_options,
            m_data->parser_settings.case_insensitive);
        if (option)
        {
            auto opt_res = process_option(*option, flag);
            switch (opt_res.first)
            {
            case OptionResult::EXIT:
                if (m_data->parser_settings.auto_exit)
                    exit(m_data->parser_settings.normal_exit_code);
                copy_remaining_arguments_to_parser_result();
                return {IteratorResultCode::OPTION, option, opt_res.second};
            case OptionResult::ERROR:
                return {IteratorResultCode::ERROR, option, {}};
            case OptionResult::LAST_ARGUMENT:
                if (!check_argument_and_option_counts())
                    return {IteratorResultCode::ERROR, nullptr, {}};
                [[fallthrough]];
            case OptionResult::STOP:
                copy_remaining_arguments_to_parser_result();
                [[fallthrough]];
            default:
                return {IteratorResultCode::OPTION, option, opt_res.second};
            }
        }
        if (!m_data->parser_settings.ignore_undefined_options
            || !starts_with(m_iterator->current(), flag))
        {
            error("Unknown option: " + std::string(m_iterator->current()));
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        else
        {
            m_parsed_args->add_unprocessed_argument(
                std::string(m_iterator->current()));
            return {IteratorResultCode::UNKNOWN, nullptr, m_iterator->current()};
        }
    }

    IteratorResult
    ArgumentIteratorImpl::process_argument(const std::string& name)
    {
        if (auto argument = m_argument_counter.next_argument())
        {
            auto s = m_parsed_args->append_value(argument->value_id, name,
                                                 argument->argument_id);
            if (argument->callback)
            {
                argument->callback(ArgumentView(argument), s,
                                   ParsedArgumentsBuilder(m_parsed_args));
            }
            if (m_data->parser_settings.argument_callback)
            {
                m_data->parser_settings.argument_callback(
                    ArgumentView(argument), s,
                    ParsedArgumentsBuilder(m_parsed_args));
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        else if (m_data->parser_settings.ignore_undefined_arguments)
        {
            m_parsed_args->add_unprocessed_argument(name);
        }
        else
        {
            error("Too many arguments, starting with \"" + name + "\".");
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        return {IteratorResultCode::UNKNOWN, nullptr, m_iterator->current()};
    }

    void ArgumentIteratorImpl::copy_remaining_arguments_to_parser_result()
    {
        for (auto str : m_iterator->remaining_arguments())
            m_parsed_args->add_unprocessed_argument(std::string(str));
    }

    size_t ArgumentIteratorImpl::count_arguments() const
    {
        size_t result = 0;
        std::unique_ptr<IOptionIterator> it(m_iterator->clone());
        bool arguments_only = false;
        for (auto arg = it->next(); arg && !arguments_only; arg = it->next())
        {
            auto option = find_option(m_options, *arg,
                                      m_data->parser_settings.allow_abbreviated_options,
                                      m_data->parser_settings.case_insensitive);
            if (option)
            {
                if (!option->argument.empty())
                    it->next_value();
                switch (option->type)
                {
                case OptionType::HELP:
                case OptionType::LAST_ARGUMENT:
                case OptionType::STOP:
                    return result;
                case OptionType::LAST_OPTION:
                    arguments_only = true;
                    break;
                default:
                    break;
                }
            }
            else if (!is_option(*arg, m_data->parser_settings.option_style))
            {
                ++result;
            }
        }

        for (auto arg = it->next(); arg; arg = it->next())
            ++result;
        return result;
    }

    bool ArgumentIteratorImpl::check_argument_and_option_counts()
    {
        for (auto& o : m_data->options)
        {
            if (!o->optional && !m_parsed_args->has(o->value_id))
            {
                auto flags = o->flags.front();
                for (unsigned i = 1; i < o->flags.size(); ++i)
                    flags += ", " + o->flags[i];
                error("Mandatory option \"" + flags + "\" is missing.");
                return false;
            }
        }
        if (m_argument_counter.is_complete())
        {
            m_state = State::DONE;
            m_parsed_args->set_result_code(ParserResultCode::SUCCESS);
            return true;
        }
        else
        {
            auto ns = ArgumentCounter::get_min_max_count(m_data->arguments);
            error((ns.first == ns.second
                   ? "Too few arguments. Expected "
                   : "Too few arguments. Expected at least ")
                  + std::to_string(ns.first) + ", received "
                  + std::to_string(m_argument_counter.count()) + ".");
            return false;
        }
    }

    void ArgumentIteratorImpl::error(const std::string& message)
    {
        if (!message.empty())
            write_error_message(*m_data, message);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        copy_remaining_arguments_to_parser_result();
        m_parsed_args->set_result_code(ParserResultCode::FAILURE);
        m_state = State::ERROR;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <iostream>

namespace argos
{
    namespace
    {
        bool check_flag_with_equal(const std::string& flag,
                                   const OptionData& od)
        {
            auto eq_pos = flag.find('=');
            if (eq_pos == std::string::npos)
                return true;
            if (eq_pos != flag.size() - 1)
                return false;
            if (od.argument.empty())
                ARGOS_THROW("Options ending with '=' must take an argument: " + flag);
            return true;
        }

        bool check_standard_flag(const std::string& flag,
                                 const OptionData& od)
        {
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() < 2)
                return false;
            if (flag[0] != '-')
                return false;
            if (flag.size() == 2)
                return true;
            if (flag[1] != '-')
                return false;
            return check_flag_with_equal(flag, od);
        }

        bool check_flag(const std::string& flag, char prefix,
                        const OptionData& od)
        {
            if (flag.size() < 2 || flag[0] != prefix)
                return false;
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() == 2)
                return true;
            return check_flag_with_equal(flag, od);
        }

        std::unique_ptr<ParserData> make_copy(const ParserData& data)
        {
            auto result = std::make_unique<ParserData>();
            result->parser_settings = data.parser_settings;
            result->help_settings = data.help_settings;
            result->arguments.reserve(data.arguments.size());
            for (const auto& a : data.arguments)
                result->arguments.push_back(std::make_unique<ArgumentData>(*a));
            result->options.reserve(data.options.size());
            for (const auto& o : data.options)
                result->options.push_back(std::make_unique<OptionData>(*o));
            return result;
        }

        void set_alue_ids(const ParserData& data)
        {
            struct InternalIdMaker
            {
                std::map<std::string_view, ValueId> explicit_ids;
                ValueId n = ValueId(0);

                std::optional<ValueId> find_value_id(std::string_view name)
                {
                    auto it = explicit_ids.find(name);
                    if (it == explicit_ids.end())
                        return {};
                    return it->second;
                }

                ValueId make_value_id(std::string_view name)
                {
                    if (auto id = find_value_id(name))
                        return *id;
                    n = ValueId(n + 1);
                    explicit_ids.emplace(name, n);
                    return n;
                }

                ValueId make_value_id(const std::vector<std::string>& names)
                {
                    for (const auto& name : names)
                    {
                        if (auto id = find_value_id(name))
                            return *id;
                    }
                    n = ValueId(n + 1);
                    for (const auto& name : names)
                        explicit_ids.emplace(name, n);
                    return n;
                }
            };

            InternalIdMaker id_maker;
            for (const auto& a : data.arguments)
            {
                if (!a->value.empty())
                {
                    a->value_id = id_maker.make_value_id(a->value);
                    id_maker.explicit_ids.emplace(a->name, a->value_id);
                }
                else
                {
                    a->value_id = id_maker.make_value_id(a->name);
                }
            }
            for (const auto& o : data.options)
            {
                if (o->operation == OptionOperation::NONE)
                    continue;
                if (!o->alias.empty())
                {
                    o->value_id = id_maker.make_value_id(o->alias);
                    for (auto& f : o->flags)
                        id_maker.explicit_ids.emplace(f, o->value_id);
                }
                else
                {
                    o->value_id = id_maker.make_value_id(o->flags);
                }
            }
        }

        inline bool has_help_option(const ParserData& data)
        {
            return std::any_of(data.options.begin(), data.options.end(),
                               [](const auto& o)
                               {return o->type == OptionType::HELP;});
        }

        inline bool has_flag(const ParserData& data, std::string_view flag)
        {
            bool ci = data.parser_settings.case_insensitive;
            return any_of(data.options.begin(), data.options.end(),
                          [&](const auto& o)
                          {return any_of(o->flags.begin(), o->flags.end(),
                                         [&](const auto& f)
                                         {return are_equal(f, flag, ci);});
                          });
        }

        void add_missing_help_option(ParserData& data)
        {
            if (!data.parser_settings.generate_help_option)
                return;
            if (has_help_option(data))
                return;
            std::vector<std::string> flags;
            switch (data.parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(data, "-h"))
                    flags.emplace_back("-h");
                if (!has_flag(data, "--help"))
                    flags.emplace_back("--help");
                break;
            case OptionStyle::SLASH:
                if (!has_flag(data, "/?"))
                    flags.emplace_back("/?");
                break;
            case OptionStyle::DASH:
                if (!has_flag(data, "-h"))
                    flags.emplace_back("-h");
                else if (!has_flag(data, "-help"))
                    flags.emplace_back("-help");
                break;
            }

            if (flags.empty())
                return;

            auto opt = Option().flags(std::move(flags)).type(OptionType::HELP)
                .help("Display the help text.")
                .constant("1").release();
            opt->argument_id = ArgumentId(data.options.size()
                                          + data.arguments.size() + 1);
            opt->section = data.current_section;
            data.options.push_back(move(opt));
        }

        void add_version_option(ParserData& data)
        {
            if (data.help_settings.version.empty())
                return;
            std::string flag;
            switch (data.parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(data, "--version"))
                    flag = "--version";
                break;
            case OptionStyle::SLASH:
                if (!has_flag(data, "/VERSION"))
                    flag = "/VERSION";
                break;
            case OptionStyle::DASH:
                if (!has_flag(data, "-version"))
                    flag = "-version";
                break;
            }

            if (flag.empty())
                return;

            auto stream = data.help_settings.output_stream
                        ? data.help_settings.output_stream
                        : &std::cout;
            auto opt = Option().flag(flag).type(OptionType::STOP)
                .help("Display the program version.")
                .constant("1")
                .callback([v = data.help_settings.version, stream]
                              (auto, auto, auto pa)
                          {
                              *stream << pa.program_name() << " " << v << "\n";
                              return true;
                          })
                .release();
            opt->argument_id = ArgumentId(data.options.size()
                                          + data.arguments.size() + 1);
            opt->section = data.current_section;
            data.options.push_back(move(opt));
        }

        ParsedArguments parse_impl(std::vector<std::string_view> args,
                                   const std::shared_ptr<ParserData>& data)
        {
            add_missing_help_option(*data);
            add_version_option(*data);
            set_alue_ids(*data);
            return ParsedArguments(
                ArgumentIteratorImpl::parse(std::move(args), data));
        }

        ArgumentIterator
        make_iterator_impl(std::vector<std::string_view> args,
                           const std::shared_ptr<ParserData>& data)
        {
            add_missing_help_option(*data);
            add_version_option(*data);
            set_alue_ids(*data);
            return ArgumentIterator(std::move(args), data);
        }
    }

    ArgumentParser::ArgumentParser()
            : ArgumentParser("UNINITIALIZED")
    {}

    ArgumentParser::ArgumentParser(std::string_view program_name,
                                   bool extract_file_name)
        : m_data(std::make_unique<ParserData>())
    {
        m_data->help_settings.program_name = extract_file_name
                                           ? get_base_name(program_name)
                                           : program_name;
    }

    ArgumentParser::ArgumentParser(ArgumentParser&& rhs) noexcept
        : m_data(std::move(rhs.m_data))
    {}

    ArgumentParser::~ArgumentParser() = default;

    ArgumentParser& ArgumentParser::operator=(ArgumentParser&& rhs) noexcept
    {
        m_data = std::move(rhs.m_data);
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Argument argument)
    {
        check_data();
        auto ad = argument.release();
        if (ad->name.empty())
            ARGOS_THROW("Argument must have a name.");
        ad->argument_id = next_argument_id();
        if (ad->section.empty())
            ad->section = m_data->current_section;
        m_data->arguments.emplace_back(std::move(ad));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option option)
    {
        check_data();

        auto od = option.release();
        if (od->flags.empty())
            ARGOS_THROW("Option must have one or more flags.");

        for (auto& flag : od->flags)
        {
            bool ok = false;
            switch (m_data->parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                ok = check_standard_flag(flag, *od);
                break;
            case OptionStyle::SLASH:
                ok = check_flag(flag, '/', *od);
                break;
            case OptionStyle::DASH:
                ok = check_flag(flag, '-', *od);
                break;
            default:
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!od->argument.empty() && !od->constant.empty())
            ARGOS_THROW("Option cannot have both argument and constant.");

        switch (od->operation)
        {
        case OptionOperation::NONE:
            if (!od->constant.empty())
                ARGOS_THROW("NONE-options cannot have a constant.");
            if (!od->alias.empty())
                ARGOS_THROW("NONE-options cannot have an alias.");
            break;
        case OptionOperation::ASSIGN:
            if (od->argument.empty() && od->constant.empty())
                od->constant = "1";
            break;
        case OptionOperation::APPEND:
            if (od->argument.empty() && od->constant.empty())
                ARGOS_THROW("Options that appends must have either constant or argument.");
            break;
        case OptionOperation::CLEAR:
            if (!od->argument.empty() ||!od->constant.empty())
                od->constant = "1";
            if (!od->optional)
                ARGOS_THROW("CLEAR-options must be optional.");
            break;
        }
        od->argument_id = next_argument_id();
        if (od->section.empty())
            od->section = m_data->current_section;
        m_data->options.push_back(std::move(od));
        return *this;
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv)
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv) const
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args)
    {
        check_data();
        return parse_impl(std::move(args), std::move(m_data));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args) const
    {
        check_data();
        return parse_impl(std::move(args), make_copy(*m_data));
    }

    ArgumentIterator ArgumentParser::make_iterator(int argc, char** argv)
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return make_iterator(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ArgumentIterator ArgumentParser::make_iterator(int argc, char** argv) const
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return make_iterator(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ArgumentIterator
    ArgumentParser::make_iterator(std::vector<std::string_view> args)
    {
        if (!m_data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return make_iterator_impl(std::move(args), std::move(m_data));
    }

    ArgumentIterator
    ArgumentParser::make_iterator(std::vector<std::string_view> args) const
    {
        check_data();
        return make_iterator_impl(std::move(args), make_copy(*m_data));
    }

    bool ArgumentParser::allow_abbreviated_options() const
    {
        check_data();
        return m_data->parser_settings.allow_abbreviated_options;
    }

    ArgumentParser& ArgumentParser::allow_abbreviated_options(bool value)
    {
        check_data();
        m_data->parser_settings.allow_abbreviated_options = value;
        return *this;
    }

    bool ArgumentParser::auto_exit() const
    {
        check_data();
        return m_data->parser_settings.auto_exit;
    }

    ArgumentParser& ArgumentParser::auto_exit(bool value)
    {
        check_data();
        m_data->parser_settings.auto_exit = value;
        return *this;
    }

    bool ArgumentParser::case_insensitive() const
    {
        check_data();
        return m_data->parser_settings.case_insensitive;
    }

    ArgumentParser& ArgumentParser::case_insensitive(bool value)
    {
        check_data();
        m_data->parser_settings.case_insensitive = value;
        return *this;
    }

    bool ArgumentParser::generate_help_option() const
    {
        check_data();
        return m_data->parser_settings.generate_help_option;
    }

    ArgumentParser& ArgumentParser::generate_help_option(bool value)
    {
        check_data();
        m_data->parser_settings.generate_help_option = value;
        return *this;
    }

    OptionStyle ArgumentParser::option_style() const
    {
        check_data();
        return m_data->parser_settings.option_style;
    }

    ArgumentParser& ArgumentParser::option_style(OptionStyle value)
    {
        check_data();
        if (value != m_data->parser_settings.option_style)
        {
            if (!m_data->options.empty())
                ARGOS_THROW("Can't change option style after"
                            " options have been added.");
            m_data->parser_settings.option_style = value;
        }
        return *this;
    }

    bool ArgumentParser::ignore_undefined_arguments() const
    {
        check_data();
        return m_data->parser_settings.ignore_undefined_arguments;
    }

    ArgumentParser& ArgumentParser::ignore_undefined_arguments(bool value)
    {
        check_data();
        m_data->parser_settings.ignore_undefined_arguments = value;
        return *this;
    }

    bool ArgumentParser::ignore_undefined_options() const
    {
        check_data();
        return m_data->parser_settings.ignore_undefined_options;
    }

    ArgumentParser& ArgumentParser::ignore_undefined_options(bool value)
    {
        check_data();
        m_data->parser_settings.ignore_undefined_options = value;
        return *this;
    }

    const ArgumentCallback& ArgumentParser::argument_callback() const
    {
        check_data();
        return m_data->parser_settings.argument_callback;
    }

    ArgumentParser& ArgumentParser::argument_callback(ArgumentCallback callback)
    {
        check_data();
        m_data->parser_settings.argument_callback = std::move(callback);
        return *this;
    }

    const OptionCallback& ArgumentParser::option_callback() const
    {
        check_data();
        return m_data->parser_settings.option_callback;
    }

    ArgumentParser& ArgumentParser::option_callback(OptionCallback callback)
    {
        check_data();
        m_data->parser_settings.option_callback = std::move(callback);
        return *this;
    }

    std::ostream* ArgumentParser::stream() const
    {
        check_data();
        return m_data->help_settings.output_stream;
    }

    ArgumentParser& ArgumentParser::stream(std::ostream* stream)
    {
        check_data();
        m_data->help_settings.output_stream = stream;
        return *this;
    }

    const std::string& ArgumentParser::program_name() const
    {
        check_data();
        return m_data->help_settings.program_name;
    }

    ArgumentParser& ArgumentParser::program_name(const std::string& name)
    {
        check_data();
        m_data->help_settings.program_name = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::about(std::string text)
    {
        return this->text(TextId::ABOUT, std::move(text));
    }

    ArgumentParser& ArgumentParser::text(TextId textId, std::string text)
    {
        check_data();
        m_data->help_settings.texts[textId] = std::move(text);
        return *this;
    }

    ArgumentParser& ArgumentParser::version(const std::string& version)
    {
        check_data();
        m_data->help_settings.version = version;
        return *this;
    }

    ArgumentParser &ArgumentParser::section(const std::string &name)
    {
        check_data();
        m_data->current_section = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::line_width(unsigned int line_width)
    {
        check_data();
        m_data->text_formatter.set_line_width(line_width);
        return *this;
    }

    void ArgumentParser::write_help_text() const
    {
        check_data();
        auto data = make_copy(*m_data);
        add_missing_help_option(*data);
        argos::write_help_text(*data);
    }

    ArgumentParser& ArgumentParser::add_word_splitting_rule(std::string str)
    {
        check_data();
        m_data->text_formatter.word_splitter().add_word(std::move(str));
        return *this;
    }

    ArgumentParser& ArgumentParser::set_exit_codes(int error, int normal_exit)
    {
        check_data();
        m_data->parser_settings.error_exit_code = error;
        m_data->parser_settings.normal_exit_code = normal_exit;
        return *this;
    }

    ArgumentParser&& ArgumentParser::move()
    {
        return std::move(*this);
    }

    void ArgumentParser::check_data() const
    {
        if (!m_data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
    }

    ArgumentId ArgumentParser::next_argument_id() const
    {
        auto& d = *m_data;
        return ArgumentId(d.options.size() + d.arguments.size() + 1);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <cerrno>
#include <cinttypes>
#include <type_traits>

namespace argos
{
    template <typename T>
    std::optional<T> parse_integer(const std::string& str, int base);

    template <>
    std::optional<int> parse_integer<int>(const std::string& str, int base);

    template <>
    std::optional<unsigned>
    parse_integer<unsigned>(const std::string& str, int base);

    template <>
    std::optional<long> parse_integer<long>(const std::string& str, int base);

    template <>
    std::optional<long long>
    parse_integer<long long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long>
    parse_integer<unsigned long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long long>
    parse_integer<unsigned long long>(const std::string& str, int base);

    template <typename T>
    std::optional<T> parse_floating_point(const std::string& str);

    template <>
    std::optional<float> parse_floating_point<float>(const std::string& str);

    template <>
    std::optional<double> parse_floating_point<double>(const std::string& str);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        template <typename T>
        T get_integer(const ArgumentValue& value, T default_value, int base)
        {
            auto s = value.value();
            if (!s)
                return default_value;
            auto n = parse_integer<T>(std::string(*s), base);
            if (!n)
                value.error();
            return *n;
        }

        template <typename T>
        T get_floating_point(const ArgumentValue& value, T default_value)
        {
            auto s = value.value();
            if (!s)
                return default_value;
            auto n = parse_floating_point<T>(std::string(*s));
            if (!n)
                value.error();
            return *n;
        }
    }

    ArgumentValue::ArgumentValue()
        : m_value_id(),
          m_argument_id()
    {}

    ArgumentValue::ArgumentValue(std::optional<std::string_view> value,
                                 std::shared_ptr<ParsedArgumentsImpl> args,
                                 ValueId value_id,
                                 ArgumentId argument_id)
        : m_value(value),
          m_args(std::move(args)),
          m_value_id(value_id),
          m_argument_id(argument_id)
    {}

    ArgumentValue::ArgumentValue(const ArgumentValue&) = default;

    ArgumentValue::ArgumentValue(ArgumentValue&& ) noexcept = default;

    ArgumentValue::~ArgumentValue() = default;

    ArgumentValue&
    ArgumentValue::operator=(const ArgumentValue&) = default;

    ArgumentValue&
    ArgumentValue::operator=(ArgumentValue&&) noexcept = default;

    ArgumentValue::operator bool() const
    {
        return m_value.has_value();
    }

    std::unique_ptr<IArgumentView>
    ArgumentValue::argument() const
    {
        if (!m_args)
            ARGOS_THROW("ArgumentValue has not been initialized.");
        return m_args->get_argument_view(m_argument_id);
    }

    std::optional<std::string_view> ArgumentValue::value() const
    {
        return m_value;
    }

    bool ArgumentValue::as_bool(bool default_value) const
    {
        if (!m_value)
            return default_value;
        return !m_value->empty() && m_value != "0" && m_value != "false";
    }

    int ArgumentValue::as_int(int default_value, int base) const
    {
        return get_integer<int>(*this, default_value, base);
    }

    unsigned ArgumentValue::as_uint(unsigned default_value, int base) const
    {
        return get_integer<unsigned>(*this, default_value, base);
    }

    long ArgumentValue::as_long(long default_value, int base) const
    {
        return get_integer<long>(*this, default_value, base);
    }

    long long ArgumentValue::as_llong(long long default_value, int base) const
    {
        return get_integer<long long>(*this, default_value, base);
    }

    unsigned long
    ArgumentValue::as_ulong(unsigned long default_value, int base) const
    {
        return get_integer<unsigned long>(*this, default_value, base);
    }

    unsigned long long
    ArgumentValue::as_ullong(unsigned long long default_value, int base) const
    {
        return get_integer<unsigned long long>(*this, default_value, base);
    }

    float ArgumentValue::as_float(float default_value) const
    {
        return get_floating_point<float>(*this, default_value);
    }

    double ArgumentValue::as_double(double default_value) const
    {
        return get_floating_point<double>(*this, default_value);
    }

    std::string ArgumentValue::as_string(const std::string& default_value) const
    {
        return m_value ? std::string(*m_value) : default_value;
    }

    ArgumentValues
    ArgumentValue::split(char separator,
                         size_t min_parts, size_t max_parts) const
    {
        if (!m_args)
            ARGOS_THROW("ArgumentValue has not been initialized.");
        if (!m_value)
            return ArgumentValues({}, m_args, m_value_id);
        auto parts = split_string(*m_value, separator, max_parts - 1);
        if (parts.size() < min_parts)
        {
            error("Invalid value: \"" + std::string(*m_value)
                  + "\". Must have at least " + std::to_string(min_parts)
                  + " values separated by \"" + separator + "\".");
            return ArgumentValues({}, m_args, m_value_id);
        }
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        values.reserve(parts.size());
        for (auto& part : parts)
            values.emplace_back(part, m_argument_id);
        return {move(values), m_args, m_value_id};
    }

    void ArgumentValue::error(const std::string& message) const
    {
        if (!m_args)
            ARGOS_THROW("ArgumentValue has not been initialized.");
        m_args->error(message, m_argument_id);
    }

    void ArgumentValue::error() const
    {
        if (!m_value)
            ARGOS_THROW("ArgumentValue has no value.");
        error("Invalid value: " + std::string(*m_value) + ".");
    }
}

//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ArgumentValueIterator::ArgumentValueIterator() = default;

    ArgumentValueIterator::ArgumentValueIterator(
        const ArgumentValueIterator::It& internal_iterator,
        std::shared_ptr<ParsedArgumentsImpl> args,
        ValueId value_id)
    : m_iterator(internal_iterator),
      m_args(std::move(args)),
      m_value_id(value_id)
    {}

    ArgumentValueIterator& ArgumentValueIterator::operator++()
    {
        ++m_iterator;
        return *this;
    }

    ArgumentValueIterator ArgumentValueIterator::operator++(int)
    {
        auto it = *this;
        ++m_iterator;
        return it;
    }

    ArgumentValue ArgumentValueIterator::operator*() const
    {
        return ArgumentValue(m_iterator->first, m_args,
                             m_value_id, m_iterator->second);
    }

    ArgumentValueIterator::It ArgumentValueIterator::internal_iterator() const
    {
        return m_iterator;
    }

    bool operator==(const ArgumentValueIterator& a, const ArgumentValueIterator& b)
    {
        return a.internal_iterator() == b.internal_iterator();
    }

    bool operator!=(const ArgumentValueIterator& a, const ArgumentValueIterator& b)
    {
        return a.internal_iterator() != b.internal_iterator();
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        ArgumentId get_argument_id(
            const std::vector<std::pair<std::string_view, ArgumentId>>& values)
        {
            if (values.empty())
                return {};
            for (auto it = next(values.begin()); it != values.end(); ++it)
            {
                if (it->second != prev(it)->second)
                    return {};
            }
            return values.front().second;
        }

        void error(const ArgumentValues& values, std::string_view value)
        {
            values.error("Invalid value: " + std::string(value) + ".");
        }

        template <typename T>
        std::vector<T> parse_floats(const ArgumentValues& values,
                                    const std::vector<T>& default_value)
        {
            if (values.empty())
                return default_value;

            std::vector<T> result;
            result.reserve(values.size());
            for (auto& v : values.raw_values())
            {
                auto value = parse_floating_point<T>(std::string(v));
                if (!value)
                    error(values, v);
                else
                    result.push_back(*value);
            }
            return result;
        }

        template <typename T>
        std::vector<T> parse_integers(const ArgumentValues& values,
                                      const std::vector<T>& default_value,
                                      int base)
        {
            if (values.empty())
                return default_value;

            std::vector<T> result;
            result.reserve(values.size());
            for (auto& v : values.raw_values())
            {
                auto value = parse_integer<T>(std::string(v), base);
                if (!value)
                    error(values, v);
                else
                    result.push_back(*value);
            }
            return result;
        }
    }

    ArgumentValues::ArgumentValues(
            std::vector<std::pair<std::string_view, ArgumentId>> values,
            std::shared_ptr<ParsedArgumentsImpl> args,
            ValueId value_id)
        : m_values(move(values)),
          m_args(move(args)),
          m_value_id(value_id)
    {}

    ArgumentValues::ArgumentValues(const ArgumentValues&) = default;

    ArgumentValues::ArgumentValues(ArgumentValues&&) noexcept = default;

    ArgumentValues::~ArgumentValues() = default;

    ArgumentValues&
    ArgumentValues::operator=(const ArgumentValues&) = default;

    ArgumentValues&
    ArgumentValues::operator=(ArgumentValues&&) noexcept = default;

    ArgumentValues::operator bool() const
    {
        return !m_values.empty();
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ArgumentValues::arguments() const
    {
        return m_args->get_argument_views(m_value_id);
    }

    void ArgumentValues::error(const std::string& message) const
    {
        m_args->error(message, get_argument_id(m_values));
    }

    bool ArgumentValues::empty() const
    {
        return m_values.empty();
    }

    size_t ArgumentValues::size() const
    {
        return m_values.size();
    }

    std::vector<ArgumentValue> ArgumentValues::values() const
    {
        std::vector<ArgumentValue> result;
        for (const auto& v : m_values)
            result.emplace_back(v.first, m_args, m_value_id, v.second);
        return result;
    }

    std::vector<std::string_view> ArgumentValues::raw_values() const
    {
        std::vector<std::string_view> result;
        for (const auto& s : m_values)
            result.push_back(s.first);
        return result;
    }

    ArgumentValue ArgumentValues::value(size_t index) const
    {
        if (m_values.empty())
            return {{}, m_args, m_value_id, {}};

        const auto& v = m_values.at(index);
        return {v.first, m_args, m_value_id, v.second};
    }

    std::vector<int>
    ArgumentValues::as_ints(const std::vector<int>& default_value,
                            int base) const
    {
        return parse_integers<int>(*this, default_value, base);
    }

    std::vector<unsigned>
    ArgumentValues::as_uints(const std::vector<unsigned>& default_value,
                             int base) const
    {
        return parse_integers<unsigned>(*this, default_value, base);
    }

    std::vector<long>
    ArgumentValues::as_longs(const std::vector<long>& default_value,
                             int base) const
    {
        return parse_integers<long>(*this, default_value, base);
    }

    std::vector<long long>
    ArgumentValues::as_llongs(const std::vector<long long>& default_value,
                              int base) const
    {
        return parse_integers<long long>(*this, default_value, base);
    }

    std::vector<unsigned long>
    ArgumentValues::as_ulongs(const std::vector<unsigned long>& default_value,
                              int base) const
    {
        return parse_integers<unsigned long>(*this, default_value, base);
    }

    std::vector<unsigned long long> ArgumentValues::as_ullongs(
        const std::vector<unsigned long long>& default_value,
        int base) const
    {
        return parse_integers<unsigned long long>(*this, default_value, base);
    }

    std::vector<float>
    ArgumentValues::as_floats(const std::vector<float>& default_value) const
    {
        return parse_floats(*this, default_value);
    }

    std::vector<double>
    ArgumentValues::as_doubles(const std::vector<double>& default_value) const
    {
        return parse_floats(*this, default_value);
    }

    std::vector<std::string> ArgumentValues::as_strings(
            const std::vector<std::string>& default_value) const
    {
        if (m_values.empty())
            return default_value;

        std::vector<std::string> result;
        result.reserve(m_values.size());
        for (const auto& v : m_values)
            result.emplace_back(v.first);
        return result;
    }

    ArgumentValues
    ArgumentValues::split(char separator,
                          size_t min_parts, size_t max_parts) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        for (auto value : m_values)
        {
            auto parts = split_string(value.first, separator, max_parts - 1);
            if (parts.size() < min_parts)
            {
                error("Invalid value: \"" + std::string(value.first)
                      + "\". Must be at least " + std::to_string(min_parts)
                      + " values separated by \"" + separator + "\".");
            }
            for (auto& part : parts)
                values.emplace_back(part, value.second);
        }
        return {move(values), m_args, m_value_id};
    }

    ArgumentValueIterator ArgumentValues::begin() const
    {
        return ArgumentValueIterator(m_values.begin(), m_args, m_value_id);
    }

    ArgumentValueIterator ArgumentValues::end() const
    {
        return ArgumentValueIterator(m_values.end(), m_args, m_value_id);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ArgumentView::ArgumentView(const ArgumentData* data)
            : m_argument(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    const std::string& ArgumentView::help() const
    {
        return m_argument->help;
    }

    const std::string& ArgumentView::section() const
    {
        return m_argument->section;
    }

    const std::string& ArgumentView::value() const
    {
        return m_argument->value;
    }

    Visibility ArgumentView::visibility() const
    {
        return m_argument->visibility;
    }

    int ArgumentView::id() const
    {
        return m_argument->id;
    }

    const std::string& ArgumentView::name() const
    {
        return m_argument->name;
    }

    bool ArgumentView::optional() const
    {
        return m_argument->min_count == 0;
    }

    std::pair<unsigned, unsigned> ArgumentView::count() const
    {
        return {m_argument->min_count, m_argument->max_count};
    }

    ValueId ArgumentView::value_id() const
    {
        return m_argument->value_id;
    }

    ArgumentId ArgumentView::argument_id() const
    {
        return m_argument->argument_id;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    unsigned get_console_width();

    unsigned get_console_width(unsigned min_width, unsigned default_width = 80);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)
    #include <sys/ioctl.h>
    #include <unistd.h>
#elif defined(WIN32)
    #define NOMINMAX
    #include <Windows.h>
#endif

namespace argos
{
#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)

    unsigned get_console_width()
    {
        winsize ws = {};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) <= -1)
            return 0;
        return unsigned(ws.ws_col);
    }

#elif defined(WIN32)

    unsigned get_console_width()
    {
        HANDLE h_con = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h_con == INVALID_HANDLE_VALUE)
            return 0;

        CONSOLE_SCREEN_BUFFER_INFO con_info;
        if (!GetConsoleScreenBufferInfo(h_con, &con_info))
            return 0;

        return unsigned(con_info.srWindow.Right - con_info.srWindow.Left);
    }

#else

    unsigned get_console_width()
    {
        return 0;
    }

#endif

    unsigned get_console_width(unsigned min_width, unsigned int default_width)
    {
        auto width = get_console_width();
        if (width == 0)
            return default_width;
        return width < min_width ? min_width : width;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        std::string get_argument_name(const ArgumentData& arg)
        {
            if (arg.name[0] == '<' || arg.name[0] == '[')
                return arg.name;

            std::string result;
            for (unsigned i = 0; i < arg.min_count; ++i)
            {
                if (!result.empty())
                    result += " ";
                result += "<" + arg.name + ">";
            }

            if (arg.max_count == arg.min_count)
                return result;

            if (!result.empty())
                result += " ";
            if (arg.max_count - arg.min_count == 1)
                result += "[<" + arg.name + ">]";
            else
                result += "[<" + arg.name + ">]...";
            return result;
        }

        bool is_stop_option(OptionType type)
        {
            return type == OptionType::HELP
                   || type == OptionType::STOP
                   || type == OptionType::EXIT;
        }

        std::string get_brief_option_name(const OptionData& opt,
                                          bool prefer_long_flag)
        {
            std::string opt_txt;
            bool braces = opt.optional
                          && !is_stop_option(opt.type);
            if (braces)
                opt_txt.push_back('[');

            std::string_view flag;
            if (prefer_long_flag)
            {
                auto it = std::find_if(opt.flags.begin(),  opt.flags.end(),
                                       [](auto& s){return s.size() > 2;});
                if (it != opt.flags.end())
                    flag = *it;
            }

            if (flag.empty())
                flag = opt.flags.front();

            opt_txt += flag;
            if (!opt.argument.empty())
            {
                if (flag.back() != '=')
                    opt_txt += " ";
                if (opt.argument.front() != '<')
                {
                    opt_txt += "<";
                    opt_txt += opt.argument;
                    opt_txt.push_back('>');
                }
                else
                {
                    opt_txt += opt.argument;
                }
            }
            if (braces)
                opt_txt.push_back(']');
            return opt_txt;
        }

        std::string get_long_option_name(const OptionData& opt)
        {
            std::string opt_txt;
            for (const auto& flag : opt.flags)
            {
                if (!opt_txt.empty())
                    opt_txt.append(", ");
                opt_txt += flag;
                if (!opt.argument.empty())
                {
                    if (flag.back() != '=')
                        opt_txt.push_back(' ');
                    if (opt.argument.front() != '<')
                    {
                        opt_txt += "<";
                        opt_txt += opt.argument;
                        opt_txt.push_back('>');
                    }
                    else
                    {
                        opt_txt += opt.argument;
                    }
                }
            }
            return opt_txt;
        }

        std::optional<std::string>
        get_custom_text(ParserData& data, TextId text_id)
        {
            auto it = data.help_settings.texts.find(text_id);
            if (it != data.help_settings.texts.end())
                return it->second;
            return {};
        }

        bool is_empty(const std::optional<std::string>& str)
        {
            return !str || str->empty();
        }

        std::optional<std::string>
        write_custom_text(ParserData& data, TextId text_id,
                          bool prepend_newline = false)
        {
            auto text = get_custom_text(data, text_id);
            if (!is_empty(text))
            {
                if (prepend_newline)
                    data.text_formatter.newline();
                data.text_formatter.write_words(*text);
                if (!data.text_formatter.is_current_line_empty())
                    data.text_formatter.newline();
            }
            return text;
        }

        void write_stop_and_help_usage(ParserData& data)
        {
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN
                    || !is_stop_option(opt->type))
                {
                    continue;
                }

                data.text_formatter.write_words(data.help_settings.program_name);
                data.text_formatter.write_words(" ");
                data.text_formatter.push_indentation(TextFormatter::CURRENT_COLUMN);
                data.text_formatter.write_lines(get_brief_option_name(*opt, true));
                data.text_formatter.write_words(" ");
                data.text_formatter.pop_indentation();
                data.text_formatter.newline();
            }
        }

        using HelpText = std::pair<std::string, std::string_view>;
        using HelpTextVector = std::vector<HelpText>;
        using SectionHelpTexts = std::pair<std::string_view, HelpTextVector>;

        unsigned int get_help_text_label_width(
            const ParserData& data,
            const std::vector<SectionHelpTexts>& sections)
        {
            // Determine what width should be reserved for the argument names
            // and option flags.
            std::vector<unsigned> name_widths;
            std::vector<unsigned> text_widths;
            for (const auto& entry : sections)
            {
                for (const auto& [name, txt] : entry.second)
                {
                    name_widths.push_back(static_cast<unsigned>(name.size()));
                    text_widths.push_back(static_cast<unsigned>(txt.size()));
                }
            }

            std::sort(name_widths.begin(), name_widths.end());
            std::sort(text_widths.begin(), text_widths.end());
            auto line_width = data.text_formatter.line_width();
            // Check if both the longest name and the longest help text
            // can fit on the same line.
            auto name_width = name_widths.back() + 3;
            if (name_width > 32 || name_width + text_widths.back() > line_width)
                return 0;
            return name_width;
        }

        void write_argument_sections(ParserData& data, bool prepend_newline)
        {
            std::vector<SectionHelpTexts> sections;

            auto add_help_text = [&](std::string_view s, std::string a, std::string_view b)
            {
                auto it = find_if(sections.begin(), sections.end(),
                                  [&](const auto& v)
                                  {return v.first == s;});
                if (it == sections.end())
                {
                    sections.push_back({s, {}});
                    it = std::prev(sections.end());
                }
                it->second.emplace_back(std::move(a), b);
            };

            auto arg_title = get_custom_text(data, TextId::ARGUMENTS_TITLE);
            if (!arg_title)
                arg_title = "ARGUMENTS";
            for (auto& a : data.arguments)
            {
                if ((a->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = a->section.empty() ? *arg_title : a->section;
                add_help_text(section, get_argument_name(*a), a->help);
            }
            auto opt_title = get_custom_text(data, TextId::OPTIONS_TITLE);
            if (!opt_title)
                opt_title = "OPTIONS";
            for (auto& o : data.options)
            {
                if ((o->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = o->section.empty() ? *opt_title : o->section;
                add_help_text(section, get_long_option_name(*o), o->help);
            }

            if (sections.empty())
                return;
            unsigned int name_width = get_help_text_label_width(data, sections);

            auto& formatter = data.text_formatter;
            for (auto&[section, txts] : sections)
            {
                if (prepend_newline)
                    formatter.newline();
                formatter.write_words(section);
                formatter.newline();
                formatter.push_indentation(2);
                for (auto& [name, text] : txts)
                {
                    formatter.write_words(name);
                    if (!text.empty())
                    {
                        if (name_width)
                        {
                            if (formatter.current_line_width() >= name_width)
                                formatter.write_words("  ");
                            formatter.push_indentation(name_width);
                        }
                        else
                        {
                            formatter.newline();
                            formatter.push_indentation(8);
                        }
                        formatter.write_words(text);
                        formatter.pop_indentation();
                    }
                    formatter.newline();
                }
                formatter.pop_indentation();
                prepend_newline = true;
            }
        }

        void write_brief_usage(ParserData& data, bool prepend_newline)
        {
            auto& formatter = data.text_formatter;
            if (prepend_newline)
                formatter.newline();

            formatter.push_indentation(2);
            write_stop_and_help_usage(data);
            formatter.write_words(data.help_settings.program_name);
            formatter.write_words(" ");
            formatter.push_indentation(TextFormatter::CURRENT_COLUMN);
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN
                    || is_stop_option(opt->type))
                {
                    continue;
                }

                formatter.write_lines(get_brief_option_name(*opt, false));
                formatter.write_words(" ");
            }
            for (auto& arg : data.arguments)
            {
                if ((arg->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                formatter.write_lines(get_argument_name(*arg));
                formatter.write_words(" ");
            }
            formatter.pop_indentation();
            formatter.newline();
            formatter.pop_indentation();
        }

        bool write_usage(ParserData& data, bool prepend_newline = false)
        {
            if (auto t = get_custom_text(data, TextId::USAGE); t && t->empty())
                return false;

            auto text1 = write_custom_text(data, TextId::USAGE_TITLE,
                                           prepend_newline);
            if (!text1)
            {
                if (prepend_newline)
                    data.text_formatter.newline();
                data.text_formatter.write_words("USAGE");
                data.text_formatter.newline();
                prepend_newline = false;
            }
            else
            {
                prepend_newline = prepend_newline && is_empty(text1);
            }
            auto text2 = write_custom_text(data, TextId::USAGE,
                                           prepend_newline);
            if (text2)
                return !is_empty(text1) || !is_empty(text2);
            write_brief_usage(data, prepend_newline);
            return true;
        }

        std::string get_name(ParserData& data, ArgumentId argument_id)
        {
            for (const auto& a : data.arguments)
            {
                if (a->argument_id == argument_id)
                    return a->name;
            }
            for (const auto& o : data.options)
            {
                if (o->argument_id == argument_id)
                {
                    std::string name = o->flags.front();
                    for (size_t i = 1; i < o->flags.size(); ++i)
                        name += ", " + o->flags[i];
                    return name;
                }
            }
            return {};
        }
    }

    void write_help_text(ParserData& data)
    {
        if (data.help_settings.output_stream)
            data.text_formatter.set_stream(data.help_settings.output_stream);
        bool newline = !is_empty(write_custom_text(data, TextId::INITIAL_TEXT));
        newline = write_usage(data, newline) || newline;
        newline = !is_empty(write_custom_text(data, TextId::ABOUT, newline)) || newline;
        write_argument_sections(data, newline);
        write_custom_text(data, TextId::FINAL_TEXT, true);
    }

    void write_error_message(ParserData& data, const std::string& msg)
    {
        if (data.help_settings.output_stream)
            data.text_formatter.set_stream(data.help_settings.output_stream);
        else
            data.text_formatter.set_stream(&std::cerr);
        data.text_formatter.write_words(data.help_settings.program_name + ": ");
        data.text_formatter.write_words(msg);
        data.text_formatter.newline();
        if (!write_custom_text(data, TextId::ERROR_USAGE))
            write_usage(data);
    }

    void write_error_message(ParserData& data, const std::string& msg,
                             ArgumentId argument_id)
    {
        if (auto name = get_name(data, argument_id); !name.empty())
            write_error_message(data, name + ": " + msg);
        else
            write_error_message(data, msg);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    Option::Option()
        : m_option(std::make_unique<OptionData>())
    {}

    Option::Option(std::initializer_list<std::string> flags)
        : m_option(std::make_unique<OptionData>())
    {
        m_option->flags = flags;
    }

    Option::Option(const Option& rhs)
        : m_option(rhs.m_option
                   ? std::make_unique<OptionData>(*rhs.m_option)
                   : std::unique_ptr<OptionData>())
    {}

    Option::Option(Option&& rhs) noexcept
        : m_option(std::move(rhs.m_option))
    {}

    Option::~Option() = default;

    Option& Option::operator=(const Option& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_option)
                m_option = std::make_unique<OptionData>(*rhs.m_option);
            else
                m_option = {};
        }
        return *this;
    }

    Option& Option::operator=(Option&& rhs) noexcept
    {
        m_option = std::move(rhs.m_option);
        return *this;
    }

    Option& Option::help(const std::string& text)
    {
        check_option();
        m_option->help = text;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        check_option();
        m_option->section = name;
        return *this;
    }

    Option& Option::alias(const std::string& id)
    {
        check_option();
        m_option->alias = id;
        return *this;
    }

    Option& Option::operation(OptionOperation operation)
    {
        check_option();
        m_option->operation = operation;
        return *this;
    }

    Option& Option::visibility(Visibility visibility)
    {
        check_option();
        m_option->visibility = visibility;
        return *this;
    }

    Option& Option::id(int id)
    {
        check_option();
        m_option->id = id;
        return *this;
    }

    Option& Option::flag(const std::string& f)
    {
        check_option();
        m_option->flags = {f};
        return *this;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        check_option();
        m_option->flags = std::move(f);
        return *this;
    }

    Option& Option::argument(const std::string& name)
    {
        check_option();
        m_option->argument = name;
        return *this;
    }

    Option& Option::initial_value(const std::string& value)
    {
        check_option();
        m_option->initial_value = value;
        return *this;
    }

    Option& Option::constant(const char* value)
    {
        return this->constant(std::string(value));
    }

    Option& Option::constant(const std::string& value)
    {
        check_option();
        m_option->constant = value;
        return *this;
    }

    Option& Option::constant(bool value)
    {
        return this->constant(value ? 1LL : 0LL);
    }

    Option& Option::constant(int value)
    {
        return this->constant(static_cast<long long>(value));
    }

    Option& Option::constant(long long value)
    {
        check_option();
        m_option->constant = std::to_string(value);
        return *this;
    }

    Option& Option::callback(OptionCallback callback)
    {
        check_option();
        m_option->callback = move(callback);
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        check_option();
        m_option->type = type;
        return *this;
    }

    Option& Option::optional(bool optional)
    {
        check_option();
        m_option->optional = optional;
        return *this;
    }

    const OptionData& Option::data() const
    {
        check_option();
        return *m_option;
    }

    std::unique_ptr<OptionData> Option::release()
    {
        check_option();
        return std::move(m_option);
    }

    void Option::check_option() const
    {
        if (!m_option)
            ARGOS_THROW("Cannot use Option instance after"
                        " release() has been called.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    OptionIterator::OptionIterator()
        : m_args_it(m_args.begin())
    {}

    OptionIterator::OptionIterator(std::vector<std::string_view> args, char prefix)
        : m_args(move(args)),
          m_args_it(m_args.begin()),
          m_prefix(prefix)
    {}

    OptionIterator::OptionIterator(const OptionIterator& rhs)
        : m_args(rhs.m_args),
          m_args_it(m_args.begin() + std::distance(rhs.m_args.begin(), rhs.m_args_it)),
          m_pos(rhs.m_pos),
          m_prefix(rhs.m_prefix)
    {}

    std::optional<std::string> OptionIterator::next()
    {
        if (m_pos != 0)
        {
            m_pos = 0;
            ++m_args_it;
        }

        if (m_args_it == m_args.end())
            return {};

        if (m_args_it->size() <= 2 || (*m_args_it)[0] != m_prefix)
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        auto eq = m_args_it->find('=');
        if (eq == std::string_view::npos)
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        m_pos = eq + 1;
        return std::string(m_args_it->substr(0, m_pos));
    }

    std::optional<std::string> OptionIterator::next_value()
    {
        if (m_args_it == m_args.end())
            return {};

        if (m_pos != std::string_view::npos)
        {
            auto result = m_args_it->substr(m_pos);
            m_pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_args_it == m_args.end())
        {
            m_pos = 0;
            return {};
        }

        m_pos = m_args_it->size();
        return std::string(*m_args_it);
    }

    std::string_view OptionIterator::current() const
    {
        if (m_args_it == m_args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_args_it;
    }

    std::vector<std::string_view> OptionIterator::remaining_arguments() const
    {
        auto it = m_pos == 0 ? m_args_it : std::next(m_args_it);
        return std::vector<std::string_view>(it, m_args.end());
    }

    OptionIterator* OptionIterator::clone() const
    {
        return new OptionIterator(*this);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    OptionView::OptionView(const OptionData* data)
        : m_option(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    const std::string& OptionView::help() const
    {
        return m_option->help;
    }

    const std::string& OptionView::section() const
    {
        return m_option->section;
    }

    const std::string& OptionView::value() const
    {
        return m_option->alias;
    }

    OptionOperation OptionView::operation() const
    {
        return m_option->operation;
    }

    Visibility OptionView::visibility() const
    {
        return m_option->visibility;
    }

    int OptionView::id() const
    {
        return m_option->id;
    }

    ValueId OptionView::value_id() const
    {
        return m_option->value_id;
    }

    const std::vector<std::string>& OptionView::flags() const
    {
        return m_option->flags;
    }

    const std::string& OptionView::argument() const
    {
        return m_option->argument;
    }

    const std::string& OptionView::initial_value() const
    {
        return m_option->initial_value;
    }

    const std::string& OptionView::constant() const
    {
        return m_option->constant;
    }

    OptionType OptionView::type() const
    {
        return m_option->type;
    }

    bool OptionView::optional() const
    {
        return m_option->optional;
    }

    ArgumentId OptionView::argument_id() const
    {
        return m_option->argument_id;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <cstdlib>

namespace argos
{
    namespace
    {
        template <typename T>
        T str_to_int(const char* str, char** endp, int base);

        template <>
        long str_to_int<long>(const char* str, char** endp, int base)
        {
            return strtol(str, endp, base);
        }

        template <>
        long long str_to_int<long long>(const char* str, char** endp, int base)
        {
            return strtoll(str, endp, base);
        }

        template <>
        unsigned long
        str_to_int<unsigned long>(const char* str, char** endp, int base)
        {
            return strtoul(str, endp, base);
        }

        template <>
        unsigned long long
        str_to_int<unsigned long long>(const char* str, char** endp, int base)
        {
            return strtoull(str, endp, base);
        }

        template <typename T>
        std::optional<T> parse_integer_impl(const std::string& str, int base)
        {
            if (str.empty())
                return {};
            char* endp = nullptr;
            errno = 0;
            auto value = str_to_int<T>(str.c_str(), &endp, base);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<int> parse_integer<int>(const std::string& str, int base)
    {
        auto n = parse_integer_impl<long>(str, base);
        if (!n)
            return {};

        if constexpr (sizeof(int) != sizeof(long))
        {
            if (*n < INT_MIN || INT_MAX < *n)
                return {};
        }
        return static_cast<int>(*n);
    }

    template <>
    std::optional<unsigned>
    parse_integer<unsigned>(const std::string& str, int base)
    {
        auto n = parse_integer_impl<unsigned long>(str, base);
        if (!n)
            return {};

        if constexpr (sizeof(unsigned) != sizeof(unsigned long))
        {
            if (UINT_MAX < *n)
                return {};
        }
        return static_cast<unsigned>(*n);
    }

    template <>
    std::optional<long> parse_integer<long>(const std::string& str, int base)
    {
        return parse_integer_impl<long>(str, base);
    }

    template <>
    std::optional<long long>
    parse_integer<long long>(const std::string& str, int base)
    {
        return parse_integer_impl<long long>(str, base);
    }

    template <>
    std::optional<unsigned long>
    parse_integer<unsigned long>(const std::string& str, int base)
    {
        return parse_integer_impl<unsigned long>(str, base);
    }

    template <>
    std::optional<unsigned long long>
    parse_integer<unsigned long long>(const std::string& str, int base)
    {
        return parse_integer_impl<unsigned long long>(str, base);
    }

    namespace
    {
        template <typename T>
        T str_to_float(const char* str, char** endp);

        template <>
        float str_to_float<float>(const char* str, char** endp)
        {
            return strtof(str, endp);
        }

        template <>
        double str_to_float<double>(const char* str, char** endp)
        {
            return strtod(str, endp);
        }

        template <typename T>
        std::optional<T> parse_floating_point_impl(const std::string& str)
        {
            if (str.empty())
                return {};
            char* endp = nullptr;
            errno = 0;
            auto value = str_to_float<T>(str.c_str(), &endp);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<float> parse_floating_point<float>(const std::string& str)
    {
        return parse_floating_point_impl<float>(str);
    }

    template <>
    std::optional<double> parse_floating_point<double>(const std::string& str)
    {
        return parse_floating_point_impl<double>(str);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_impl(move(impl))
    {}

    ParsedArguments::ParsedArguments(ParsedArguments&& rhs) noexcept
        : m_impl(move(rhs.m_impl))
    {}

    ParsedArguments::~ParsedArguments() = default;

    ParsedArguments& ParsedArguments::operator=(ParsedArguments&& rhs) noexcept
    {
        m_impl = move(rhs.m_impl);
        return *this;
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_impl->has(m_impl->get_value_id(name));
    }

    bool ParsedArguments::has(const IArgumentView& arg) const
    {
        return m_impl->has(arg.value_id());
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto value = m_impl->get_value(id);
        if (value)
            return {value->first, m_impl, id, value->second};
        else
            return {{}, m_impl, id, {}};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        auto value = m_impl->get_value(arg.value_id());
        if (value)
            return {value->first, m_impl, arg.value_id(), arg.argument_id()};
        else
            return {{}, m_impl, arg.value_id(), arg.argument_id()};
    }

    ArgumentValues ParsedArguments::values(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto values = m_impl->get_values(id);
        return {values, m_impl, id};
    }

    ArgumentValues ParsedArguments::values(const IArgumentView& arg) const
    {
        auto values = m_impl->get_values(arg.value_id());
        return {values, m_impl, arg.value_id()};
    }

    std::vector<std::unique_ptr<ArgumentView>>
    ParsedArguments::all_arguments() const
    {
        std::vector<std::unique_ptr<ArgumentView>> result;
        for (auto& a : m_impl->parser_data()->arguments)
            result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        return result;
    }

    std::vector<std::unique_ptr<OptionView>>
    ParsedArguments::all_options() const
    {
        std::vector<std::unique_ptr<OptionView>> result;
        for (auto& o : m_impl->parser_data()->options)
            result.emplace_back(std::make_unique<OptionView>(o.get()));
        return result;
    }

    ParserResultCode ParsedArguments::result_code() const
    {
        return m_impl->result_code();
    }

    OptionView ParsedArguments::stop_option() const
    {
        const auto* option = m_impl->stop_option();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }

    const std::vector<std::string>&
    ParsedArguments::unprocessed_arguments() const
    {
        return m_impl->unprocessed_arguments();
    }

    void ParsedArguments::filter_parsed_arguments(int& argc, char**& argv)
    {
        if (argc <= 1)
            return;
        const auto& unprocessed = m_impl->unprocessed_arguments();
        auto it = unprocessed.begin();
        int out = 1;
        if (!unprocessed.empty())
        {
            for (int in = 1; in < argc; ++in)
            {
                if (argv[in] == *it)
                {
                    argv[out++] = argv[in];
                    if (++it == unprocessed.end())
                        break;
                }
            }
        }
        argc = out;
    }

    void ParsedArguments::error(const std::string& msg)
    {
        m_impl->error(msg);
    }

    namespace
    {
        std::string get_name(const IArgumentView& arg)
        {
            if (const auto* a = dynamic_cast<const ArgumentView*>(&arg))
                return a->name();

            if (const auto* o = dynamic_cast<const OptionView*>(&arg))
            {
                std::string s;
                for (const auto& f : o->flags())
                {
                    if (!s.empty())
                        s += ", ";
                    s += f;
                }
                return s;
            }
            return {};
        }

        void print_argument(std::ostream& stream,
                            const std::string& label,
                            const ArgumentValues& values)
        {
            stream << label << ":";
            for (const auto value : values)
                stream << " \"" << value.as_string() << "\"";
            stream << "\n";
        }
    }

    void print(const ParsedArguments& args)
    {
        print(args, std::cout);
    }

    void print(const ParsedArguments& args, std::ostream& stream)
    {
        std::vector<const IArgumentView*> argViews;
        auto a = args.all_arguments();
        std::transform(a.begin(), a.end(), back_inserter(argViews),
                       [](auto& av) {return av.get();});
        auto o = args.all_options();
        std::transform(o.begin(), o.end(), back_inserter(argViews),
                       [](auto& ov) {return ov.get();});

        stable_sort(argViews.begin(), argViews.end(),
                    [](auto& a, auto& b) {return a->value_id() < b->value_id();});

        std::vector<std::pair<const IArgumentView*, std::string>> labels;
        for (const auto* arg : argViews)
        {
            if (!labels.empty() && labels.back().first->value_id() == arg->value_id())
                labels.back().second += ", " + get_name(*arg);
            else
                labels.emplace_back(arg, get_name(*arg));
        }

        for (const auto&[arg, label] : labels)
            print_argument(stream, label, args.values(*arg));

        if (!args.unprocessed_arguments().empty())
        {
            stream << "Unprocessed arguments:";
            for (auto& arg : args.unprocessed_arguments())
                stream << " \"" << arg << "\"";
        }
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(
            std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_impl(move(impl))
    {}

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const std::string& name,
                                   const std::string& value)
    {
        m_impl->append_value(m_impl->get_value_id(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_impl->append_value(arg.value_id(), value, arg.argument_id());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const std::string& name,
                                   const std::string& value)
    {
        m_impl->assign_value(m_impl->get_value_id(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_impl->assign_value(arg.value_id(), value, arg.argument_id());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const std::string& name)
    {
        m_impl->clear_value(m_impl->get_value_id(name));
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const IArgumentView& arg)
    {
        m_impl->clear_value(arg.value_id());
        return *this;
    }

    ArgumentValue ParsedArgumentsBuilder::value(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto value = m_impl->get_value(id);
        if (value)
            return {value->first, m_impl, id, value->second};
        else
            return {{}, m_impl, id, {}};
    }

    ArgumentValue
    ParsedArgumentsBuilder::value(const IArgumentView& arg) const
    {
        auto value = m_impl->get_value(arg.value_id());
        if (value)
            return {value->first, m_impl, arg.value_id(), arg.argument_id()};
        else
            return {{}, m_impl, arg.value_id(), arg.argument_id()};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto values = m_impl->get_values(id);
        return {move(values), m_impl, id};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const IArgumentView& arg) const
    {
        auto values = m_impl->get_values(arg.value_id());
        return {move(values), m_impl, arg.value_id()};
    }

    bool ParsedArgumentsBuilder::has(const std::string& name) const
    {
        return m_impl->has(m_impl->get_value_id(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg) const
    {
        return m_impl->has(arg.value_id());
    }

    void ParsedArgumentsBuilder::error(const std::string& msg)
    {
         m_impl->error(msg);
    }

    void ParsedArgumentsBuilder::error(const std::string& msg,
                                       const IArgumentView& arg)
    {
        m_impl->error(msg, arg.argument_id());
    }

    std::ostream& ParsedArgumentsBuilder::stream() const
    {
        auto custom_stream = m_impl->parser_data()->help_settings.output_stream;
        return custom_stream ? *custom_stream : std::cout;
    }

    const std::string& ParsedArgumentsBuilder::program_name() const
    {
      return m_impl->parser_data()->help_settings.program_name;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        template <typename It, typename Value, typename IsLess>
        It lower_bound(It begin, It end, Value&& v, IsLess is_less)
        {
            while (begin != end)
            {
                auto offset = std::distance(begin, end) / 2;
                auto mid = begin + offset;
                if (is_less(*mid, v))
                    begin = mid + 1;
                else
                    end = mid;
            }
            return begin;
        }
    }

    ParsedArgumentsImpl::ParsedArgumentsImpl(std::shared_ptr<ParserData> data)
        : m_data(move(data))
    {
        assert(m_data);
        for (auto& a : m_data->arguments)
        {
            m_ids.emplace_back(a->name, a->value_id, a->argument_id);
            if (!a->value.empty())
                m_ids.emplace_back(a->value, a->value_id, a->argument_id);
        }
        for (auto& o : m_data->options)
        {
            if (o->operation == OptionOperation::NONE)
                continue;

            for (auto& f : o->flags)
                m_ids.emplace_back(f, o->value_id, o->argument_id);
            if (!o->alias.empty())
                m_ids.emplace_back(o->alias, o->value_id, o->argument_id);
        }
        if (!m_ids.empty())
        {
            using std::get;
            sort(m_ids.begin(), m_ids.end());
            for (auto it = next(m_ids.begin()); it != m_ids.end(); ++it)
            {
                auto p = prev(it);
                if (get<0>(*it) == get<0>(*p) && get<2>(*it) != get<2>(*p))
                    get<2>(*it) = get<2>(*p) = {};
            }
            m_ids.erase(unique(m_ids.begin(), m_ids.end()), m_ids.end());
        }
    }

    bool ParsedArgumentsImpl::has(ValueId value_id) const
    {
        return m_values.find(value_id) != m_values.end();
    }

    const std::vector<std::string>&
    ParsedArgumentsImpl::unprocessed_arguments() const
    {
        return m_unprocessed_arguments;
    }

    void ParsedArgumentsImpl::add_unprocessed_argument(const std::string& arg)
    {
        m_unprocessed_arguments.push_back(arg);
    }

    std::string_view
    ParsedArgumentsImpl::assign_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id)
    {
        auto it = m_values.lower_bound(value_id);
        if (it == m_values.end() || it->first != value_id)
            return append_value(value_id, value, argument_id);

        it->second = {value, argument_id};
        auto nxt = next(it);
        while (nxt != m_values.end() && nxt->first == value_id)
            m_values.erase(nxt++);
        return it->second.first;
    }

    std::string_view
    ParsedArgumentsImpl::append_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id)
    {
        return m_values.insert({value_id, {value, argument_id}})->second.first;
    }

    void ParsedArgumentsImpl::clear_value(ValueId value_id)
    {
        m_values.erase(value_id);
    }

    ValueId
    ParsedArgumentsImpl::get_value_id(std::string_view value_name) const
    {
        using std::get;
        auto it = lower_bound(m_ids.begin(), m_ids.end(), value_name,
                              [](auto& p, auto& s) {return get<0>(p) < s;});
        if (it == m_ids.end() || get<0>(*it) != value_name)
            ARGOS_THROW("Unknown value: " + std::string(value_name));
        return get<1>(*it);
    }

    std::optional<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::get_value(ValueId value_id) const
    {
        auto it = m_values.lower_bound(value_id);
        if (it == m_values.end() || it->first != value_id)
            return {};
        auto nx = next(it);
        if (nx != m_values.end() && nx->first == value_id)
            ARGOS_THROW("Attempt to read multiple values as a single value.");
        return it->second;
    }

    std::vector<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::get_values(ValueId value_id) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> result;
        for (auto it = m_values.lower_bound(value_id);
             it != m_values.end() && it->first == value_id; ++it)
        {
            result.emplace_back(it->second);
        }
        return result;
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ParsedArgumentsImpl::get_argument_views(ValueId value_id) const
    {
        std::vector<std::unique_ptr<IArgumentView>> result;
        for (auto& a : m_data->arguments)
        {
            if (a->value_id == value_id)
                result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        }
        for (auto& o : m_data->options)
        {
            if (o->value_id == value_id)
                result.emplace_back(std::make_unique<OptionView>(o.get()));
        }
        return result;
    }

    std::unique_ptr<IArgumentView>
    ParsedArgumentsImpl::get_argument_view(ArgumentId argument_id) const
    {
        for (auto& a : m_data->arguments)
        {
            if (a->argument_id == argument_id)
                return std::make_unique<ArgumentView>(a.get());
        }
        for (auto& o : m_data->options)
        {
            if (o->argument_id == argument_id)
                return std::make_unique<OptionView>(o.get());
        }
        return {};
    }

    const std::shared_ptr<ParserData>& ParsedArgumentsImpl::parser_data() const
    {
        return m_data;
    }

    ParserResultCode ParsedArgumentsImpl::result_code() const
    {
        return m_result_code;
    }

    void ParsedArgumentsImpl::set_result_code(ParserResultCode result_code)
    {
        m_result_code = result_code;
    }

    const OptionData* ParsedArgumentsImpl::stop_option() const
    {
        return m_stop_option;
    }

    void ParsedArgumentsImpl::set_breaking_option(const OptionData* option)
    {
        m_result_code = ParserResultCode::STOP;
        m_stop_option = option;
    }

    void ParsedArgumentsImpl::error(const std::string& message)
    {
        write_error_message(*m_data, message);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }

    void ParsedArgumentsImpl::error(const std::string& message,
                                    ArgumentId argument_id)
    {
        write_error_message(*m_data, message, argument_id);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    StandardOptionIterator::StandardOptionIterator()
        : m_args_it(m_args.end()),
          m_pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_args(move(args)),
          m_args_it(m_args.begin()),
          m_pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(const StandardOptionIterator& rhs)
        : m_args(rhs.m_args),
          m_args_it(m_args.begin() + std::distance(rhs.m_args.begin(), rhs.m_args_it)),
          m_pos(rhs.m_pos)
    {}

    std::optional<std::string> StandardOptionIterator::next()
    {
        if (m_pos == std::string_view::npos)
        {
            m_pos = 0;
            ++m_args_it;
        }
        else if (m_pos != 0)
        {
            if (m_pos < m_args_it->size() && (*m_args_it)[1] != '-')
            {
                auto c = (*m_args_it)[m_pos++];
                if (m_pos == m_args_it->size())
                    m_pos = std::string_view::npos;
                return std::string{'-', c};
            }
            ++m_args_it;
            m_pos = 0;
        }

        if (m_args_it == m_args.end())
            return {};

        if (m_args_it->size() <= 2 || (*m_args_it)[0] != '-')
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        if ((*m_args_it)[1] != '-')
        {
            m_pos = 2;
            return std::string(m_args_it->substr(0, 2));
        }

        auto eq = m_args_it->find('=');
        if (eq == std::string_view::npos)
        {
            m_pos = std::string_view::npos;
            return std::string(*m_args_it);
        }

        m_pos = eq + 1;
        return std::string(m_args_it->substr(0, m_pos));
    }

    std::optional<std::string> StandardOptionIterator::next_value()
    {
        if (m_args_it == m_args.end())
            return {};

        if (m_pos != std::string_view::npos)
        {
            auto result = m_args_it->substr(m_pos);
            m_pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_args_it == m_args.end())
        {
            m_pos = 0;
            return {};
        }

        return std::string(*m_args_it);
    }

    std::string_view StandardOptionIterator::current() const
    {
        if (m_args_it == m_args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_args_it;
    }

    std::vector<std::string_view> StandardOptionIterator::remaining_arguments() const
    {
        auto it = m_pos == 0 ? m_args_it : std::next(m_args_it);
        return std::vector<std::string_view>(it, m_args.end());
    }

    IOptionIterator* StandardOptionIterator::clone() const
    {
        return new StandardOptionIterator(*this);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    bool are_equal_chars_ci(char a, char b)
    {
        if (a == b)
            return true;
        auto ua = uint8_t(a), ub = uint8_t(b);
        if ((ua ^ ub) != 32)
            return false;
        return 'A' <= (ua & 0xDFu) && (ua & 0xDFu) <= 'Z';
    }

    bool are_equal_ci(std::string_view str1, std::string_view str2)
    {
        if (str1.size() != str2.size())
            return false;
        return std::equal(str2.begin(), str2.end(), str1.begin(), are_equal_chars_ci);
    }

    bool are_equal(std::string_view str1, std::string_view str2,
                   bool case_insensitive)
    {
        if (case_insensitive)
            return are_equal_ci(str1, str2);
        return str1 == str2;
    }

    bool starts_with(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool starts_with_ci(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin(),
                          are_equal_chars_ci);
    }

    bool starts_with(std::string_view str, std::string_view prefix,
                     bool case_insensitive)
    {
        if (case_insensitive)
            return starts_with_ci(str, prefix);
        return starts_with(str, prefix);
    }

    int compare_ci(int c1, int c2)
    {
        if (c1 == c2)
            return 0;
        auto ic1 = int(uint8_t(c1) & 0xDFu);
        if (ic1 < 'A' || 'Z' < ic1)
            return c1 - c2;
        auto ic2 = int(uint8_t(c2) & 0xDFu);
        return ic1 - ic2;
    }

    bool is_less_ci(std::string_view str1, std::string_view str2)
    {
        auto size = std::min(str1.size(), str2.size());
        for (size_t i = 0; i < size; ++i)
        {
            if (auto cmp = compare_ci(str1[i], str2[i]); cmp != 0)
                return cmp < 0;
        }
        return str1.size() < str2.size();
    }

    bool is_less(std::string_view str1, std::string_view str2,
                 bool case_insensitive)
    {
        if (case_insensitive)
            return is_less_ci(str1, str2);
        return str1 < str2;
    }

    std::vector<std::string_view>
    split_string(std::string_view s, char delimiter, size_t max_split)
    {
        if (max_split == 0)
            return {s};

        std::vector<std::string_view> result;
        size_t pos = 0;
        while (true)
        {
            auto next_pos = s.find(delimiter, pos);
            result.push_back(s.substr(pos, next_pos - pos));
            if (next_pos == std::string_view::npos)
                break;
            pos = next_pos + 1;
            if (result.size() == max_split)
            {
                result.push_back(s.substr(pos));
                break;
            }
        }
        return result;
    }

    std::string_view get_base_name(std::string_view str)
    {
        auto pos = str.find_last_of("/\\");
        return pos == std::string_view::npos ? str : str.substr(pos + 1);
    }

    constexpr size_t get_code_point_length(char c) noexcept
    {
        auto u = unsigned(static_cast<uint8_t>(c));
        if (u < 0x80)
            return 1;
        if (u > 0xF7)
            return 0;
        switch (unsigned(u >> 4u) & 7u)
        {
        case 7: return 4;
        case 6: return 3;
        case 5:
        case 4: return 2;
        default: break;
        }
        return 0;
    }

    size_t count_code_points(std::string_view str)
    {
        size_t count = 0;
        size_t char_len = 0;
        for (auto c : str)
        {
            if (char_len == 0)
            {
                char_len = get_code_point_length(c);
                if (char_len == 0)
                    return str.size();
                ++count;
                --char_len;
            }
            else if ((unsigned(static_cast<uint8_t>(c)) & 0xC0u) == 0x80u)
            {
                --char_len;
            }
            else
            {
                return str.size();
            }
        }
        return count;
    }

    size_t find_nth_code_point(std::string_view str, size_t n)
    {
        if (n >= str.size())
            return std::string_view::npos;
        size_t count = 0;
        size_t char_len = 0;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (char_len == 0)
            {
                if (count == n)
                    return i;
                char_len = get_code_point_length(str[i]);
                if (char_len == 0)
                    return n;
                ++count;
                --char_len;
            }
            else if ((static_cast<uint8_t>(str[i]) & 0xC0u) == 0x80u)
            {
                --char_len;
            }
            else
            {
                return n;
            }
        }
        return char_len ? n : std::string_view::npos;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        std::pair<std::string_view, std::string_view>
        next_line(std::string_view text)
        {
            auto pos = text.find_first_of("\n\r");
            if (pos == std::string_view::npos)
                return {text, {}};
            if (text[pos] == '\n'
                || (pos + 1 != text.size() && text[pos + 1] != '\n'))
            {
                return {text.substr(0, pos), text.substr(pos + 1)};
            }
            return {text.substr(0, pos), text.substr(pos + 2)};
        }

        std::tuple<char, std::string_view, std::string_view>
        next_token(std::string_view text)
        {
            if (text.empty())
                return {'\0', text, {}};

            switch (text[0])
            {
            case '\t':
                return {'\t', text.substr(0, 1), text.substr(1)};
            case '\r':
                if (text.size() > 1 && text[1] == '\n')
                    return {'\n', text.substr(0, 2), text.substr(2)};
                [[fallthrough]];
            case '\n':
                return {'\n', text.substr(0, 1), text.substr(1)};
            case ' ':
                if (auto n = text.find_first_not_of(' '); n != std::string_view::npos)
                    return {' ', text.substr(0, n), text.substr(n)};
                else
                    return {' ', text, {}};
            default:
                if (auto n = text.find_first_of("\t\r\n "); n != std::string_view::npos)
                    return {'A', text.substr(0, n), text.substr(n)};
                else
                    return {'A', text, {}};
            }
        }

        bool is_list_marker(const std::string_view& str)
        {
            if (str.size() == 1)
                return  str[0] == '-' || str[0] == '*';

            if (str.size() > 1 && str.back() == '.')
            {
                return std::all_of(str.begin(), str.end() - 1,
                                   [](auto c) {return std::isdigit(c);});
            }

            return false;
        }
    }

    TextFormatter::TextFormatter()
        : TextFormatter(&std::cout, get_console_width(32))
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, get_console_width(32))
    {}

    TextFormatter::TextFormatter(std::ostream* stream, unsigned line_width)
        : m_writer(line_width)
    {
        if (line_width <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_writer.set_stream(stream);
        m_indents.push_back(0);
    }

    WordSplitter& TextFormatter::word_splitter()
    {
        return m_word_splitter;
    }

    std::ostream* TextFormatter::stream() const
    {
        return m_writer.stream();
    }

    void TextFormatter::set_stream(std::ostream* stream)
    {
        m_writer.set_stream(stream);
    }

    void TextFormatter::push_indentation(unsigned indent)
    {
        if (indent == CURRENT_COLUMN)
        {
            indent = m_writer.current_width();
            m_writer.set_spaces(0);
        }
        m_indents.push_back(indent);
        m_writer.set_indentation(indent);
    }

    void TextFormatter::pop_indentation()
    {
        if (m_indents.size() == 1)
            ARGOS_THROW("No more indentations to pop.");
        m_indents.pop_back();
        m_writer.set_indentation(m_indents.back());
    }

    void TextFormatter::write_words(std::string_view text)
    {
        while (!text.empty())
        {
            auto [type, token, remainder] = next_token(text);
            switch (type)
            {
            case '\t':
                m_writer.tab();
                begin_alignment();
                break;
            case '\n':
                end_alignment();
                m_writer.newline();
                m_writer.set_spaces(0);
                break;
            case ' ':
                m_writer.set_spaces(static_cast<unsigned>(token.size()));
                begin_alignment();
                break;
            case 'A':
                update_alignment(token);
                append_word(token);
                break;
            default:
                break;
            }
            text = remainder;
        }
        end_alignment();
    }

    void TextFormatter::write_lines(std::string_view text)
    {
        auto remainder = text;
        while (!remainder.empty())
        {
            auto [line, rem] = next_line(remainder);
            if (!line.empty())
                append_word(line);
            if (!rem.empty())
                newline();
            remainder = rem;
        }
        if (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            newline();
    }

    void TextFormatter::newline()
    {
        m_writer.newline();
        m_writer.set_spaces(0);
    }

    void TextFormatter::flush()
    {
        m_writer.flush();
    }

    unsigned TextFormatter::line_width() const
    {
        return m_writer.line_width();
    }

    void TextFormatter::set_line_width(unsigned line_width)
    {
        if (line_width <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_writer.set_line_width(line_width);
    }

    unsigned int TextFormatter::current_line_width() const
    {
        return m_writer.current_width();
    }

    bool TextFormatter::is_current_line_empty() const
    {
        return m_writer.is_current_line_empty();
    }

    void TextFormatter::append_word(std::string_view word)
    {
        auto remainder = word;
        while (!m_writer.write(remainder))
        {
            auto width = m_writer.remaining_width();
            auto[w, s, r] = m_word_splitter.split(
                word,
                word.size() - remainder.size(),
                width,
                m_writer.is_current_line_empty());
            if (!w.empty())
            {
                m_writer.write(w);
                if (s)
                    m_writer.write(std::string_view(&s, 1));
                newline();
                remainder = r;
            }
            else if (m_writer.is_current_line_empty())
            {
                if (m_writer.spaces() != 0)
                {
                    m_writer.set_spaces(0);
                }
                else
                {
                    m_writer.write(remainder, true);
                    return;
                }
            }
            else
            {
                newline();
                m_writer.set_spaces(0);
            }
        }
    }

    void TextFormatter::begin_alignment()
    {
        if (m_writer.is_current_line_empty()
            || m_state == State::UNALIGNED_MARKER)
        {
            push_indentation(CURRENT_COLUMN);
            m_state = State::ALIGNMENT;
        }
        else if (m_state == State::ALIGNED_MARKER)
        {
            pop_indentation();
            push_indentation(CURRENT_COLUMN);
            m_state = State::ALIGNMENT;
        }
    }

    void TextFormatter::update_alignment(const std::string_view& token)
    {
        if (m_writer.is_current_line_empty() && is_list_marker(token))
        {
            m_state = m_state == State::ALIGNMENT
                      ? State::ALIGNED_MARKER
                      : State::UNALIGNED_MARKER;
        }
    }

    void TextFormatter::end_alignment()
    {
        if (m_state == State::ALIGNMENT || m_state == State::ALIGNED_MARKER)
        {
            pop_indentation();
            m_state = State::NO_ALIGNMENT;
        }
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    TextWriter::TextWriter(unsigned line_width)
        : m_stream(&std::cout),
          m_line_width(line_width)
    {}

    std::ostream* TextWriter::stream() const
    {
        return m_stream;
    }

    void TextWriter::set_stream(std::ostream* stream)
    {
        m_stream = stream;
    }

    unsigned TextWriter::indentation() const
    {
        return m_indent;
    }

    bool TextWriter::set_indentation(unsigned indent)
    {
        if (m_indent >= m_line_width)
            return false;
        m_indent = indent;
        return true;
    }

    bool TextWriter::write(std::string_view str, bool force)
    {
        auto width = current_width();
        auto remaining = std::max(width, m_line_width) - width;
        auto str_width = static_cast<unsigned>(count_code_points(str));
        if (!force && str_width > remaining)
            return false;
        m_line.append(width - m_current_line_width, ' ');
        m_spaces = 0;
        m_line.append(str);
        m_current_line_width += width - m_current_line_width + str_width;
        return true;
    }

    void TextWriter::newline()
    {
        m_line.push_back('\n');
        m_current_line_width = 0;
        flush();
    }

    void TextWriter::flush()
    {
        m_stream->write(m_line.data(), m_line.size());
        m_line.clear();
    }

    void TextWriter::tab()
    {
        m_spaces += m_tab_size - (current_width() % m_tab_size);
    }

    unsigned TextWriter::spaces() const
    {
        return m_spaces;
    }

    void TextWriter::set_spaces(unsigned n)
    {
        m_spaces = n;
    }

    unsigned TextWriter::current_width() const
    {
        return std::max(m_current_line_width, m_indent) + m_spaces;
    }

    unsigned TextWriter::remaining_width() const
    {
        return m_line_width - std::min(current_width(), m_line_width);
    }

    bool TextWriter::is_current_line_empty() const
    {
        return m_line.empty();
    }

    unsigned TextWriter::line_width() const
    {
        return m_line_width;
    }

    void TextWriter::set_line_width(unsigned width)
    {
        m_line_width = width;
    }

    std::string_view TextWriter::currentLine() const
    {
        return m_line;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        bool is_utf8_continuation(char c)
        {
            return (uint8_t(c) & 0xC0u) == 0x80;
        }
    }

    void WordSplitter::add_word(std::string word_rule)
    {
        size_t offset = 0;
        std::vector<Split> splits;
        for (auto pos = word_rule.find_first_of(' '); pos != std::string::npos;
             pos = word_rule.find_first_of(' ', pos + 1))
        {
            if (pos == 0 || word_rule[pos - 1] == ' ')
                ARGOS_THROW("Invalid split rule: '" + word_rule + "'");
            auto sep = word_rule[pos - 1] == '-' ? '\0' : '-';
            splits.push_back({unsigned(pos - offset), sep});
            ++offset;
        }
        splits.push_back({unsigned(word_rule.size() - offset), '\0'});
        word_rule.erase(remove(word_rule.begin(), word_rule.end(), ' '),
                        word_rule.end());
        m_strings.push_back(move(word_rule));
        m_splits.insert({std::string_view(m_strings.back()), move(splits)});
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t start_index,
                        size_t max_length, bool must_split) const
    {
        auto it = m_splits.find(word);
        if (it != m_splits.end())
        {
            Split prev = {unsigned(start_index), '\0'};
            size_t length = 0;
            for (auto split : it->second)
            {
                if (split.index < start_index + 1)
                    continue;
                length += count_code_points(word.substr(prev.index, split.index - prev.index));
                if (length + (split.separator ? 1 : 0) > max_length)
                    break;
                prev = split;
            }
            if (prev.index > start_index + 1)
                return {word.substr(start_index, prev.index - start_index),
                        prev.separator,
                        word.substr(prev.index)};
        }
        if (must_split)
            return default_rule(word.substr(start_index), max_length);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::default_rule(std::string_view word, size_t max_length) const
    {
        if (max_length <= 2)
            return {{}, '\0', word};
        auto max_pos = find_nth_code_point(word, max_length);
        if (max_pos == std::string_view::npos)
            return {word, '\0', {}};
        auto ignore_utf8 = max_pos == max_length;
        --max_pos;
        if (!ignore_utf8)
        {
            while (is_utf8_continuation(word[max_pos]))
                --max_pos;
        }

        auto min_pos = (max_length + 2) / 3;
        auto index = max_pos;
        for (auto count = max_length - 1; count-- > min_pos;)
        {
            --index;
            if (!ignore_utf8)
            {
                while (is_utf8_continuation(word[index]))
                    --index;
            }
            if (uint8_t(word[index - 1]) >= 127 || uint8_t(word[index]) >= 127)
                continue;
            if ((isalnum(word[index - 1]) == 0) != (isalnum(word[index]) == 0))
                return {word.substr(0, index), '\0', word.substr(index)};
            if ((isdigit(word[index - 1]) == 0) != (isdigit(word[index]) == 0))
                return {word.substr(0, index), '-', word.substr(index)};
        }
        return {word.substr(0, max_pos),
                '-',
                word.substr(max_pos)};
    }
}
