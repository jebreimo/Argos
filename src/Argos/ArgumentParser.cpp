//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentParser.hpp"

#include <algorithm>
#include <iostream>
#include "ArgosThrow.hpp"
#include "ArgumentIteratorImpl.hpp"
#include "HelpText.hpp"
#include "ParserData.hpp"
#include "StringUtilities.hpp"

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
