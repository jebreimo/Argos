//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "HelpText.hpp"

#include <algorithm>

namespace Argos
{
    namespace
    {
        std::string getArgumentName(const ArgumentData& arg)
        {
            if (arg.name[0] == '<' || arg.name[0] == '[')
                return arg.name;
            else if (arg.minCount == 0)
                return "[<" + arg.name + ">]";
            else
                return "<" + arg.name + ">";
        }

        std::string getBriefOptionName(const OptionData& opt)
        {
            std::string optTxt;
            bool braces = !opt.mandatory
                          && opt.type != OptionType::STOP
                          && opt.type != OptionType::HELP;
            if (braces)
                optTxt.push_back('[');
            auto& flag = opt.flags.front();
            optTxt += flag;
            if (!opt.argument.empty())
            {
                if (flag.back() != '=')
                    optTxt += " ";
                optTxt += "<";
                optTxt += opt.argument;
                optTxt.push_back('>');
            }
            if (braces)
                optTxt.push_back(']');
            return optTxt;
        }

        std::string getLongOptionName(const OptionData& opt)
        {
            std::string optTxt;
            for (auto& flag : opt.flags)
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

        bool writeCustomText(ParserData& data, TextId textId)
        {
            auto text = getCustomText(data, textId);
            if (!text)
                return false;
            if (!text->empty())
            {
                data.textFormatter.writeText(*text);
                data.textFormatter.newline();
            }
            return true;
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
            for (auto& entry : sections)
            {
                for (auto& [name, txt] : entry.second)
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
                // Check if 75% of the names and help texts can fit on
                // the same line.
                auto index75 = 3 * nameWidths.size() / 4;
                nameWidth = nameWidths[index75] + 3;
                if (nameWidth + textWidths[index75] > lineWidth)
                    nameWidth = lineWidth / 5;
            }
            return nameWidth;
        }

        void writeArgumentSections(ParserData& data)
        {
            std::vector<SectionHelpTexts> sections;

            auto addHelpText = [&](std::string_view s, std::string a, std::string_view b)
            {
                auto it = find_if(sections.begin(), sections.end(),
                                  [&](auto& v) {return v.first == s;});
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

            for (auto&[section, txts] : sections)
            {
                data.textFormatter.writeText(section);
                data.textFormatter.newline();
                data.textFormatter.pushIndentation(2);
                for (auto& [name, text] : txts)
                {
                    data.textFormatter.writeText(name);
                    if (data.textFormatter.currentLineWidth() >= nameWidth)
                        data.textFormatter.writeText(" ");
                    data.textFormatter.pushIndentation(nameWidth);
                    data.textFormatter.writeText(text);
                    data.textFormatter.popIndentation();
                    data.textFormatter.newline();
                }
                data.textFormatter.popIndentation();
            }
        }

        void writeBriefUsage(ParserData& data)
        {
            data.textFormatter.pushIndentation(2);

            writeStopAndHelpUsage(data);

            data.textFormatter.writeText(data.helpSettings.programName);
            data.textFormatter.writeText(" ");
            data.textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (opt->type == OptionType::HELP
                    || opt->type == OptionType::STOP)
                    continue;

                data.textFormatter.writePreformattedText(getBriefOptionName(*opt));
                data.textFormatter.writeText(" ");
            }
            for (auto& arg : data.arguments)
            {
                if ((arg->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                data.textFormatter.writePreformattedText(getArgumentName(*arg));
                data.textFormatter.writeText(" ");
            }
            data.textFormatter.popIndentation();
            data.textFormatter.newline();
            data.textFormatter.popIndentation();
        }

        void writeUsage(ParserData& data)
        {
            if (!writeCustomText(data, TextId::USAGE_TITLE))
            {
                data.textFormatter.writeText("USAGE");
                data.textFormatter.newline();
            }
            if (!writeCustomText(data, TextId::USAGE))
                writeBriefUsage(data);
        }

        std::string getName(ParserData& data, ArgumentId argumentId)
        {
            for (auto& a : data.arguments)
            {
                if (a->argumentId == argumentId)
                    return a->name;
            }
            for (auto& o : data.options)
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
        writeCustomText(data, TextId::INITIAL_TEXT);
        writeUsage(data);
        writeCustomText(data, TextId::TEXT);
        writeArgumentSections(data);
        writeCustomText(data, TextId::FINAL_TEXT);
    }

    void writeErrorMessage(ParserData& data, const std::string& msg)
    {
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
