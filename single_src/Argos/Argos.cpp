#include "Argos.hpp"
//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

#define _ARGOS_THROW_3(file, line, msg) \
    throw ::argos::ArgosException(file ":" #line ": " msg)

#define _ARGOS_THROW_2(file, line, msg) \
    _ARGOS_THROW_3(file, line, msg)

#define ARGOS_THROW(msg) \
    _ARGOS_THROW_2(__FILE__, __LINE__, msg)

//****************************************************************************
// Copyright © 2023 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2023-10-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <variant>

namespace argos
{
    using TextSource = std::variant<std::string, TextCallback>;

    inline std::string get_text(const TextSource& source)
    {
        if (auto str = std::get_if<std::string>(&source))
            return *str;
        return std::get<TextCallback>(source)();
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <string>

namespace argos
{
    struct ArgumentData
    {
        std::string name;
        TextSource help;
        std::string section;
        std::string alias;
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
// This file is distributed under the Zero-Clause BSD License.
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

    Argument& Argument::help(TextCallback callback)
    {
        check_argument();
        m_argument->help = callback;
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
        m_argument->alias = id;
        return *this;
    }

    Argument& Argument::callback(ArgumentCallback callback)
    {
        check_argument();
        m_argument->callback = std::move(callback);
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

    Argument& Argument::mandatory(bool mandatory)
    {
        return optional(!mandatory);
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
            ARGOS_THROW("Argument has been moved.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <vector>

namespace argos
{
    struct OptionData
    {
        std::vector<std::string> flags;
        TextSource help;
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

    void validate_and_update(OptionData& option, OptionStyle style);
}

//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <map>
#include <memory>

namespace argos
{
    struct CommandData
    {
        CommandData();

        CommandData(const CommandData&);

        CommandData(CommandData&&) noexcept;

        ~CommandData();

        CommandData& operator=(const CommandData&);

        CommandData& operator=(CommandData&&) noexcept;

        void add(std::unique_ptr<ArgumentData> arg);

        void add(std::unique_ptr<OptionData> opt);

        void add(std::unique_ptr<CommandData> cmd);

        void copy_from(const CommandData& cmd);

        void build_option_index(bool case_insensitive);

        [[nodiscard]] const OptionData*
        find_option(std::string_view flag,
                    bool allow_abbreviations,
                    bool case_insensitive) const;

        [[nodiscard]] const CommandData*
        find_command(std::string_view cmd_name,
                     bool case_insensitive) const;

        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;
        std::vector<std::unique_ptr<CommandData>> commands;
        std::string current_section;

        std::string name;
        std::string full_name;
        std::map<TextId, TextSource> texts;
        Visibility visibility = Visibility::NORMAL;
        std::optional<bool> require_subcommand;
        bool multi_command = false;
        /**
         * The heading the command is listed under in the parent
         * command's help.
         */
        std::string section;
        int id = 0;
        ArgumentId argument_id = {};

    private:
        [[nodiscard]] const OptionData*
        find_option_impl(std::string_view flag,
                         bool allow_abbreviations,
                         bool case_insensitive) const;

        std::vector<std::pair<std::string_view, const OptionData*>> option_index;
    };

    struct ParserData;

    /**
     * Finish the initialization of this command and any subcommands, and
     * make them ready for parsing arguments.
     */
    void finish_initialization(CommandData& cmd,
                               const ParserData& data,
                               ValueId start_id = {},
                               ArgumentId argument_id = {});

    struct ParserSettings;

    bool has_flag(const CommandData& cmd,
                  std::string_view flag,
                  const ParserSettings& settings);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class ArgumentCounter
    {
    public:
        ArgumentCounter();

        explicit ArgumentCounter(const CommandData& command);

        ArgumentCounter(const CommandData& command,
                        size_t argument_count,
                        size_t initial_count = 0);

        const ArgumentData* next_argument();

        [[nodiscard]] size_t count() const;

        [[nodiscard]] bool is_complete() const;

        static std::pair<size_t, size_t>
        get_min_max_count(const CommandData& command);

        static bool requires_argument_count(const CommandData& command);

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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        size_t find_first_optional(const CommandData& command)
        {
            auto& arguments = command.arguments;
            size_t result = 0;
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                if (arguments[i]->min_count > 0)
                    result = i + 1;
            }
            return result;
        }

        void make_argument_counters(
            const CommandData& command,
            std::vector<std::pair<size_t, const ArgumentData*>>& counters,
            size_t& first_optional)
        {
            first_optional = find_first_optional(command);
            for (size_t i = 0; i < command.arguments.size(); ++i)
            {
                auto& a = command.arguments[i];
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
        make_argument_counters(const CommandData& command,
                               size_t n)
        {
            const auto [lo, hi] = ArgumentCounter::get_min_max_count(command);
            if (n < lo)
                n = 0;
            else if (n > hi)
                n = hi - lo;
            else
                n -= lo;

            std::vector<std::pair<size_t, const ArgumentData*>> result;
            for (auto& arg : command.arguments)
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
    {
    }

    ArgumentCounter::ArgumentCounter(const CommandData& command)
    {
        make_argument_counters(command, m_counters, m_first_optional);
    }

    ArgumentCounter::ArgumentCounter(const CommandData& command,
                                     size_t argument_count,
                                     size_t initial_count)
        : m_counters(make_argument_counters(command, argument_count)),
          m_first_optional(m_counters.size())
    {
        for (size_t i = 0; i < initial_count; ++i)
        {
            if (next_argument() == nullptr)
                break;
        }
    }

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
    ArgumentCounter::get_min_max_count(const CommandData& command)
    {
        size_t lo = 0, hi = 0;
        for (auto& arg : command.arguments)
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

    bool ArgumentCounter::requires_argument_count(const CommandData& command)
    {
        bool deterministic = true;
        for (auto& arg : command.arguments)
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
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the Zero-Clause BSD License.
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

        [[nodiscard]] std::ostream* stream() const;

        void set_stream(std::ostream* stream);

        [[nodiscard]] unsigned indentation() const;

        bool set_indentation(unsigned indent);

        bool write(std::string_view str, bool force = false);

        void newline();

        void flush();

        void tab();

        [[nodiscard]] unsigned spaces() const;

        void set_spaces(unsigned n);

        [[nodiscard]] unsigned current_width() const;

        [[nodiscard]] unsigned remaining_width() const;

        [[nodiscard]] bool is_current_line_empty() const;

        [[nodiscard]] unsigned line_width() const;

        void set_line_width(unsigned width);

        [[nodiscard]] std::string_view currentLine() const;
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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <list>

namespace argos
{
    class WordSplitter
    {
    public:
        void add_word(std::string word_rule);

        void add_words(std::vector<std::string> word_rules);

        [[nodiscard]] std::tuple<std::string_view, char, std::string_view>
        split(std::string_view word, size_t start_index, size_t max_length,
              bool must_split) const;
    private:
        static std::tuple<std::string_view, char, std::string_view>
        default_rule(std::string_view word, size_t max_length) ;

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
// This file is distributed under the Zero-Clause BSD License.
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

        [[nodiscard]] std::ostream* stream() const;

        void set_stream(std::ostream* stream);

        [[nodiscard]] unsigned line_width() const;

        void set_line_width(unsigned line_width);

        [[nodiscard]] unsigned current_line_width() const;

        [[nodiscard]] bool is_current_line_empty() const;

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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

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
        std::ostream* output_stream = nullptr;
        unsigned line_width = 0;
        std::vector<std::string> word_split_rules;
    };

    struct ParserData
    {
        CommandData command;
        ParserSettings parser_settings;
        HelpSettings help_settings;
        std::string version;
    };

    void finish_initialization(ParserData& data);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <optional>
#include <span>

namespace argos
{
    class OptionIterator
    {
    public:
        OptionIterator();

        explicit OptionIterator(std::vector<std::string_view> args,
                                char prefix);

        OptionIterator(const OptionIterator& rhs);

        std::optional<std::string> next();

        std::optional<std::string> next_value();

        [[nodiscard]] std::string_view current() const;

        [[nodiscard]] std::span<std::string> remaining_arguments() const;

        void insert(const std::vector<std::string>& args);
    private:
        std::vector<std::string> m_all_args;
        std::span<std::string> m_args;
        size_t m_pos = 0;
        char m_prefix = '-';
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class StandardOptionIterator
    {
    public:
        StandardOptionIterator();

        explicit StandardOptionIterator(std::vector<std::string_view> args);

        StandardOptionIterator(const StandardOptionIterator& rhs);

        std::optional<std::string> next();

        std::optional<std::string> next_value();

        [[nodiscard]] std::string_view current() const;

        [[nodiscard]] std::span<std::string> remaining_arguments();

        void insert(std::vector<std::string> args);
    private:
        void split_concatenated_flags();

        std::vector<std::string> m_all_args;
        std::span<std::string> m_args;
        size_t m_pos = 0;
    };
}

//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-19.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    struct OptionIteratorWrapper
    {
        std::optional<std::string> next()
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).next();
            else
                return std::get<StandardOptionIterator>(iterator).next();
        }

        std::optional<std::string> next_value()
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).next_value();
            else
                return std::get<StandardOptionIterator>(iterator).next_value();
        }

        [[nodiscard]] std::string_view current() const
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).current();
            else
                return std::get<StandardOptionIterator>(iterator).current();
        }

        [[nodiscard]] std::span<std::string> remaining_arguments()
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                return std::get<OptionIterator>(iterator).remaining_arguments();
            else
                return std::get<StandardOptionIterator>(iterator).remaining_arguments();
        }

        void insert(const std::vector<std::string>& args)
        {
            if (std::holds_alternative<OptionIterator>(iterator))
                std::get<OptionIterator>(iterator).insert(args);
            else
                std::get<StandardOptionIterator>(iterator).insert(args);
        }

        std::variant<OptionIterator, StandardOptionIterator> iterator;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        explicit ParsedArgumentsImpl(const CommandData* command,
                                     std::shared_ptr<ParserData> data);

        [[nodiscard]] bool has(ValueId value_id) const;

        [[nodiscard]] const std::vector<std::string>& unprocessed_arguments() const;

        void add_unprocessed_argument(const std::string& arg);

        std::string_view assign_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id);

        std::string_view append_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id);

        void clear_value(ValueId value_id);

        [[nodiscard]] ValueId get_value_id(std::string_view value_name) const;

        [[nodiscard]] std::optional<std::pair<std::string_view, ArgumentId>>
        get_value(ValueId value_id) const;

        [[nodiscard]] std::vector<std::pair<std::string_view, ArgumentId>>
        get_values(ValueId value_id) const;

        const std::shared_ptr<ParsedArgumentsImpl>&
        add_subcommand(const CommandData* command);

        const std::vector<std::shared_ptr<ParsedArgumentsImpl>>&
        subcommands() const;

        [[nodiscard]] std::vector<std::unique_ptr<IArgumentView>>
        get_argument_views(ValueId value_id) const;

        [[nodiscard]] std::unique_ptr<IArgumentView>
        get_argument_view(ArgumentId argument_id) const;

        [[nodiscard]] const std::shared_ptr<ParserData>& parser_data() const;

        [[nodiscard]] const CommandData* command() const;

        [[nodiscard]] ParserResultCode result_code() const;

        void set_result_code(ParserResultCode result_code);

        [[nodiscard]] const OptionData* stop_option() const;

        void set_breaking_option(const OptionData* option);

        [[noreturn]]
        void error(const std::string& message) const;

        [[noreturn]]
        void error(const std::string& message, ArgumentId argument_id);

    private:
        /// The values received from ArgumentIteratorImpl.
        std::multimap<ValueId, std::pair<std::string, ArgumentId>> m_values;
        /// Index from flag, argument name or alias to ValueId and ArgumentId
        std::vector<std::tuple<std::string_view, ValueId, ArgumentId>> m_ids;
        std::vector<std::string> m_unprocessed_arguments;
        const CommandData* m_command;
        std::vector<std::shared_ptr<ParsedArgumentsImpl>> m_commands;
        std::shared_ptr<ParserData> m_data;
        ParserResultCode m_result_code = ParserResultCode::NONE;
        const OptionData* m_stop_option = nullptr;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    enum class IteratorResultCode
    {
        ARGUMENT,
        OPTION,
        COMMAND,
        DONE,
        UNKNOWN,
        ERROR
    };

    using IteratorResultData = std::variant<
        std::monostate,
        const ArgumentData*,
        const OptionData*,
        const CommandData*>;

    using IteratorResult = std::tuple<
        IteratorResultCode,
        IteratorResultData,
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

        [[nodiscard]] const std::shared_ptr<ParsedArgumentsImpl>&
        parsed_arguments() const;

        [[nodiscard]] const std::shared_ptr<ParsedArgumentsImpl>&
        toplevel_parsed_arguments() const;

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

        IteratorResult process_argument(const std::string& value);

        IteratorResult process_command(const CommandData* command);

        void copy_remaining_arguments_to_parser_result();

        [[nodiscard]] size_t count_arguments() const;

        bool check_argument_and_option_counts();

        [[nodiscard]] std::pair<const CommandData*, size_t>
        find_sibling_command(std::string_view name) const;

        [[nodiscard]]
        std::optional<size_t> find_first_multi_command_parent() const;

        void reactivate_multi_command_parent(size_t index);

        void update_arguments(const std::vector<std::string>& args);

        void error(const std::string& message = {});

        bool has_all_mandatory_options(const ParsedArgumentsImpl& parsed_args,
                                       const CommandData& command);

        std::shared_ptr<ParserData> m_data;
        const CommandData* m_command = nullptr;
        std::vector<std::shared_ptr<ParsedArgumentsImpl>> m_parsed_args;
        OptionIteratorWrapper m_iterator;
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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args,
                                       std::shared_ptr<ParserData> parser_data)
        : m_impl(std::make_unique<ArgumentIteratorImpl>(std::move(args),
                                                        std::move(parser_data)))
    {}

    ArgumentIterator::ArgumentIterator(ArgumentIterator&& rhs) noexcept
        : m_impl(std::move(rhs.m_impl))
    {}

    ArgumentIterator::~ArgumentIterator() = default;

    ArgumentIterator&
    ArgumentIterator::operator=(ArgumentIterator&& rhs) noexcept
    {
        m_impl = std::move(rhs.m_impl);
        return *this;
    }

    bool ArgumentIterator::next(std::unique_ptr<IArgumentView>& arg,
                                std::string_view& value)
    {
        const auto res = impl().next();
        switch (std::get<0>(res))
        {
        case IteratorResultCode::ARGUMENT:
            arg = std::make_unique<ArgumentView>(
                    std::get<const ArgumentData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::OPTION:
            arg = std::make_unique<OptionView>(
                    std::get<const OptionData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::COMMAND:
            arg = std::make_unique<CommandView>(
                    std::get<const CommandData*>(std::get<1>(res)));
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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    void write_help_text(const ParserData& data, const CommandData& cmd);

    void write_error_message(const ParserData& data,
                             const CommandData& cmd,
                             const std::string& msg);

    void write_error_message(const ParserData& data,
                             const CommandData& cmd,
                             const std::string& msg,
                             ArgumentId argument_id);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the Zero-Clause BSD License.
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

    char to_lower(char c);

    void to_lower(std::string& word);

    std::string to_lower(std::string_view word);

    bool is_lower(std::string_view word);

    template <typename T>
    inline void pop_front(std::span<T>& span)
    {
        span = span.subspan(1);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <algorithm>

namespace argos
{
    namespace
    {
        bool is_option(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            return s[0] == (style == OptionStyle::SLASH ? '/' : '-');
        }

        std::variant<OptionIterator, StandardOptionIterator>
        make_option_iterator(OptionStyle style,
                             std::vector<std::string_view> args)
        {
            switch (style)
            {
            case OptionStyle::SLASH:
                return OptionIterator(std::move(args), '/');
            case OptionStyle::DASH:
                return OptionIterator(std::move(args), '-');
            default:
                return StandardOptionIterator(std::move(args));
            }
        }
    }

    ArgumentIteratorImpl::ArgumentIteratorImpl(std::vector<std::string_view> args,
                                               std::shared_ptr<ParserData> data)
        : m_data(std::move(data)),
          m_command(&m_data->command),
          m_parsed_args{std::make_shared<ParsedArgumentsImpl>(m_command, m_data)},
          m_iterator{
              make_option_iterator(m_data->parser_settings.option_style,
                                   std::move(args))
          }
    {
        auto& parsed_args = *parsed_arguments();
        for (const auto& option : m_command->options)
        {
            if (!option->initial_value.empty())
            {
                parsed_args.append_value(option->value_id,
                                         option->initial_value,
                                         option->argument_id);
            }
        }

        if (!ArgumentCounter::requires_argument_count(*m_command))
        {
            m_argument_counter = ArgumentCounter(*m_command);
        }
        else
        {
            m_argument_counter = ArgumentCounter(*m_command,
                                                 count_arguments());
        }
    }

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(std::move(args), data);
        while (true)
        {
            const auto code = std::get<0>(iterator.next());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.toplevel_parsed_arguments();
    }

    IteratorResult ArgumentIteratorImpl::next()
    {
        if (m_state == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_state == State::DONE)
            return {IteratorResultCode::DONE, {}, {}};

        const auto arg = m_state == State::ARGUMENTS_AND_OPTIONS
                             ? m_iterator.next()
                             : m_iterator.next_value();
        if (!arg)
        {
            if (check_argument_and_option_counts())
                return {IteratorResultCode::DONE, {}, {}};
            else
                return {IteratorResultCode::ERROR, {}, {}};
        }

        if (m_state == State::ARGUMENTS_AND_OPTIONS
            && is_option(*arg, m_data->parser_settings.option_style))
        {
            return process_option(*arg);
        }
        else if (m_command->commands.empty())
        {
            return process_argument(*arg);
        }
        else if (auto cmd = m_command->find_command(
            *arg, m_data->parser_settings.case_insensitive))
        {
            return process_command(cmd);
        }
        else
        {
            error("Unknown command: " + std::string(*arg));
            return {IteratorResultCode::ERROR, {}, {}};
        }
    }

    const std::shared_ptr<ParsedArgumentsImpl>&
    ArgumentIteratorImpl::parsed_arguments() const
    {
        return m_parsed_args.back();
    }

    const std::shared_ptr<ParsedArgumentsImpl>&
    ArgumentIteratorImpl::toplevel_parsed_arguments() const
    {
        return m_parsed_args.front();
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::process_option(const OptionData& opt,
                                         const std::string& flag)
    {
        const auto& parsed_args = parsed_arguments();
        std::string_view arg;
        switch (opt.operation)
        {
        case OptionOperation::ASSIGN:
            if (!opt.constant.empty())
            {
                parsed_args->assign_value(opt.value_id, opt.constant,
                                          opt.argument_id);
            }
            else if (const auto value = m_iterator.next_value())
            {
                arg = parsed_args->assign_value(opt.value_id, *value,
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
                parsed_args->append_value(opt.value_id, opt.constant,
                                          opt.argument_id);
            }
            else if (const auto value = m_iterator.next_value())
            {
                arg = parsed_args->append_value(opt.value_id, *value,
                                                opt.argument_id);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            parsed_args->clear_value(opt.value_id);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (opt.callback)
        {
            CallbackArguments args(OptionView(&opt), arg, parsed_args);
            opt.callback(args);
            update_arguments(args.new_arguments);
        }
        if (m_data->parser_settings.option_callback)
        {
            CallbackArguments args(OptionView(&opt), arg, parsed_args);
            m_data->parser_settings.option_callback(args);
            update_arguments(args.new_arguments);
        }

        switch (opt.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            write_help_text(*m_data, *m_command);
            [[fallthrough]];
        case OptionType::EXIT:
            m_state = State::DONE;
            parsed_args->set_breaking_option(&opt);
            return {OptionResult::EXIT, arg};
        case OptionType::STOP:
            m_state = State::DONE;
            parsed_args->set_breaking_option(&opt);
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
        auto option = m_command->find_option(
            flag,
            m_data->parser_settings.allow_abbreviated_options,
            m_data->parser_settings.case_insensitive);
        if (option)
        {
            auto [res, arg] = process_option(*option, flag);
            switch (res)
            {
            case OptionResult::EXIT:
                if (m_data->parser_settings.auto_exit)
                    exit(m_data->parser_settings.normal_exit_code);
                copy_remaining_arguments_to_parser_result();
                return {IteratorResultCode::OPTION, option, arg};
            case OptionResult::ERROR:
                return {IteratorResultCode::ERROR, option, {}};
            case OptionResult::LAST_ARGUMENT:
                if (!check_argument_and_option_counts())
                    return {IteratorResultCode::ERROR, {}, {}};

                if (auto index = find_first_multi_command_parent())
                {
                    reactivate_multi_command_parent(*index);
                    return {IteratorResultCode::OPTION, option, arg};
                }

                [[fallthrough]];
            case OptionResult::STOP:
                copy_remaining_arguments_to_parser_result();
                [[fallthrough]];
            default:
                return {IteratorResultCode::OPTION, option, arg};
            }
        }
        if (!m_data->parser_settings.ignore_undefined_options
            || !starts_with(m_iterator.current(), flag))
        {
            error("Unknown option: " + std::string(m_iterator.current()));
            return {IteratorResultCode::ERROR, {}, {}};
        }
        else
        {
            parsed_arguments()->add_unprocessed_argument(
                std::string(m_iterator.current()));
            return {IteratorResultCode::UNKNOWN, {}, m_iterator.current()};
        }
    }

    IteratorResult
    ArgumentIteratorImpl::process_argument(const std::string& value)
    {
        const auto& parsed_args = parsed_arguments();
        if (auto [next_cmd, i] = find_sibling_command(value); next_cmd)
        {
            reactivate_multi_command_parent(i);
            return process_command(next_cmd);
        }
        if (auto argument = m_argument_counter.next_argument())
        {
            auto s = parsed_args->append_value(argument->value_id, value,
                                               argument->argument_id);
            if (argument->callback)
            {
                CallbackArguments args(ArgumentView(argument), s, parsed_args);
                argument->callback(args);
                update_arguments(args.new_arguments);
            }

            if (m_data->parser_settings.argument_callback)
            {
                CallbackArguments args(ArgumentView(argument), s, parsed_args);
                m_data->parser_settings.argument_callback(args);
                update_arguments(args.new_arguments);
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        if (m_data->parser_settings.ignore_undefined_arguments)
        {
            parsed_args->add_unprocessed_argument(value);
            return {IteratorResultCode::UNKNOWN, {}, m_iterator.current()};
        }

        error("Too many arguments, starting from \"" + value + "\".");
        return {IteratorResultCode::ERROR, {}, {}};
    }

    IteratorResult
    ArgumentIteratorImpl::process_command(const CommandData* command)
    {
        m_parsed_args.push_back(parsed_arguments()->add_subcommand(command));
        m_command = command;
        if (!ArgumentCounter::requires_argument_count(*m_command))
        {
            m_argument_counter = ArgumentCounter(*m_command);
        }
        else
        {
            m_argument_counter = ArgumentCounter(*m_command,
                                                 count_arguments());
        }
        m_state = State::ARGUMENTS_AND_OPTIONS;
        return {IteratorResultCode::COMMAND, command, m_iterator.current()};
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ArgumentIteratorImpl::copy_remaining_arguments_to_parser_result()
    {
        auto& parsed_args = *parsed_arguments();
        for (auto str : m_iterator.remaining_arguments())
            parsed_args.add_unprocessed_argument(std::string(str));
    }

    size_t ArgumentIteratorImpl::count_arguments() const
    {
        size_t result = 0;
        auto it = m_iterator;
        auto [min_count, _] = ArgumentCounter::get_min_max_count(*m_command);
        for (auto arg = it.next(); arg; arg = it.next())
        {
            const OptionData* option = m_command->find_option(
                *arg,
                m_data->parser_settings.allow_abbreviated_options,
                m_data->parser_settings.case_insensitive);

            if (option)
            {
                if (!option->argument.empty())
                    it.next_value();

                bool arguments_only = false;

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

                if (arguments_only)
                    break;
            }
            else if (!is_option(*arg, m_data->parser_settings.option_style))
            {
                if (m_command->find_command(*arg, m_data->parser_settings.case_insensitive))
                    return result;

                if (result >= min_count && find_sibling_command(*arg).first)
                    return result;

                ++result;
            }
        }

        for (auto arg = it.next_value(); arg; arg = it.next_value())
        {
            if (m_command->find_command(*arg, m_data->parser_settings.case_insensitive))
                return result;

            if (result >= min_count && find_sibling_command(*arg).first)
                return result;

            ++result;
        }
        return result;
    }

    bool ArgumentIteratorImpl::check_argument_and_option_counts()
    {
        auto& parsed_args = *parsed_arguments();
        if (!has_all_mandatory_options(parsed_args, *m_command))
            return false;

        if (*m_command->require_subcommand && parsed_args.subcommands().empty())
        {
            error("No command was given.");
            return false;
        }

        if (m_argument_counter.is_complete())
        {
            m_state = State::DONE;
            for (const auto& pa : m_parsed_args)
                pa->set_result_code(ParserResultCode::SUCCESS);
            return true;
        }
        else
        {
            auto [lo, hi] = ArgumentCounter::get_min_max_count(*m_command);
            error((lo == hi
                       ? "Too few arguments. Expected "
                       : "Too few arguments. Expected at least ")
                  + std::to_string(lo) + ", received "
                  + std::to_string(m_argument_counter.count()) + ".");
            return false;
        }
    }

    std::optional<size_t>
    ArgumentIteratorImpl::find_first_multi_command_parent() const
    {
        auto size = m_parsed_args.size();
        if (size <= 1)
            return {};

        for (size_t i = size - 1; i-- > 0;)
        {
            const auto& parent = *m_parsed_args[i]->command();
            if (parent.multi_command)
                return i;
        }

        return {};
    }

    std::pair<const CommandData*, size_t>
    ArgumentIteratorImpl::find_sibling_command(std::string_view name) const
    {
        if (!m_argument_counter.is_complete())
            return {nullptr, 0};

        if (auto index = find_first_multi_command_parent())
        {
            return {
                m_parsed_args[*index]->command()->find_command(
                    name, m_data->parser_settings.case_insensitive),
                *index
            };
        }

        return {nullptr, 0};
    }

    void ArgumentIteratorImpl::reactivate_multi_command_parent(size_t index)
    {
        m_parsed_args.resize(index + 1);
        m_command = m_parsed_args.back()->command();
        m_argument_counter = {};
        m_state = State::ARGUMENTS_ONLY;
    }

    void ArgumentIteratorImpl::update_arguments(
        const std::vector<std::string>& args)
    {
        if (args.empty())
            return;

        m_iterator.insert(args);
        if (!ArgumentCounter::requires_argument_count(*m_command))
            return;

        auto current_count = m_argument_counter.count();
        auto total_count = current_count + count_arguments();
        m_argument_counter = ArgumentCounter(*m_command, total_count,
                                             current_count);
    }

    void ArgumentIteratorImpl::error(const std::string& message)
    {
        if (!message.empty())
            write_error_message(*m_data, *m_command, message);

        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);

        copy_remaining_arguments_to_parser_result();
        for (const auto& parsed_args : m_parsed_args)
            parsed_args->set_result_code(ParserResultCode::FAILURE);
        m_state = State::ERROR;
    }

    bool ArgumentIteratorImpl::has_all_mandatory_options(
        const ParsedArgumentsImpl& parsed_args,
        const CommandData& command)
    {
        for (const auto& o : command.options)
        {
            if (!o->optional && !parsed_args.has(o->value_id))
            {
                auto flags = o->flags.front();
                for (unsigned i = 1; i < o->flags.size(); ++i)
                    flags += ", " + o->flags[i];
                error("Mandatory option is missing: " + flags);
                return false;
            }
        }

        return true;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <cstring>
#include <iostream>

namespace argos
{
    namespace
    {
        std::unique_ptr<ParserData> make_copy(const ParserData& data)
        {
            auto result = std::make_unique<ParserData>();
            result->parser_settings = data.parser_settings;
            result->help_settings = data.help_settings;
            result->command = data.command;
            return result;
        }

        constexpr char DEFAULT_NAME[] = "UNINITIALIZED";

        ParsedArguments parse_arguments(std::vector<std::string_view> args,
                                        const std::shared_ptr<ParserData>& data)
        {
            finish_initialization(*data);
            return ParsedArguments(
                ArgumentIteratorImpl::parse(std::move(args), data));
        }

        ArgumentIterator
        make_argument_iterator(std::vector<std::string_view> args,
                               const std::shared_ptr<ParserData>& data)
        {
            finish_initialization(*data);
            return {std::move(args), data};
        }
    }

    ArgumentParser::ArgumentParser()
        : ArgumentParser(DEFAULT_NAME)
    {}

    ArgumentParser::ArgumentParser(std::string_view program_name,
                                   bool extract_file_name)
        : m_data(std::make_unique<ParserData>())
    {
        m_data->command.name = extract_file_name
                                   ? get_base_name(program_name)
                                   : program_name;
    }

    ArgumentParser::ArgumentParser(ArgumentParser&& rhs) noexcept
        : m_data(std::move(rhs.m_data))
    {
    }

    ArgumentParser::~ArgumentParser() = default;

    ArgumentParser& ArgumentParser::operator=(ArgumentParser&& rhs) noexcept
    {
        m_data = std::move(rhs.m_data);
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Argument& argument)
    {
        return add(std::move(argument));
    }

    ArgumentParser& ArgumentParser::add(Argument&& argument)
    {
        check_data();
        m_data->command.add(argument.release());
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option& option)
    {
        return add(std::move(option));
    }

    ArgumentParser& ArgumentParser::add(Option&& option)
    {
        check_data();
        m_data->command.add(option.release());
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Command& command)
    {
        return add(std::move(command));
    }

    ArgumentParser& ArgumentParser::add(Command&& command)
    {
        check_data();
        m_data->command.add(command.release());
        return *this;
    }

    ArgumentParser& ArgumentParser::copy_from(const Command& command)
    {
        check_data();
        m_data->command.copy_from(command.internal_ref());
        return *this;
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv)
    {
        if (argc <= 0)
            return parse(std::vector<std::string_view>());

        if (m_data->command.name == DEFAULT_NAME
            && std::strlen(argv[0]) != 0)
        {
            m_data->command.name = get_base_name(argv[0]);
        }

        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv) const
    {
        if (argc <= 0)
            return parse(std::vector<std::string_view>());
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args)
    {
        check_data();
        return parse_arguments(std::move(args), std::move(m_data));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args) const
    {
        check_data();
        return parse_arguments(std::move(args), make_copy(*m_data));
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
        return make_argument_iterator(std::move(args), std::move(m_data));
    }

    ArgumentIterator
    ArgumentParser::make_iterator(std::vector<std::string_view> args) const
    {
        check_data();
        return make_argument_iterator(std::move(args), make_copy(*m_data));
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
            if (!m_data->command.options.empty())
                ARGOS_THROW("Can't change option style after"
                            " options have been added.");
            m_data->parser_settings.option_style = value;
        }
        return *this;
    }

    std::optional<bool> ArgumentParser::require_subcommand() const
    {
        check_data();
        return m_data->command.require_subcommand;
    }

    ArgumentParser& ArgumentParser::require_subcommand(bool value)
    {
        check_data();
        m_data->command.require_subcommand = value;
        return *this;
    }

    bool ArgumentParser::allow_multiple_subcommands() const
    {
        check_data();
        return m_data->command.multi_command;
    }

    ArgumentParser& ArgumentParser::allow_multiple_subcommands(bool value)
    {
        check_data();
        m_data->command.multi_command = value;
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
        return m_data->command.name;
    }

    ArgumentParser& ArgumentParser::program_name(const std::string& name)
    {
        check_data();
        m_data->command.name = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::about(std::string text)
    {
        return this->text(TextId::ABOUT, std::move(text));
    }

    ArgumentParser& ArgumentParser::text(TextId textId, std::string text)
    {
        check_data();
        m_data->command.texts[textId] = std::move(text);
        return *this;
    }

    ArgumentParser& ArgumentParser::text(TextId textId,
                                         std::function<std::string()> callback)
    {
        check_data();
        m_data->command.texts[textId] = std::move(callback);
        return *this;
    }

    ArgumentParser& ArgumentParser::version(const std::string& version)
    {
        check_data();
        m_data->version = version;
        return *this;
    }

    ArgumentParser& ArgumentParser::current_section(const std::string& name)
    {
        check_data();
        m_data->command.current_section = name;
        return *this;
    }

    const std::string& ArgumentParser::current_section() const
    {
        check_data();
        return m_data->command.current_section;
    }

    ArgumentParser& ArgumentParser::section(const std::string& name)
    {
        return current_section(name);
    }

    ArgumentParser& ArgumentParser::line_width(unsigned int line_width)
    {
        check_data();
        m_data->help_settings.line_width = line_width;
        return *this;
    }

    void ArgumentParser::write_help_text() const
    {
        write_subcommand_help_text({});
    }

    void ArgumentParser::write_subcommand_help_text(const std::vector<std::string>& path) const
    {
        check_data();
        const auto data = make_copy(*m_data);
        finish_initialization(*data);
        const auto* cmd = &data->command;
        for (auto& name : path)
        {
            cmd = cmd->find_command(name, data->parser_settings.case_insensitive);
            if (!cmd)
                ARGOS_THROW("Unknown command: " + name);
        }
        argos::write_help_text(*data, *cmd);
    }

    ArgumentParser& ArgumentParser::add_word_splitting_rule(std::string str)
    {
        check_data();
        m_data->help_settings.word_split_rules.push_back(std::move(str));
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
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <cerrno>
#include <cinttypes>
#include <type_traits>

namespace argos
{
    template <typename T>
    std::optional<T> parse_integer(const std::string& str, int base) = delete;

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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        template <typename T>
        T get_integer(const ArgumentValue& value, T default_value, int base)
        {
            const auto s = value.value();
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
            const auto s = value.value();
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

    ArgumentValue::ArgumentValue(const std::optional<std::string_view>& value,
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
            return {{}, m_args, m_value_id};
        auto parts = split_string(*m_value, separator, max_parts - 1);
        if (parts.size() < min_parts)
        {
            error("Invalid value: \"" + std::string(*m_value)
                  + "\". Must have at least " + std::to_string(min_parts)
                  + " values separated by \"" + separator + "\".");
        }
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        values.reserve(parts.size());
        for (auto& part : parts)
            values.emplace_back(part, m_argument_id);
        return {std::move(values), m_args, m_value_id};
    }

    ArgumentValues
    ArgumentValue::split_n(char separator, size_t num_parts) const
    {
        return split(separator, num_parts, num_parts);
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
// This file is distributed under the Zero-Clause BSD License.
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

    ArgumentValueIterator ArgumentValueIterator::operator++(int) &
    {
        auto it = *this;
        ++m_iterator;
        return it;
    }

    ArgumentValue ArgumentValueIterator::operator*() const
    {
        return {m_iterator->first, m_args,
                m_value_id, m_iterator->second};
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
// This file is distributed under the Zero-Clause BSD License.
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
        : m_values(std::move(values)),
          m_args(std::move(args)),
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

    ArgumentValue ArgumentValues::operator[](size_t index) const
    {
        return value(index);
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
        result.reserve(m_values.size());
        for (const auto& [value, arg_id] : m_values)
            result.emplace_back(value, m_args, m_value_id, arg_id);
        return result;
    }

    std::vector<std::string_view> ArgumentValues::raw_values() const
    {
        std::vector<std::string_view> result;
        result.reserve(m_values.size());
        for (const auto& [value, _] : m_values)
            result.push_back(value);
        return result;
    }

    ArgumentValue ArgumentValues::value(size_t index) const
    {
        if (m_values.empty())
            return {{}, m_args, m_value_id, {}};

        const auto& [value, arg_id] = m_values.at(index);
        return {value, m_args, m_value_id, arg_id};
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
        for (const auto& [value, _] : m_values)
            result.emplace_back(value);
        return result;
    }

    ArgumentValues
    ArgumentValues::split(char separator,
                          size_t min_parts, size_t max_parts) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        for (auto [value, arg_id] : m_values)
        {
            auto parts = split_string(value, separator, max_parts - 1);
            if (parts.size() < min_parts)
            {
                error("Invalid value: \"" + std::string(value)
                      + "\". Must be at least " + std::to_string(min_parts)
                      + " values separated by \"" + separator + "\".");
            }
            for (auto& part : parts)
                values.emplace_back(part, arg_id);
        }
        return {std::move(values), m_args, m_value_id};
    }

    ArgumentValues
    ArgumentValues::split_n(char separator, size_t num_parts) const
    {
        return split(separator, num_parts, num_parts);
    }

    ArgumentValueIterator ArgumentValues::begin() const
    {
        return {m_values.begin(), m_args, m_value_id};
    }

    ArgumentValueIterator ArgumentValues::end() const
    {
        return {m_values.end(), m_args, m_value_id};
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the Zero-Clause BSD License.
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

    std::string ArgumentView::help() const
    {
        return get_text(m_argument->help);
    }

    const std::string& ArgumentView::section() const
    {
        return m_argument->section;
    }

    const std::string& ArgumentView::alias() const
    {
        return m_argument->alias;
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
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    Command::Command()
        : data_(std::make_unique<CommandData>())
    {}

    Command::Command(std::string name)
        : data_(std::make_unique<CommandData>())
    {
        data_->name = std::move(name);
    }

    Command::Command(const Command& rhs)
        : data_(rhs.data_ ? std::make_unique<CommandData>(*rhs.data_) : nullptr)
    {
    }

    Command::Command(Command&& rhs) noexcept
        : data_(std::move(rhs.data_))
    {
    }

    Command::~Command() = default;

    Command& Command::operator=(const Command& rhs)
    {
        if (this != &rhs)
        {
            data_ = rhs.data_
                        ? std::make_unique<CommandData>(*rhs.data_)
                        : nullptr;
        }
        return *this;
    }

    Command& Command::operator=(Command&& rhs) noexcept
    {
        data_ = std::move(rhs.data_);
        return *this;
    }

    Command& Command::add(Argument& argument)
    {
        return add(std::move(argument));
    }

    Command& Command::add(Argument&& argument)
    {
        check_command();
        data_->add(argument.release());
        return *this;
    }

    Command& Command::add(Option& option)
    {
        return add(std::move(option));
    }

    Command& Command::add(Option&& option)
    {
        check_command();
        data_->add(option.release());
        return *this;
    }

    Command& Command::add(Command& command)
    {
        return add(std::move(command));
    }

    Command& Command::add(Command&& command)
    {
        check_command();
        data_->add(command.release());
        return *this;
    }

    Command& Command::name(std::string name)
    {
        check_command();
        data_->name = std::move(name);
        return *this;
    }

    Command& Command::help(std::string text)
    {
        check_command();
        data_->texts[TextId::HELP] = std::move(text);
        return *this;
    }

    Command& Command::about(std::string text)
    {
        check_command();
        data_->texts[TextId::ABOUT] = std::move(text);
        return *this;
    }

    Command& Command::section(std::string name)
    {
        check_command();
        data_->section = std::move(name);
        return *this;
    }

    Command& Command::current_section(std::string name)
    {
        check_command();
        data_->current_section = std::move(name);
        return *this;
    }

    Command& Command::text(TextId textId, std::string text)
    {
        check_command();
        data_->texts[textId] = std::move(text);
        return *this;
    }

    Command& Command::text(TextId textId, std::function<std::string()> callback)
    {
        check_command();
        data_->texts[textId] = std::move(callback);
        return *this;
    }

    Command& Command::visibility(Visibility visibility)
    {
        check_command();
        data_->visibility = visibility;
        return *this;
    }

    Command& Command::id(int id)
    {
        check_command();
        data_->id = id;
        return *this;
    }

    Command& Command::allow_multiple_subcommands(bool multi_command)
    {
        check_command();
        data_->multi_command = multi_command;
        return *this;
    }

    Command& Command::require_subcommand(bool require_subcommand)
    {
        check_command();
        data_->require_subcommand = require_subcommand;
        return *this;
    }

    Command& Command::copy_from(Command& command)
    {
        check_command();
        data_->copy_from(*command.data_);
        return *this;
    }

    std::unique_ptr<CommandData> Command::release()
    {
        return std::move(data_);
    }

    const CommandData& Command::internal_ref() const
    {
        check_command();
        return *data_;
    }

    void Command::check_command() const
    {
        if (!data_)
            ARGOS_THROW("Command has been moved.");
    }
}

//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    constexpr char DEFAULT_HELP_TEXT[] = "Display this help text.";

    CommandData::CommandData() = default;

    CommandData::CommandData(const CommandData& rhs)
        : current_section(rhs.current_section),
          name(rhs.name),
          full_name(rhs.full_name),
          texts(rhs.texts),
          require_subcommand(rhs.require_subcommand),
          multi_command(rhs.multi_command),
          section(rhs.section),
          id(rhs.id),
          argument_id(rhs.argument_id)
    {
        arguments.reserve(rhs.arguments.size());
        for (const auto& a : rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));
        options.reserve(rhs.options.size());
        for (const auto& o : rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));
        commands.reserve(rhs.commands.size());
        for (const auto& c : rhs.commands)
            commands.push_back(std::make_unique<CommandData>(*c));
    }

    CommandData::CommandData(CommandData&& rhs) noexcept
        : arguments(std::move(rhs.arguments)),
          options(std::move(rhs.options)),
          commands(std::move(rhs.commands)),
          current_section(std::move(rhs.current_section)),
          name(std::move(rhs.name)),
          full_name(std::move(rhs.full_name)),
          texts(std::move(rhs.texts)),
          require_subcommand(rhs.require_subcommand),
          multi_command(rhs.multi_command),
          section(std::move(rhs.section)),
          id(rhs.id),
          argument_id(rhs.argument_id)
    {
    }

    CommandData::~CommandData() = default;

    CommandData& CommandData::operator=(const CommandData& rhs)
    {
        if (&rhs == this)
            return *this;
        current_section = rhs.current_section;
        name = rhs.name;
        full_name = rhs.full_name;
        texts = rhs.texts;
        require_subcommand = rhs.require_subcommand;
        multi_command = rhs.multi_command;
        section = rhs.section;
        id = rhs.id;
        argument_id = rhs.argument_id;

        arguments.clear();
        arguments.reserve(rhs.arguments.size());
        for (const auto& a : rhs.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));

        options.clear();
        options.reserve(rhs.options.size());
        for (const auto& o : rhs.options)
            options.push_back(std::make_unique<OptionData>(*o));

        commands.clear();
        commands.reserve(rhs.commands.size());
        for (const auto& c : rhs.commands)
            commands.push_back(std::make_unique<CommandData>(*c));

        return *this;
    }

    CommandData& CommandData::operator=(CommandData&& rhs) noexcept
    {
        if (&rhs == this)
            return *this;

        current_section = std::move(rhs.current_section);
        name = std::move(rhs.name);
        full_name = std::move(rhs.full_name);
        texts = std::move(rhs.texts);
        arguments = std::move(rhs.arguments);
        options = std::move(rhs.options);
        commands = std::move(rhs.commands);
        require_subcommand = rhs.require_subcommand;
        multi_command = rhs.multi_command;
        section = std::move(rhs.section);
        id = rhs.id;
        argument_id = rhs.argument_id;
        return *this;
    }

    void CommandData::add(std::unique_ptr<ArgumentData> arg)
    {
        if (!arg)
            ARGOS_THROW("Argument is empty (it has probably already been added).");
        if (!commands.empty())
            ARGOS_THROW("A command cannot have both arguments and subcommands.");
        if (arg->name.empty())
            ARGOS_THROW("Argument must have a name.");
        if (arg->section.empty())
            arg->section = current_section;
        arguments.emplace_back(std::move(arg));
    }

    void CommandData::add(std::unique_ptr<OptionData> opt)
    {
        if (!opt)
            ARGOS_THROW("Option is empty (it has probably already been added).");
        if (opt->flags.empty())
            ARGOS_THROW("Option must have at least one flag.");
        if (opt->section.empty())
            opt->section = current_section;
        options.push_back(std::move(opt));
    }

    void CommandData::add(std::unique_ptr<CommandData> cmd)
    {
        if (!cmd)
            ARGOS_THROW("Command is empty (it has probably already been added).");
        if (!arguments.empty())
            ARGOS_THROW("A command cannot have both arguments and subcommands.");
        if (cmd->name.empty())
            ARGOS_THROW("Command must have a name.");
        if (cmd->section.empty())
            cmd->section = current_section;
        commands.push_back(std::move(cmd));
    }

    void CommandData::copy_from(const CommandData& cmd)
    {
        for (const auto& a : cmd.arguments)
            arguments.push_back(std::make_unique<ArgumentData>(*a));

        for (const auto& o : cmd.options)
            options.push_back(std::make_unique<OptionData>(*o));

        for (const auto& c : cmd.commands)
            commands.push_back(std::make_unique<CommandData>(*c));

        for (const auto& [text_id, source] : cmd.texts)
        {
            if (texts.contains(text_id))
                ARGOS_THROW("Text with ID " + to_string(text_id) + " already exists.");
            texts.emplace(text_id, source);
        }

        if (!multi_command)
            multi_command = cmd.multi_command;
    }

    void CommandData::build_option_index(bool case_insensitive)
    {
        std::vector<std::pair<std::string_view, const OptionData*>> index;
        for (auto& option : options)
        {
            for (auto& flag : option->flags)
                index.emplace_back(flag, option.get());
        }

        sort(index.begin(), index.end(), [&](const auto& a, const auto& b)
        {
            return is_less(a.first, b.first, case_insensitive);
        });

        const auto it = adjacent_find(
            index.begin(), index.end(),
            [&](const auto& a, const auto& b)
            {
                return are_equal(a.first, b.first, case_insensitive);
            });

        if (it == index.end())
        {
            option_index = std::move(index);
            return;
        }

        if (it->first == next(it)->first)
        {
            ARGOS_THROW("Multiple definitions of flag "
                + std::string(it->first));
        }

        ARGOS_THROW("Conflicting flags: " + std::string(it->first)
            + " and " + std::string(next(it)->first));
    }

    const OptionData* CommandData::find_option(std::string_view flag,
                                               bool allow_abbreviations,
                                               bool case_insensitive) const
    {
        auto opt = find_option_impl(flag, allow_abbreviations,
                                    case_insensitive);
        if (opt == nullptr && flag.size() > 2 && flag.back() == '=')
        {
            flag = flag.substr(0, flag.size() - 1);
            opt = find_option_impl(flag, allow_abbreviations,
                                   case_insensitive);
            if (opt && opt->argument.empty())
                opt = nullptr;
        }
        return opt;
    }

    const OptionData* CommandData::find_option_impl(std::string_view flag,
                                                    bool allow_abbreviations,
                                                    bool case_insensitive) const
    {
        const auto it = std::lower_bound(
            option_index.begin(), option_index.end(),
            std::pair(flag, nullptr),
            [&](auto& a, auto& b)
            {
                return is_less(a.first, b.first, case_insensitive);
            });
        if (it == option_index.end())
            return nullptr;
        if (it->first == flag)
            return it->second;
        if (case_insensitive && are_equal_ci(it->first, flag))
            return it->second;
        if (!allow_abbreviations)
            return nullptr;
        if (!starts_with(it->first, flag, case_insensitive))
            return nullptr;
        const auto nxt = next(it);
        if (nxt != option_index.end()
            && starts_with(nxt->first, flag, case_insensitive))
        {
            return nullptr;
        }
        return it->second;
    }

    const CommandData* CommandData::find_command(std::string_view cmd_name,
                                                 bool case_insensitive) const
    {
        for (const auto& c : commands)
        {
            if (are_equal(c->name, cmd_name, case_insensitive))
                return c.get();
        }
        return nullptr;
    }

    namespace
    {
        void update_require_command(CommandData& cmd)
        {
            if (cmd.require_subcommand.value_or(false) && cmd.commands.empty())
                ARGOS_THROW("require_command is true, but no commands have been added.");

            if (!cmd.require_subcommand)
            {
                cmd.require_subcommand = !cmd.commands.empty();
            }
        }

        bool has_help_option(const CommandData& cmd)
        {
            for (const auto& o : cmd.options)
            {
                if (o->type == OptionType::HELP)
                    return true;
            }
            return false;
        }

        void add_help_option(CommandData& cmd, const ParserSettings& settings)
        {
            if (!settings.generate_help_option)
                return;
            if (has_help_option(cmd))
                return;
            std::vector<std::string> flags;
            switch (settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(cmd, "-h", settings))
                    flags.emplace_back("-h");
                if (!has_flag(cmd, "--help", settings))
                    flags.emplace_back("--help");
                break;
            case OptionStyle::SLASH:
                if (!has_flag(cmd, "/?", settings))
                    flags.emplace_back("/?");
                break;
            case OptionStyle::DASH:
                if (!has_flag(cmd, "-h", settings))
                    flags.emplace_back("-h");
                else if (!has_flag(cmd, "-help", settings))
                    flags.emplace_back("-help");
                break;
            }

            if (flags.empty())
                return;

            auto opt = Option().flags(std::move(flags)).type(OptionType::HELP)
                .help(DEFAULT_HELP_TEXT)
                .constant("1").release();
            opt->section = cmd.current_section;
            cmd.options.push_back(std::move(opt));
        }

        std::pair<ValueId, ArgumentId>
        set_internal_ids(const CommandData& cmd,
                         ValueId value_id_offset,
                         ArgumentId argument_id_offset)
        {
            struct InternalIdMaker
            {
                std::map<std::string_view, ValueId> explicit_ids;
                int id;

                explicit InternalIdMaker(ValueId start_id)
                    : id(start_id)
                {
                }

                std::optional<ValueId> find_value_id(std::string_view name)
                {
                    const auto it = explicit_ids.find(name);
                    if (it == explicit_ids.end())
                        return {};
                    return it->second;
                }

                ValueId get_value_id(std::string_view name)
                {
                    if (const auto value_id = find_value_id(name))
                        return *value_id;
                    id++;
                    explicit_ids.emplace(name, ValueId(id));
                    return ValueId(id);
                }

                ValueId get_value_id(const std::vector<std::string>& names)
                {
                    for (const auto& name : names)
                    {
                        if (const auto value_id = find_value_id(name))
                            return *value_id;
                    }
                    id++;
                    for (const auto& name : names)
                        explicit_ids.emplace(name, ValueId(id));
                    return ValueId(id);
                }
            };

            int argument_id = argument_id_offset;
            InternalIdMaker id_maker(value_id_offset);
            for (const auto& a : cmd.arguments)
            {
                a->argument_id = ArgumentId(++argument_id);
                if (!a->alias.empty())
                {
                    a->value_id = id_maker.get_value_id(a->alias);
                    id_maker.explicit_ids.emplace(a->name, a->value_id);
                }
                else
                {
                    a->value_id = id_maker.get_value_id(a->name);
                }
            }
            for (const auto& o : cmd.options)
            {
                o->argument_id = ArgumentId(++argument_id);
                if (o->operation == OptionOperation::NONE)
                    continue;
                if (!o->alias.empty())
                {
                    o->value_id = id_maker.get_value_id(o->alias);
                    for (auto& f : o->flags)
                        id_maker.explicit_ids.emplace(f, o->value_id);
                }
                else
                {
                    o->value_id = id_maker.get_value_id(o->flags);
                }
            }

            return {ValueId(id_maker.id), ArgumentId(argument_id)};
        }
    }

    bool has_flag(const CommandData& cmd,
                  std::string_view flag,
                  const ParserSettings& settings)
    {
        for (auto& o : cmd.options)
        {
            for (auto& f : o->flags)
            {
                if (are_equal(f, flag, settings.case_insensitive))
                    return true;
            }
        }
        return false;
    }

    // NOLINT(*-no-recursion)
    void finish_initialization(CommandData& cmd,
                               const ParserData& data,
                               ValueId start_id,
                               ArgumentId argument_id)
    {
        if (cmd.full_name.empty())
            cmd.full_name = cmd.name;
        for (auto& o : cmd.options)
            validate_and_update(*o, data.parser_settings.option_style);
        update_require_command(cmd);
        add_help_option(cmd, data.parser_settings);

        argument_id = ArgumentId(argument_id + 1);
        cmd.argument_id = argument_id;
        std::tie(start_id, argument_id) = set_internal_ids(cmd, start_id, argument_id);

        cmd.build_option_index(data.parser_settings.case_insensitive);
        for (auto& c : cmd.commands)
        {
            c->full_name = cmd.name + ' ' + c->name;
            finish_initialization(*c, data, start_id, argument_id);
        }
    }
}

//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-21.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    CommandView::CommandView(const CommandData* command)
        : m_command(command)
    {}

    std::string CommandView::help() const
    {
        auto it = m_command->texts.find(TextId::ABOUT);
        return it != m_command->texts.end() ? get_text(it->second) : "";
    }

    const std::string& CommandView::section() const
    {
        return m_command->section;
    }

    const std::string& CommandView::alias() const
    {
        return m_command->name;
    }

    Visibility CommandView::visibility() const
    {
        return m_command->visibility;
    }

    int CommandView::id() const
    {
        return m_command->id;
    }

    ValueId CommandView::value_id() const
    {
        return {};
    }

    ArgumentId CommandView::argument_id() const
    {
        return m_command->argument_id;
    }

    std::string CommandView::name() const
    {
        return m_command->name;
    }

    std::vector<ArgumentView> CommandView::arguments() const
    {
        std::vector<ArgumentView> result;
        for (const auto& arg : m_command->arguments)
            result.emplace_back(arg.get());
        return result;
    }

    std::vector<OptionView> CommandView::options() const
    {
        std::vector<OptionView> result;
        for (const auto& opt : m_command->options)
            result.emplace_back(opt.get());
        return result;
    }

    std::vector<CommandView> CommandView::subcommands() const
    {
        std::vector<CommandView> result;
        for (const auto& cmd : m_command->commands)
            result.emplace_back(cmd.get());
        return result;
    }

    bool CommandView::require_subcommand() const
    {
        // Instances of CommandView are only created after require_subcommand
        // has been automatically set, the or-value should therefore never
        // be returned, and it doesn't matter that it might not be correct.
        return m_command->require_subcommand.value_or(false);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the Zero-Clause BSD License.
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
// This file is distributed under the Zero-Clause BSD License.
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
        const HANDLE h_con = GetStdHandle(STD_OUTPUT_HANDLE);
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
        const auto width = get_console_width();
        if (width == 0)
            return default_width;
        return width < min_width ? min_width : width;
    }
}

//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-10-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
#define ENUM_CASE(x) case TextId::x: return #x

    std::string to_string(TextId textId)
    {
        switch (textId)
        {
        ENUM_CASE(INITIAL_TEXT);
        ENUM_CASE(USAGE_TITLE);
        ENUM_CASE(USAGE);
        ENUM_CASE(ABOUT);
        ENUM_CASE(SUBCOMMANDS_TITLE);
        ENUM_CASE(ARGUMENTS_TITLE);
        ENUM_CASE(OPTIONS_TITLE);
        ENUM_CASE(FINAL_TEXT);
        ENUM_CASE(ERROR_USAGE);
        ENUM_CASE(HELP);
        default:
            return "<Unknown>";
        }
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        constexpr auto DEFAULT_SUBCOMMANDS_TITLE = "COMMANDS";
        constexpr auto DEFAULT_ARGUMENTS_TITLE = "ARGUMENTS";
        constexpr auto DEFAULT_OPTIONS_TITLE = "OPTIONS";
        constexpr auto DEFAULT_USAGE_TITLE = "USAGE";

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
            const bool braces = opt.optional
                                && !is_stop_option(opt.type);
            if (braces)
                opt_txt.push_back('[');

            std::string_view flag;
            if (prefer_long_flag)
            {
                const auto it = std::find_if(opt.flags.begin(), opt.flags.end(),
                                             [](auto& s) { return s.size() > 2; });
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
        get_custom_text(const CommandData& data, TextId text_id)
        {
            const auto it = data.texts.find(text_id);
            if (it != data.texts.end())
            {
                return get_text(it->second);
            }
            return {};
        }

        bool is_empty(const std::optional<std::string>& str)
        {
            return !str || str->empty();
        }

        std::optional<std::string>
        write_custom_text(TextFormatter& formatter,
                          const CommandData& cmd,
                          TextId text_id,
                          bool prepend_newline = false)
        {
            auto text = get_custom_text(cmd, text_id);
            if (!is_empty(text))
            {
                if (prepend_newline)
                    formatter.newline();
                formatter.write_words(*text);
                if (!formatter.is_current_line_empty())
                    formatter.newline();
            }
            return text;
        }

        void write_stop_and_help_usage(TextFormatter& formatter,
                                       const CommandData& cmd)
        {
            for (auto& opt : cmd.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN
                    || !is_stop_option(opt->type))
                {
                    continue;
                }

                formatter.write_words(cmd.full_name);
                formatter.write_words(" ");
                formatter.push_indentation(TextFormatter::CURRENT_COLUMN);
                formatter.write_lines(get_brief_option_name(*opt, true));
                formatter.write_words(" ");
                formatter.pop_indentation();
                formatter.newline();
            }
        }

        using HelpText = std::pair<std::string, std::string>;
        using HelpTextVector = std::vector<HelpText>;
        using SectionHelpTexts = std::pair<std::string_view, HelpTextVector>;

        unsigned int get_help_text_label_width(
            const TextFormatter& formatter,
            const std::vector<SectionHelpTexts>& sections)
        {
            // Determine what width should be reserved for the argument names
            // and option flags.
            std::vector<unsigned> name_widths;
            std::vector<unsigned> text_widths;
            for (const auto& [_, help_texts] : sections)
            {
                for (const auto& [name, txt] : help_texts)
                {
                    name_widths.push_back(static_cast<unsigned>(name.size()));
                    text_widths.push_back(static_cast<unsigned>(txt.size()));
                }
            }

            std::sort(name_widths.begin(), name_widths.end());
            std::sort(text_widths.begin(), text_widths.end());
            const auto line_width = formatter.line_width();
            // Check if both the longest name and the longest help text
            // can fit on the same line.
            const auto name_width = name_widths.back() + 3;
            if (name_width > 32 || name_width + text_widths.back() > line_width)
                return 0;
            return name_width;
        }

        void write_argument_sections(TextFormatter& formatter,
                                     const CommandData& command,
                                     bool prepend_newline)
        {
            std::vector<SectionHelpTexts> sections;

            auto add_help_text = [&](std::string_view s, std::string a, std::string b)
            {
                auto it = find_if(sections.begin(), sections.end(),
                                  [&](const auto& v)
                                  {
                                      return v.first == s;
                                  });
                if (it == sections.end())
                {
                    sections.push_back({s, {}});
                    it = std::prev(sections.end());
                }
                it->second.emplace_back(std::move(a), std::move(b));
            };

            // List all arguments
            auto arg_title = get_custom_text(command, TextId::ARGUMENTS_TITLE);
            if (!arg_title)
                arg_title = DEFAULT_ARGUMENTS_TITLE;
            for (auto& a : command.arguments)
            {
                if ((a->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = a->section.empty() ? *arg_title : a->section;
                add_help_text(section, get_argument_name(*a), get_text(a->help));
            }

            // List all sub-commands after arguments, as arguments must be
            // given first.
            auto cmd_title = get_custom_text(command, TextId::SUBCOMMANDS_TITLE);
            if (!cmd_title)
                cmd_title = DEFAULT_SUBCOMMANDS_TITLE;
            for (auto& c : command.commands)
            {
                if ((c->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = c->section.empty() ? *cmd_title : c->section;
                add_help_text(section, c->name,
                              get_custom_text(*c, TextId::HELP).value_or(""));
            }

            // List all options.
            auto opt_title = get_custom_text(command, TextId::OPTIONS_TITLE);
            if (!opt_title)
                opt_title = DEFAULT_OPTIONS_TITLE;
            for (auto& o : command.options)
            {
                if ((o->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = o->section.empty() ? *opt_title : o->section;
                add_help_text(section, get_long_option_name(*o), get_text(o->help));
            }

            if (sections.empty())
                return;

            const unsigned name_width = get_help_text_label_width(formatter, sections);

            for (auto& [section, texts] : sections)
            {
                if (prepend_newline)
                    formatter.newline();
                formatter.write_words(section);
                formatter.newline();
                formatter.push_indentation(2);
                for (auto& [name, text] : texts)
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

        void write_brief_regular_options(TextFormatter& formatter,
                                         const CommandData& command)
        {
            for (auto& opt : command.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN
                    || is_stop_option(opt->type))
                {
                    continue;
                }

                formatter.write_lines(get_brief_option_name(*opt, false));
                formatter.write_words(" ");
            }
        }

        void write_brief_arguments(TextFormatter& formatter,
                                   const CommandData& command)
        {
            for (auto& arg : command.arguments)
            {
                if ((arg->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                formatter.write_lines(get_argument_name(*arg));
                formatter.write_words(" ");
            }
        }

        void write_brief_commands(TextFormatter& formatter,
                                  const CommandData& command)
        {
            if (command.commands.empty())
                return;

            auto brackets = !command.require_subcommand.value_or(false);

            if (brackets)
                formatter.write_words("[");

            bool first_command = true;
            for (auto& cmd : command.commands)
            {
                if ((cmd->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (!first_command)
                    formatter.write_words("|");
                first_command = false;
                formatter.write_words(cmd->name);
            }

            if (brackets)
                formatter.write_words("]");
        }

        void write_brief_usage(TextFormatter& formatter,
                               const CommandData& command,
                               bool prepend_newline)
        {
            if (prepend_newline)
                formatter.newline();

            formatter.push_indentation(2);
            write_stop_and_help_usage(formatter, command);
            formatter.write_words(command.full_name);
            formatter.write_words(" ");
            formatter.push_indentation(TextFormatter::CURRENT_COLUMN);

            write_brief_regular_options(formatter, command);
            write_brief_arguments(formatter, command);
            write_brief_commands(formatter, command);

            formatter.pop_indentation();
            formatter.newline();
            formatter.pop_indentation();
        }

        bool write_usage(TextFormatter& formatter,
                         const CommandData& command,
                         bool prepend_newline = false)
        {
            if (const auto t = get_custom_text(command, TextId::USAGE); t && t->empty())
                return false;

            const auto text1 = write_custom_text(formatter, command,
                                                 TextId::USAGE_TITLE,
                                                 prepend_newline);
            if (!text1)
            {
                if (prepend_newline)
                    formatter.newline();
                formatter.write_words(DEFAULT_USAGE_TITLE);
                formatter.newline();
                prepend_newline = false;
            }
            else
            {
                prepend_newline = prepend_newline && is_empty(text1);
            }
            const auto text2 = write_custom_text(formatter, command,
                                                 TextId::USAGE,
                                                 prepend_newline);
            if (text2)
                return !is_empty(text1) || !is_empty(text2);
            write_brief_usage(formatter, command, prepend_newline);
            return true;
        }

        std::string get_name(const CommandData& data, ArgumentId argument_id)
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

    void write_help_text(const ParserData& data, const CommandData& cmd)
    {
        TextFormatter formatter;
        if (data.help_settings.line_width)
            formatter.set_line_width(data.help_settings.line_width);
        if (data.help_settings.output_stream)
            formatter.set_stream(data.help_settings.output_stream);
        formatter.word_splitter().add_words(data.help_settings.word_split_rules);
        bool newline = !is_empty(write_custom_text(formatter, cmd, TextId::INITIAL_TEXT));
        newline = write_usage(formatter, cmd, newline) || newline;
        newline = !is_empty(write_custom_text(formatter, cmd, TextId::ABOUT, newline))
                  || !is_empty(write_custom_text(formatter, cmd, TextId::HELP, newline))
                  || newline;
        write_argument_sections(formatter, cmd, newline);
        write_custom_text(formatter, cmd, TextId::FINAL_TEXT, true);
    }

    void write_error_message(const ParserData& data,
                             const CommandData& cmd,
                             const std::string& msg)
    {
        TextFormatter formatter;
        if (data.help_settings.line_width)
            formatter.set_line_width(data.help_settings.line_width);
        if (data.help_settings.output_stream)
            formatter.set_stream(data.help_settings.output_stream);
        else
            formatter.set_stream(&std::cerr);
        formatter.word_splitter().add_words(data.help_settings.word_split_rules);
        formatter.write_words(cmd.full_name + ": ");
        formatter.write_words(msg);
        formatter.newline();
        if (!write_custom_text(formatter, cmd, TextId::ERROR_USAGE))
            write_usage(formatter, cmd);
    }

    void write_error_message(const ParserData& data,
                             const CommandData& cmd,
                             const std::string& msg,
                             ArgumentId argument_id)
    {
        if (const auto name = get_name(cmd, argument_id); !name.empty())
            write_error_message(data, cmd, name + ": " + msg);
        else
            write_error_message(data, cmd, msg);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    Option::Option()
        : m_option(std::make_unique<OptionData>())
    {}

    Option::Option(std::string flag)
        : m_option(std::make_unique<OptionData>())
    {
        m_option->flags.emplace_back(std::move(flag));
    }

    Option::Option(std::string flag1, std::string flag2)
        : m_option(std::make_unique<OptionData>())
    {
        m_option->flags.emplace_back(std::move(flag1));
        m_option->flags.emplace_back(std::move(flag2));
    }

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

    Option& Option::help(TextCallback callback)
    {
        check_option();
        m_option->help = callback;
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
        m_option->callback = std::move(callback);
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

    Option& Option::mandatory(bool mandatory)
    {
        return optional(!mandatory);
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
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-10-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        bool check_flag_with_equal(const std::string& flag,
                                   const OptionData& od)
        {
            const auto eq_pos = flag.find('=');
            if (eq_pos == std::string::npos)
                return true;
            if (eq_pos != flag.size() - 1)
                ARGOS_THROW("Options can not have a '=' in the middle: " + flag);
            if (od.argument.empty())
                ARGOS_THROW("Options ending with '=' must have a named argument: " + flag);
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
    }

    void validate_and_update(OptionData& option, OptionStyle style)
    {
        if (option.flags.empty())
            ARGOS_THROW("Option must have one or more flags.");

        for (auto& flag : option.flags)
        {
            bool ok = false;
            switch (style)
            {
            case OptionStyle::STANDARD:
                ok = check_standard_flag(flag, option);
                break;
            case OptionStyle::SLASH:
                ok = check_flag(flag, '/', option);
                break;
            case OptionStyle::DASH:
                ok = check_flag(flag, '-', option);
                break;
            default:
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!option.argument.empty() && !option.constant.empty())
            ARGOS_THROW("Option cannot have both argument and constant.");

        switch (option.operation)
        {
        case OptionOperation::NONE:
            if (!option.constant.empty())
                ARGOS_THROW("NONE-options cannot have a constant.");
            if (!option.alias.empty())
                ARGOS_THROW("NONE-options cannot have an alias.");
            break;
        case OptionOperation::ASSIGN:
            if (option.argument.empty() && option.constant.empty())
                option.constant = "1";
            break;
        case OptionOperation::APPEND:
            if (option.argument.empty() && option.constant.empty())
                ARGOS_THROW("Options that appends must have either constant or argument.");
            break;
        case OptionOperation::CLEAR:
            if (!option.argument.empty() || !option.constant.empty())
                option.constant = "1";
            if (!option.optional)
                ARGOS_THROW("CLEAR-options must be optional.");
            break;
        }
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    OptionIterator::OptionIterator()
        : m_args(m_all_args)
    {
    }

    OptionIterator::OptionIterator(std::vector<std::string_view> args, char prefix)
        : m_all_args(args.begin(), args.end()),
          m_args(m_all_args),
          m_prefix(prefix)
    {
    }

    OptionIterator::OptionIterator(const OptionIterator& rhs)
        : m_all_args(rhs.m_all_args),
          m_args(m_all_args.begin() + rhs.m_all_args.size() - rhs.m_args.size(), m_all_args.end()),
          m_pos(rhs.m_pos),
          m_prefix(rhs.m_prefix)
    {
    }

    std::optional<std::string> OptionIterator::next()
    {
        if (m_pos != 0)
        {
            m_pos = 0;
            pop_front(m_args);
        }

        if (m_args.empty())
            return {};

        if (m_args[0].size() <= 2 || m_args[0][0] != m_prefix)
        {
            m_pos = std::string_view::npos;
            return std::string(m_args[0]);
        }

        const auto eq = m_args[0].find('=');
        if (eq == std::string_view::npos)
        {
            m_pos = std::string_view::npos;
            return std::string(m_args[0]);
        }

        m_pos = eq + 1;
        return std::string(m_args[0].substr(0, m_pos));
    }

    std::optional<std::string> OptionIterator::next_value()
    {
        if (m_args.empty())
            return {};

        if (m_pos != std::string_view::npos)
        {
            const auto result = m_args[0].substr(m_pos);
            m_pos = std::string_view::npos;
            return std::string(result);
        }

        pop_front(m_args);
        if (m_args.empty())
        {
            m_pos = 0;
            return {};
        }

        m_pos = m_args[0].size();
        return std::string(m_args[0]);
    }

    std::string_view OptionIterator::current() const
    {
        if (m_args.empty())
            ARGOS_THROW("There is no current argument.");
        return m_args[0];
    }

    std::span<std::string> OptionIterator::remaining_arguments() const
    {
        return m_pos == 0 ? m_args : m_args.subspan(1);
    }

    void OptionIterator::insert(const std::vector<std::string>& args)
    {
        auto args_index = m_all_args.size() - m_args.size();
        auto insert_index = args_index;
        if (m_pos != 0)
            insert_index++;
        m_all_args.insert(m_all_args.begin() + insert_index,
                          args.begin(), args.end());
        m_args = std::span(m_all_args.begin() + args_index, m_all_args.end());
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the Zero-Clause BSD License.
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

    std::string OptionView::help() const
    {
        return get_text(m_option->help);
    }

    const std::string& OptionView::section() const
    {
        return m_option->section;
    }

    const std::string& OptionView::alias() const
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
// This file is distributed under the Zero-Clause BSD License.
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
        const auto n = parse_integer_impl<long>(str, base);
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
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_impl(std::move(impl))
    {}

    ParsedArguments::ParsedArguments(const ParsedArguments& rhs) = default;

    ParsedArguments::ParsedArguments(ParsedArguments&& rhs) noexcept = default;

    ParsedArguments::~ParsedArguments() = default;

    ParsedArguments& ParsedArguments::operator=(const ParsedArguments&) = default;

    ParsedArguments& ParsedArguments::operator=(ParsedArguments&& rhs) noexcept = default;

    CommandView ParsedArguments::command() const
    {
        return CommandView(m_impl->command());
    }

    std::string_view ParsedArguments::name() const
    {
        return m_impl->command()->name;
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_impl->has(m_impl->get_value_id(name));
    }

    bool ParsedArguments::has(const IArgumentView& arg) const
    {
        return m_impl->has(arg.value_id());
    }

    std::vector<ParsedArguments> ParsedArguments::subcommands() const
    {
        std::vector<ParsedArguments> result;
        for (const auto& subcommand : m_impl->subcommands())
            result.emplace_back(subcommand);
        return result;
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        if (auto value = m_impl->get_value(id))
            return {value->first, m_impl, id, value->second};
        else
            return {{}, m_impl, id, {}};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        if (auto value = m_impl->get_value(arg.value_id()))
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
        for (auto& a : m_impl->command()->arguments)
            result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        return result;
    }

    std::vector<std::unique_ptr<OptionView>>
    ParsedArguments::all_options() const
    {
        std::vector<std::unique_ptr<OptionView>> result;
        for (auto& o : m_impl->command()->options)
            result.emplace_back(std::make_unique<OptionView>(o.get()));
        return result;
    }

    std::vector<std::unique_ptr<CommandView>>
    ParsedArguments::all_subcommands() const
    {
        std::vector<std::unique_ptr<CommandView>> result;
        for (auto& c : m_impl->command()->commands)
            result.emplace_back(std::make_unique<CommandView>(c.get()));
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

    void ParsedArguments::filter_parsed_arguments(int& argc, char**& argv) const
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

    void ParsedArguments::error(const std::string& msg) const
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

    void print(const ParsedArguments& parsed_args, std::ostream& stream)
    {
        std::vector<const IArgumentView*> argViews;
        auto args = parsed_args.all_arguments();
        std::transform(args.begin(), args.end(), back_inserter(argViews),
                       [](auto& av) {return av.get();});
        auto opts = parsed_args.all_options();
        std::transform(opts.begin(), opts.end(), back_inserter(argViews),
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
            print_argument(stream, label, parsed_args.values(*arg));

        if (!parsed_args.unprocessed_arguments().empty())
        {
            stream << "Unprocessed arguments:";
            for (auto& arg : parsed_args.unprocessed_arguments())
                stream << " \"" << arg << "\"";
        }

        for (const auto& subcommand : parsed_args.subcommands())
        {
            stream << "\nSubcommand: " << subcommand.name() << "\n";
            print(subcommand, stream);
        }
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(
            std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_impl(std::move(impl))
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
        if (auto value = m_impl->get_value(id))
            return {value->first, m_impl, id, value->second};
        else
            return {{}, m_impl, id, {}};
    }

    ArgumentValue
    ParsedArgumentsBuilder::value(const IArgumentView& arg) const
    {
        if (auto value = m_impl->get_value(arg.value_id()))
            return {value->first, m_impl, arg.value_id(), arg.argument_id()};
        else
            return {{}, m_impl, arg.value_id(), arg.argument_id()};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const std::string& name) const
    {
        auto id = m_impl->get_value_id(name);
        auto values = m_impl->get_values(id);
        return {std::move(values), m_impl, id};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const IArgumentView& arg) const
    {
        auto values = m_impl->get_values(arg.value_id());
        return {std::move(values), m_impl, arg.value_id()};
    }

    bool ParsedArgumentsBuilder::has(const std::string& name) const
    {
        return m_impl->has(m_impl->get_value_id(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg) const
    {
        return m_impl->has(arg.value_id());
    }

    void ParsedArgumentsBuilder::error(const std::string& msg) const
    {
         m_impl->error(msg);
    }

    void ParsedArgumentsBuilder::error(const std::string& msg,
                                       const IArgumentView& arg) const
    {
        m_impl->error(msg, arg.argument_id());
    }

    std::ostream& ParsedArgumentsBuilder::stream() const
    {
        const auto custom_stream = m_impl->parser_data()->help_settings.output_stream;
        return custom_stream ? *custom_stream : std::cout;
    }

    const std::string& ParsedArgumentsBuilder::program_name() const
    {
      return m_impl->parser_data()->command.name;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <cassert>

namespace argos
{
    namespace
    {
        template <typename ItT, typename ValueT, typename IsLessT>
        ItT lower_bound(ItT begin, ItT end, ValueT&& v, IsLessT is_less)
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

    ParsedArgumentsImpl::ParsedArgumentsImpl(const CommandData* command,
                                             std::shared_ptr<ParserData> data)
        : m_command(command),
          m_data(std::move(data))
    {
        assert(m_data);
        for (auto& a : m_command->arguments)
        {
            m_ids.emplace_back(a->name, a->value_id, a->argument_id);
            if (!a->alias.empty())
                m_ids.emplace_back(a->alias, a->value_id, a->argument_id);
        }
        for (auto& o : m_command->options)
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
                              [](auto& p, auto& s) { return get<0>(p) < s; });
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

    const std::shared_ptr<ParsedArgumentsImpl>&
    ParsedArgumentsImpl::add_subcommand(const CommandData* command)
    {
        m_commands.push_back(std::make_shared<ParsedArgumentsImpl>(
            command, m_data));
        return m_commands.back();
    }

    const std::vector<std::shared_ptr<ParsedArgumentsImpl>>&
    ParsedArgumentsImpl::subcommands() const
    {
        return m_commands;
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ParsedArgumentsImpl::get_argument_views(ValueId value_id) const
    {
        std::vector<std::unique_ptr<IArgumentView>> result;
        for (auto& a : m_command->arguments)
        {
            if (a->value_id == value_id)
                result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        }
        for (auto& o : m_command->options)
        {
            if (o->value_id == value_id)
                result.emplace_back(std::make_unique<OptionView>(o.get()));
        }
        return result;
    }

    std::unique_ptr<IArgumentView>
    ParsedArgumentsImpl::get_argument_view(ArgumentId argument_id) const
    {
        for (auto& a : m_command->arguments)
        {
            if (a->argument_id == argument_id)
                return std::make_unique<ArgumentView>(a.get());
        }
        for (auto& o : m_command->options)
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

    const CommandData* ParsedArgumentsImpl::command() const
    {
        return m_command;
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

    void ParsedArgumentsImpl::error(const std::string& message) const
    {
        write_error_message(*m_data, *m_command, message);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }

    void ParsedArgumentsImpl::error(const std::string& message,
                                    ArgumentId argument_id)
    {
        write_error_message(*m_data, *m_command, message, argument_id);
        if (m_data->parser_settings.auto_exit)
            exit(m_data->parser_settings.error_exit_code);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }
}

//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-11.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        void add_version_option(ParserData& data)
        {
            if (data.version.empty())
                return;
            auto& cmd = data.command;
            std::string flag;
            switch (data.parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                if (!has_flag(cmd, "--version", data.parser_settings))
                    flag = "--version";
                break;
            case OptionStyle::SLASH:
                if (!has_flag(cmd, "/VERSION", data.parser_settings))
                    flag = "/VERSION";
                break;
            case OptionStyle::DASH:
                if (!has_flag(cmd, "-version", data.parser_settings))
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
                .callback([v = data.version, stream]
                (auto& a)
                    {
                        *stream << a.builder.program_name() << " " << v << "\n";
                        return true;
                    })
                .release();
            opt->section = cmd.current_section;
            cmd.options.push_back(std::move(opt));
        }
    }

    void finish_initialization(ParserData& data)
    {
        add_version_option(data);
        finish_initialization(data.command, data);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    StandardOptionIterator::StandardOptionIterator()
    {
    }

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_all_args(args.begin(), args.end()),
          m_args(m_all_args)
    {
    }

    StandardOptionIterator::StandardOptionIterator(const StandardOptionIterator& rhs)
        : m_all_args(rhs.m_all_args),
          m_args(m_all_args.begin() + rhs.m_all_args.size() - rhs.m_args.size(), m_all_args.end()),
          m_pos(rhs.m_pos)
    {
    }

    std::optional<std::string> StandardOptionIterator::next()
    {
        if (m_pos != 0)
        {
            // m_pos is always greater than one if we get here.
            if (m_pos < m_args[0].size() && m_args[0][1] != '-')
            {
                const auto c = m_args[0][m_pos++];
                if (m_pos == m_args[0].size())
                    m_pos = std::string::npos;
                return std::string{'-', c};
            }
            pop_front(m_args);
            m_pos = 0;
        }

        if (m_args.empty())
            return {};

        if (m_args[0].size() <= 2 || m_args[0][0] != '-')
        {
            m_pos = std::string::npos;
            return m_args[0];
        }

        if (m_args[0][1] != '-')
        {
            m_pos = 2;
            return m_args[0].substr(0, 2);
        }

        m_pos = m_args[0].find('=');

        if (m_pos == std::string::npos)
            return m_args[0];

        m_pos++;
        return m_args[0].substr(0, m_pos);
    }

    std::optional<std::string> StandardOptionIterator::next_value()
    {
        if (m_args.empty())
            return {};

        if (m_pos != std::string::npos)
        {
            const auto pos = m_pos;
            m_pos = std::string::npos;
            return m_args[0].substr(pos);
        }

        pop_front(m_args);
        if (m_args.empty())
        {
            m_pos = 0;
            return {};
        }

        return m_args[0];
    }

    std::string_view StandardOptionIterator::current() const
    {
        if (m_args.empty())
            ARGOS_THROW("There is no current argument.");
        return m_args[0];
    }

    std::span<std::string> StandardOptionIterator::remaining_arguments()
    {
        split_concatenated_flags();
        return m_pos == 0 ? m_args : m_args.subspan(1);
    }

    void StandardOptionIterator::insert(std::vector<std::string> args)
    {
        split_concatenated_flags();
        auto args_index = m_all_args.size() - m_args.size();
        auto insert_index = args_index;
        if (m_pos != 0)
            insert_index++;
        m_all_args.insert(m_all_args.begin() + insert_index,
                          args.begin(), args.end());
        m_args = std::span(m_all_args.begin() + args_index, m_all_args.end());
    }

    void StandardOptionIterator::split_concatenated_flags()
    {
        if (m_pos == 0 || m_pos >= m_args[0].size() || m_args[0][1] == '-')
            return;

        auto index = m_all_args.size() - m_args.size();
        m_all_args.insert(m_all_args.begin() + index + 1,
                          "-" + m_args[0].substr(m_pos));
        m_all_args[index].resize(m_pos);
        m_args = std::span(m_all_args.begin() + index, m_all_args.end());
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <cstdint>

namespace argos
{
    bool are_equal_chars_ci(char a, char b)
    {
        if (a == b)
            return true;
        const auto ua = uint8_t(a);
        const auto ub = uint8_t(b);
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
        const auto ic1 = int(uint8_t(c1) & 0xDFu);
        if (ic1 < 'A' || 'Z' < ic1)
            return c1 - c2;
        const auto ic2 = int(uint8_t(c2) & 0xDFu);
        return ic1 - ic2;
    }

    bool is_less_ci(std::string_view str1, std::string_view str2)
    {
        const auto size = std::min(str1.size(), str2.size());
        for (size_t i = 0; i < size; ++i)
        {
            if (const auto cmp = compare_ci(str1[i], str2[i]); cmp != 0)
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
            const auto next_pos = s.find(delimiter, pos);
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
        const auto pos = str.find_last_of("/\\");
        return pos == std::string_view::npos ? str : str.substr(pos + 1);
    }

    constexpr size_t get_code_point_length(char c) noexcept
    {
        const auto u = unsigned(static_cast<uint8_t>(c));
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
        for (const auto c : str)
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

    char to_lower(char c)
    {
        return ('A' <= c && c <= 'Z') ? char(c - 'A' + 'a') : c;
    }

    void to_lower(std::string& word)
    {
        for (auto& c: word)
            c = to_lower(c);
    }

    std::string to_lower(std::string_view word)
    {
        std::string result(word);
        to_lower(result);
        return result;
    }

    bool is_lower(std::string_view word)
    {
        return std::all_of(word.begin(), word.end(), [](char c)
        {
            return c < 'A' || 'Z' < c;
        });
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace argos
{
    namespace
    {
        std::pair<std::string_view, std::string_view>
        next_line(std::string_view text)
        {
            const auto pos = text.find_first_of("\n\r");
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
                if (const auto n = text.find_first_not_of(' '); n != std::string_view::npos)
                    return {' ', text.substr(0, n), text.substr(n)};
                else
                    return {' ', text, {}};
            default:
                if (const auto n = text.find_first_of("\t\r\n "); n != std::string_view::npos)
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
        : TextFormatter(&std::cout, 0)
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, 0)
    {}

    TextFormatter::TextFormatter(std::ostream* stream, unsigned line_width)
        : m_writer(line_width ? line_width : get_console_width(32))
    {
        if (m_writer.line_width() <= 2)
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
            const auto width = m_writer.remaining_width();
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
// This file is distributed under the Zero-Clause BSD License.
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
        if (indent >= m_line_width)
            return false;
        m_indent = indent;
        return true;
    }

    bool TextWriter::write(std::string_view str, bool force)
    {
        const auto width = current_width();
        const auto remaining = std::max(width, m_line_width) - width;
        const auto str_width = static_cast<unsigned>(count_code_points(str));
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
        m_stream->write(m_line.data(), std::streamsize(m_line.size()));
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
// This file is distributed under the Zero-Clause BSD License.
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

        std::string_view remove_punctuation(std::string_view word)
        {
            auto last = word.find_last_not_of(".,;:!?()[]{}<>\"'`");
            return word.substr(0, last + 1);
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
            const auto sep = word_rule[pos - 1] == '-' ? '\0' : '-';
            splits.push_back({unsigned(pos - offset), sep});
            ++offset;
        }
        splits.push_back({unsigned(word_rule.size() - offset), '\0'});
        word_rule.erase(remove(word_rule.begin(), word_rule.end(), ' '),
                        word_rule.end());
        to_lower(word_rule);
        m_strings.push_back(std::move(word_rule));
        m_splits.insert({std::string_view(m_strings.back()), std::move(splits)});
    }

    void WordSplitter::add_words(std::vector<std::string> word_rules)
    {
        for (auto& word_rule: word_rules)
            add_word(std::move(word_rule));
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t start_index,
                        size_t max_length, bool must_split) const
    {
        auto key = remove_punctuation(word);
        const auto it = is_lower(key)
                            ? m_splits.find(key)
                            : m_splits.find(to_lower(key));
        if (it != m_splits.end())
        {
            Split prev = {unsigned(start_index), '\0'};
            size_t length = 0;
            for (const auto split: it->second)
            {
                if (split.index < start_index + 1)
                    continue;
                length += count_code_points(word.substr(prev.index, split.index - prev.index));
                if (length + (split.separator ? 1 : 0) > max_length)
                    break;
                prev = split;
            }
            if (prev.index > start_index + 1)
                return {
                    word.substr(start_index, prev.index - start_index),
                    prev.separator,
                    word.substr(prev.index)
                };
        }
        if (must_split)
            return default_rule(word.substr(start_index), max_length);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::default_rule(std::string_view word, size_t max_length)
    {
        if (max_length <= 2)
            return {{}, '\0', word};
        auto max_pos = find_nth_code_point(word, max_length);
        if (max_pos == std::string_view::npos)
            return {word, '\0', {}};
        const auto ignore_utf8 = max_pos == max_length;
        --max_pos;
        if (!ignore_utf8)
        {
            while (is_utf8_continuation(word[max_pos]))
                --max_pos;
        }

        const auto min_pos = (max_length + 2) / 3;
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
        return {
            word.substr(0, max_pos),
            '-',
            word.substr(max_pos)
        };
    }
}
