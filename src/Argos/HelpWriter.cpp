//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "HelpWriter.hpp"

#include "ConsoleWidth.hpp"

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
            auto flag = min_element(opt.flags.begin(), opt.flags.end(),
                                    [](auto& a, auto& b)
                                    {return a.size() < b.size();});
            std::string optTxt;
            bool braces = !opt.mandatory
                          && opt.type != OptionType::STOP
                          && opt.type != OptionType::HELP;
            if (braces)
                optTxt.push_back('[');
            optTxt += *flag;
            if (!opt.argument.empty())
            {
                if (flag->back() != '=')
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
    }

    HelpWriter::HelpWriter() = default;

    void HelpWriter::writeHelpText(ParserData& data) const
    {
        writeUsage(data);
        writeArgumentSections(data);
        writeEndText(data);
    }

    void HelpWriter::writeErrorMessage(ParserData& data,
                                       const std::string& msg) const
    {
        data.textFormatter.writeText(data.helpSettings.programName + ":");
        data.textFormatter.writeText(msg);
        data.textFormatter.newline();
        writeBriefUsage(data);
    }

    void HelpWriter::writeUsage(ParserData& data) const
    {
        auto usage = getCustomText(data, TextId::USAGE);
        if (!usage)
            writeBriefUsage(data);
        else if (!usage->empty())
            data.textFormatter.writePreformattedText(*usage);
    }

    void HelpWriter::writeBriefUsage(ParserData& data) const
    {
        auto title = getCustomText(data, TextId::USAGE_TITLE);
        data.textFormatter.writeText(title ? *title : "USAGE\n");
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

    void HelpWriter::writeStopAndHelpUsage(ParserData& data) const
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

    void HelpWriter::writeArgumentSections(ParserData& data) const
    {
        using HelpText = std::pair<std::string, std::string_view>;
        using HelpTextVector = std::vector<HelpText>;
        using SectionHelpTexts = std::pair<std::string_view, HelpTextVector>;
        std::vector<SectionHelpTexts> sections;
        auto addHelpText = [&](std::string_view s, std::string a, std::string_view b)
        {
            auto it = find_if(sections.begin(), sections.end(),
                              [&](auto& t) {return t.first == s;});
            if (it == sections.end())
            {
                sections.push_back({s, {}});
                it = std::prev(sections.end());
            }
            it->second.emplace_back(std::move(a), b);
        };

        auto argSection = getCustomText(data, TextId::ARGUMENTS_SECTION);
        if (!argSection)
            argSection = "ARGUMENTS";
        for (auto& arg : data.arguments)
        {
            if ((arg->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                continue;
            auto& section = arg->section.empty() ? *argSection : arg->section;
            addHelpText(section, getArgumentName(*arg), arg->text);
            //auto it = find_if(sections.begin(), sections.end(),
            //                  [&](auto& t){return t.first == section;});
            //if (it == sections.end())
            //{
            //    sections.push_back({section, {}});
            //    it = std::prev(sections.end());
            //}
            //it->second.emplace_back(getArgumentName(*arg), arg->text);
        }
        auto optSection = getCustomText(data, TextId::OPTIONS_SECTION);
        if (!optSection)
            optSection = "OPTIONS";
        for (auto& opt : data.options)
        {
            if ((opt->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                continue;
            auto& section = opt->section.empty() ? *optSection : opt->section;
            addHelpText(section, getLongOptionName(*opt), opt->text);
        }

        if (sections.empty())
            return;

        std::vector<size_t> nameWidths;
        std::vector<size_t> textWidths;
        for (auto&[sec, txts] : sections)
        {
            for (auto&[name, txt] : txts)
            {
                nameWidths.push_back(name.size());
                textWidths.push_back(txt.size());
            }
        }

        std::sort(nameWidths.begin(), nameWidths.end());
        std::sort(textWidths.begin(), textWidths.end());
        auto nameWidth = nameWidths.back() + 3;
        if (nameWidth + textWidths.back() > data.textFormatter.lineWidth())
        {
            auto index75 = 3 * nameWidths.size() / 4;
            nameWidth = nameWidths[index75] + 3;
            if (nameWidth + textWidths[index75] > data.textFormatter.lineWidth())
                nameWidth = data.textFormatter.lineWidth() / 4;
        }
        for (auto& [section, txts] : sections)
        {
            data.textFormatter.writeText(section);
            data.textFormatter.newline();
            data.textFormatter.pushIndentation(2);
            for (auto& [name, text] : txts)
            {
                data.textFormatter.writeText(name);
                data.textFormatter.pushIndentation(nameWidth);
                data.textFormatter.writeText(text);
                data.textFormatter.popIndentation();
                data.textFormatter.newline();
            }
            data.textFormatter.popIndentation();
        }
    }

    void HelpWriter::writeEndText(ParserData& data) const
    {
    }

    std::optional<std::string>
    HelpWriter::getCustomText(ParserData& data, TextId textId) const
    {
        auto it = data.helpSettings.texts.find(textId);
        if (it != data.helpSettings.texts.end())
            return it->second;
        return {};
    }
}
