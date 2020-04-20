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

namespace Argos
{
    namespace
    {
        std::string getArgumentName(const ArgumentData& arg)
        {
            if (arg.name[0] == '<' || arg.name[0] == '[')
                return arg.name;

            std::string result;
            for (int i = 0; i < arg.minCount; ++i)
            {
                if (!result.empty())
                    result += " ";
                result += "<" + arg.name + ">";
            }

            if (arg.maxCount == arg.minCount)
                return result;

            if (!result.empty())
                result += " ";
            if (arg.maxCount - arg.minCount == 1)
                result += "[<" + arg.name + ">]";
            else
                result += "[<" + arg.name + ">]...";
            return result;
        }

        std::string getBriefOptionName(const OptionData& opt)
        {
            std::string optTxt;
            bool braces = opt.optional
                          && opt.type != OptionType::STOP
                          && opt.type != OptionType::HELP;
            if (braces)
                optTxt.push_back('[');
            const auto& flag = opt.flags.front();
            optTxt += flag;
            if (!opt.argument.empty())
            {
                if (flag.back() != '=')
                    optTxt += " ";
                if (opt.argument.front() != '<')
                {
                    optTxt += "<";
                    optTxt += opt.argument;
                    optTxt.push_back('>');
                }
                else
                {
                    optTxt += opt.argument;
                }
            }
            if (braces)
                optTxt.push_back(']');
            return optTxt;
        }

        std::string getLongOptionName(const OptionData& opt)
        {
            std::string optTxt;
            for (const auto& flag : opt.flags)
            {
                if (!optTxt.empty())
                    optTxt.append(", ");
                optTxt += flag;
                if (!opt.argument.empty())
                {
                    if (flag.back() != '=')
                        optTxt.push_back(' ');
                    optTxt.push_back('<');
                    optTxt += opt.argument;
                    optTxt.push_back('>');
                }
            }
            return optTxt;
        }

        std::optional<std::string>
        getCustomText(ParserData& data, TextId textId)
        {
            auto it = data.helpSettings.texts.find(textId);
            if (it != data.helpSettings.texts.end())
                return it->second;
            return {};
        }

        bool isEmpty(const std::optional<std::string>& str)
        {
            return !str || str->empty();
        }

        std::optional<std::string>
        writeCustomText(ParserData& data, TextId textId,
                        bool prependNewline = false)
        {
            auto text = getCustomText(data, textId);
            if (!isEmpty(text))
            {
                if (prependNewline)
                    data.textFormatter.newline();
                data.textFormatter.writeText(*text);
                if (!data.textFormatter.isCurrentLineEmpty())
                    data.textFormatter.newline();
            }
            return text;
        }

        void writeStopAndHelpUsage(ParserData& data)
        {
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (opt->type != OptionType::HELP
                    && opt->type != OptionType::STOP)
                    continue;

                data.textFormatter.writeText(data.helpSettings.programName);
                data.textFormatter.writeText(" ");
                data.textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
                data.textFormatter.writePreformattedText(getBriefOptionName(*opt));
                data.textFormatter.writeText(" ");
                data.textFormatter.popIndentation();
                data.textFormatter.newline();
            }
        }

        using HelpText = std::pair<std::string, std::string_view>;
        using HelpTextVector = std::vector<HelpText>;
        using SectionHelpTexts = std::pair<std::string_view, HelpTextVector>;

        unsigned int getHelpTextLabelWidth(
            const ParserData& data,
            const std::vector<SectionHelpTexts>& sections)
        {
            // Determine what width should be reserved for the argument names
            // and option flags.
            std::vector<unsigned> nameWidths;
            std::vector<unsigned> textWidths;
            for (const auto& entry : sections)
            {
                for (const auto& [name, txt] : entry.second)
                {
                    nameWidths.push_back(static_cast<unsigned>(name.size()));
                    textWidths.push_back(static_cast<unsigned>(txt.size()));
                }
            }

            std::sort(nameWidths.begin(), nameWidths.end());
            std::sort(textWidths.begin(), textWidths.end());
            auto lineWidth = data.textFormatter.lineWidth();
            // Check if both the longest name and the longest help text
            // can fit on the same line.
            auto nameWidth = nameWidths.back() + 3;
            if (nameWidth + textWidths.back() > lineWidth)
            {
                // Check if 80% of the names and help texts can fit on
                // the same line.
                auto index80 = 4 * nameWidths.size() / 5;
                nameWidth = nameWidths[index80] + 3;
                if (nameWidth + textWidths[index80] > lineWidth)
                    return 0;
            }
            return nameWidth;
        }

        void writeArgumentSections(ParserData& data, bool prependNewline)
        {
            std::vector<SectionHelpTexts> sections;

            auto addHelpText = [&](std::string_view s, std::string a, std::string_view b)
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

            auto argTitle = getCustomText(data, TextId::ARGUMENTS_TITLE);
            if (!argTitle)
                argTitle = "ARGUMENTS";
            for (auto& a : data.arguments)
            {
                if ((a->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = a->section.empty() ? *argTitle : a->section;
                addHelpText(section, getArgumentName(*a), a->text);
            }
            auto optTitle = getCustomText(data, TextId::OPTIONS_TITLE);
            if (!optTitle)
                optTitle = "OPTIONS";
            for (auto& o : data.options)
            {
                if ((o->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = o->section.empty() ? *optTitle : o->section;
                addHelpText(section, getLongOptionName(*o), o->text);
            }

            if (sections.empty())
                return;
            unsigned int nameWidth = getHelpTextLabelWidth(data, sections);

            auto& formatter = data.textFormatter;
            for (auto&[section, txts] : sections)
            {
                if (prependNewline)
                    formatter.newline();
                formatter.writeText(section);
                formatter.newline();
                formatter.pushIndentation(2);
                for (auto& [name, text] : txts)
                {
                    formatter.writeText(name);
                    if (nameWidth)
                    {
                        if (formatter.currentLineWidth() >= nameWidth)
                            formatter.writeText("  ");
                        formatter.pushIndentation(nameWidth);
                    }
                    else
                    {
                        formatter.newline();
                        formatter.pushIndentation(8);
                    }
                    formatter.writeText(text);
                    formatter.popIndentation();
                    formatter.newline();
                }
                formatter.popIndentation();
                prependNewline = true;
            }
        }

        void writeBriefUsage(ParserData& data, bool prependNewline)
        {
            auto& formatter = data.textFormatter;
            if (prependNewline)
                formatter.newline();

            formatter.pushIndentation(2);
            writeStopAndHelpUsage(data);
            formatter.writeText(data.helpSettings.programName);
            formatter.writeText(" ");
            formatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (opt->type == OptionType::HELP
                    || opt->type == OptionType::STOP)
                    continue;

                formatter.writePreformattedText(getBriefOptionName(*opt));
                formatter.writeText(" ");
            }
            for (auto& arg : data.arguments)
            {
                if ((arg->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                formatter.writePreformattedText(getArgumentName(*arg));
                formatter.writeText(" ");
            }
            formatter.popIndentation();
            formatter.newline();
            formatter.popIndentation();
        }

        bool writeUsage(ParserData& data, bool prependNewline = false)
        {
            if (auto t = getCustomText(data, TextId::USAGE); t && t->empty())
                return false;

            auto text1 = writeCustomText(data, TextId::USAGE_TITLE,
                                         prependNewline);
            if (!text1)
            {
                if (prependNewline)
                    data.textFormatter.newline();
                data.textFormatter.writeText("USAGE");
                data.textFormatter.newline();
                prependNewline = false;
            }
            else
            {
                prependNewline = prependNewline && isEmpty(text1);
            }
            auto text2 = writeCustomText(data, TextId::USAGE,
                                         prependNewline);
            if (text2)
                return !isEmpty(text1) || !isEmpty(text2);
            writeBriefUsage(data, prependNewline);
            return true;
        }

        std::string getName(ParserData& data, ArgumentId argumentId)
        {
            for (const auto& a : data.arguments)
            {
                if (a->argumentId == argumentId)
                    return a->name;
            }
            for (const auto& o : data.options)
            {
                if (o->argumentId == argumentId)
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

    void writeHelpText(ParserData& data)
    {
        if (data.helpSettings.outputStream)
            data.textFormatter.setStream(data.helpSettings.outputStream);
        bool newline = !isEmpty(writeCustomText(data, TextId::INITIAL_TEXT));
        newline = writeUsage(data, newline) || newline;
        newline = !isEmpty(writeCustomText(data, TextId::TEXT, newline)) || newline;
        writeArgumentSections(data, newline);
        writeCustomText(data, TextId::FINAL_TEXT, true);
    }

    void writeErrorMessage(ParserData& data, const std::string& msg)
    {
        if (data.helpSettings.outputStream)
            data.textFormatter.setStream(data.helpSettings.outputStream);
        else
            data.textFormatter.setStream(&std::cerr);
        data.textFormatter.writeText(data.helpSettings.programName + ": ");
        data.textFormatter.writeText(msg);
        data.textFormatter.newline();
        if (!writeCustomText(data, TextId::ERROR_USAGE))
            writeUsage(data);
    }

    void writeErrorMessage(ParserData& data, const std::string& msg,
                           ArgumentId argumentId)
    {
        if (auto name = getName(data, argumentId); !name.empty())
            writeErrorMessage(data, name + ": " + msg);
        else
            writeErrorMessage(data, msg);
    }
}
