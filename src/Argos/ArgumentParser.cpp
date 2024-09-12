//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentParser.hpp"

#include <algorithm>
#include <cstring>
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
            const auto eq_pos = flag.find('=');
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

    ArgumentParser& ArgumentParser::add(Argument argument)
    {
        check_data();
        auto ad = argument.release();
        if (ad->name.empty())
            ARGOS_THROW("Argument must have a name.");
        ad->argument_id = next_argument_id();
        if (ad->section.empty())
            ad->section = m_data->command.current_section;
        m_data->command.arguments.emplace_back(std::move(ad));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option option)
    {
        check_data();

        auto od = option.release();
        if (!od)
            ARGOS_THROW("Option is empty (it has probably already been added).");
        update_and_validate_option(*od);
        od->argument_id = next_argument_id();
        if (od->section.empty())
            od->section = m_data->command.current_section;
        m_data->command.options.push_back(std::move(od));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Command command)
    {
        check_data();

        auto cmd = command.release();
        if (!cmd)
            ARGOS_THROW("Command is empty (it has probably already been added).");
        if (cmd->section.empty())
            cmd->section = m_data->command.current_section;
        m_data->command.commands.push_back(std::move(cmd));
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

    std::optional<bool> ArgumentParser::require_command() const
    {
        check_data();
        return m_data->command.require_command;
    }

    ArgumentParser& ArgumentParser::require_command(bool value)
    {
        check_data();
        m_data->command.require_command = value;
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

    ArgumentParser& ArgumentParser::section(const std::string& name)
    {
        check_data();
        m_data->command.current_section = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::line_width(unsigned int line_width)
    {
        check_data();
        m_data->help_settings.line_width = line_width;
        return *this;
    }

    void ArgumentParser::write_help_text() const
    {
        check_data();
        const auto data = make_copy(*m_data);
        finish_initialization(*data);
        argos::write_help_text(*data, data->command);
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

    ArgumentId ArgumentParser::next_argument_id() const
    {
        const auto& cmd = m_data->command;
        return ArgumentId(cmd.options.size() + cmd.arguments.size() + 1);
    }

    void ArgumentParser::update_and_validate_option(OptionData& od)
    {
        if (od.flags.empty())
            ARGOS_THROW("Option must have one or more flags.");

        for (auto& flag : od.flags)
        {
            bool ok = false;
            switch (m_data->parser_settings.option_style)
            {
            case OptionStyle::STANDARD:
                ok = check_standard_flag(flag, od);
                break;
            case OptionStyle::SLASH:
                ok = check_flag(flag, '/', od);
                break;
            case OptionStyle::DASH:
                ok = check_flag(flag, '-', od);
                break;
            default:
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!od.argument.empty() && !od.constant.empty())
            ARGOS_THROW("Option cannot have both argument and constant.");

        switch (od.operation)
        {
        case OptionOperation::NONE:
            if (!od.constant.empty())
                ARGOS_THROW("NONE-options cannot have a constant.");
            if (!od.alias.empty())
                ARGOS_THROW("NONE-options cannot have an alias.");
            break;
        case OptionOperation::ASSIGN:
            if (od.argument.empty() && od.constant.empty())
                od.constant = "1";
            break;
        case OptionOperation::APPEND:
            if (od.argument.empty() && od.constant.empty())
                ARGOS_THROW("Options that appends must have either constant or argument.");
            break;
        case OptionOperation::CLEAR:
            if (!od.argument.empty() || !od.constant.empty())
                od.constant = "1";
            if (!od.optional)
                ARGOS_THROW("CLEAR-options must be optional.");
            break;
        }
    }
}
