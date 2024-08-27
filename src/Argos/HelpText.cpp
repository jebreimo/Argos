//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "HelpText.hpp"

#include <algorithm>
#include <iostream>

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
            const bool braces = opt.optional
                                && !is_stop_option(opt.type);
            if (braces)
                opt_txt.push_back('[');

            std::string_view flag;
            if (prefer_long_flag)
            {
                const auto it = std::find_if(opt.flags.begin(),  opt.flags.end(),
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
            const auto it = data.help_settings.texts.find(text_id);
            if (it != data.help_settings.texts.end())
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

        using HelpText = std::pair<std::string, std::string>;
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
            const auto line_width = data.text_formatter.line_width();
            // Check if both the longest name and the longest help text
            // can fit on the same line.
            const auto name_width = name_widths.back() + 3;
            if (name_width > 32 || name_width + text_widths.back() > line_width)
                return 0;
            return name_width;
        }

        void write_argument_sections(ParserData& data, bool prepend_newline)
        {
            std::vector<SectionHelpTexts> sections;

            auto add_help_text = [&](std::string_view s, std::string a, std::string b)
            {
                auto it = find_if(sections.begin(), sections.end(),
                                  [&](const auto& v)
                                  {return v.first == s;});
                if (it == sections.end())
                {
                    sections.push_back({s, {}});
                    it = std::prev(sections.end());
                }
                it->second.emplace_back(std::move(a), std::move(b));
            };

            auto arg_title = get_custom_text(data, TextId::ARGUMENTS_TITLE);
            if (!arg_title)
                arg_title = "ARGUMENTS";
            for (auto& a : data.arguments)
            {
                if ((a->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = a->section.empty() ? *arg_title : a->section;
                add_help_text(section, get_argument_name(*a), get_text(a->help));
            }
            auto opt_title = get_custom_text(data, TextId::OPTIONS_TITLE);
            if (!opt_title)
                opt_title = "OPTIONS";
            for (auto& o : data.options)
            {
                if ((o->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = o->section.empty() ? *opt_title : o->section;
                add_help_text(section, get_long_option_name(*o), get_text(o->help));
            }

            if (sections.empty())
                return;
            const unsigned name_width = get_help_text_label_width(data, sections);

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
            if (const auto t = get_custom_text(data, TextId::USAGE); t && t->empty())
                return false;

            const auto text1 = write_custom_text(data, TextId::USAGE_TITLE,
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
            const auto text2 = write_custom_text(data, TextId::USAGE,
                                                 prepend_newline);
            if (text2)
                return !is_empty(text1) || !is_empty(text2);
            write_brief_usage(data, prepend_newline);
            return true;
        }

        std::string get_name(const ParserData& data, ArgumentId argument_id)
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
        if (const auto name = get_name(data, argument_id); !name.empty())
            write_error_message(data, name + ": " + msg);
        else
            write_error_message(data, msg);
    }
}
