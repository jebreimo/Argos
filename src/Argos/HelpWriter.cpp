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
            if (!opt.mandatory)
                optTxt.push_back('[');
            optTxt += *flag;
            if (!opt.argument.empty())
            {
                optTxt += " <";
                optTxt += opt.argument;
                optTxt.push_back('>');
            }
            if (!opt.mandatory)
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

    HelpWriter::HelpWriter(std::shared_ptr<ParserData> data)
        : m_Data(move(data))
    {
        m_Data->textFormatter.setLineWidth(getConsoleWidth(20, 80));
    }

    void HelpWriter::writeHelpText() const
    {
        writeUsage();
        writeArgumentSections();
        writeEndText();
    }

    void HelpWriter::writeErrorMessage(const std::string& msg) const
    {
        m_Data->textFormatter.writeText(m_Data->helpSettings.programName + ":");
        m_Data->textFormatter.writeText(msg);
        m_Data->textFormatter.newline();
        writeBriefUsage();
    }

    void HelpWriter::writeErrorMessage(const ArgumentData& argument,
                                       const std::string& msg) const
    {
        writeErrorMessage(msg);
    }

    void HelpWriter::writeErrorMessage(const OptionData& option,
                                       const std::string& msg) const
    {
        writeErrorMessage(msg);
    }

    void HelpWriter::writeUsage() const
    {
        auto usage = getCustomText(TextId::USAGE);
        if (!usage)
            writeBriefUsage();
        else if (!usage->empty())
            m_Data->textFormatter.writePreformattedText(*usage);
    }

    void HelpWriter::writeBriefUsage() const
    {
        auto title = getCustomText(TextId::USAGE_TITLE);
        m_Data->textFormatter.writeText(title ? *title : "USAGE\n");
        m_Data->textFormatter.pushIndentation(2);
        m_Data->textFormatter.writeText(m_Data->helpSettings.programName);
        m_Data->textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
        for (auto& opt : m_Data->options)
        {
            if (opt->hidden)
                continue;

            m_Data->textFormatter.writePreformattedText(getBriefOptionName(*opt));
        }
        for (auto& arg : m_Data->arguments)
        {
            if (arg->hidden)
                continue;
            m_Data->textFormatter.writePreformattedText(getArgumentName(*arg));
        }
        m_Data->textFormatter.popIndentation();
        m_Data->textFormatter.newline();
        m_Data->textFormatter.popIndentation();
    }

    std::string HelpWriter::generateUsage() const
    {
        return {};
    }

    void HelpWriter::writeArgumentSections() const
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

        auto argSection = getCustomText(TextId::ARGUMENTS_SECTION);
        if (!argSection)
            argSection = "ARGUMENTS";
        for (auto& arg : m_Data->arguments)
        {
            if (arg->hidden)
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
        auto optSection = getCustomText(TextId::OPTIONS_SECTION);
        if (!optSection)
            optSection = "OPTIONS";
        for (auto& opt : m_Data->options)
        {
            if (opt->hidden)
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
        if (nameWidth + textWidths.back() > m_Data->textFormatter.lineWidth())
        {
            auto index75 = 3 * nameWidths.size() / 4;
            nameWidth = nameWidths[index75] + 3;
            if (nameWidth + textWidths[index75] > m_Data->textFormatter.lineWidth())
                nameWidth = m_Data->textFormatter.lineWidth() / 4;
        }
        for (auto& [section, txts] : sections)
        {
            m_Data->textFormatter.writeText(section);
            m_Data->textFormatter.newline();
            m_Data->textFormatter.pushIndentation(2);
            for (auto& [name, text] : txts)
            {
                m_Data->textFormatter.writeText(name);
                m_Data->textFormatter.pushIndentation(nameWidth);
                m_Data->textFormatter.writeText(text);
                m_Data->textFormatter.popIndentation();
                m_Data->textFormatter.newline();
            }
            m_Data->textFormatter.popIndentation();
        }
    }

    void HelpWriter::writeEndText() const
    {

    }

    std::optional<std::string> HelpWriter::getCustomText(TextId textId) const
    {
        auto it = m_Data->helpSettings.texts.find(textId);
        if (it != m_Data->helpSettings.texts.end())
            return it->second;
        return {};
    }
}
