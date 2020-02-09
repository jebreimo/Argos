//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "HelpWriter.hpp"

#include <iostream>

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
    {}

    void HelpWriter::writeHelpText() const
    {
        writeUsage();
        writeArgumentSections();
        writeEndText();
    }

    void HelpWriter::writeErrorMessage(const std::string& msg) const
    {
        m_Data->textFormatter.writeText(msg);
        m_Data->textFormatter.newline();
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
        writeBriefUsage();
    }

    void HelpWriter::writeBriefUsage() const
    {
        auto title = getCustomText(TextId::USAGE_TITLE);
        m_Data->textFormatter.writeText(title ? *title : "USAGE\n");
        m_Data->textFormatter.pushIndentation(2);
        m_Data->textFormatter.writeText(m_Data->helpSettings.programName);
        m_Data->textFormatter.writeFormattedText(" ", false, false);
        m_Data->textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
        for (auto& opt : m_Data->options)
        {
            if (opt->hidden)
                continue;

            m_Data->textFormatter.writeFormattedText(getBriefOptionName(*opt), true, true);
        }
        for (auto& arg : m_Data->arguments)
        {
            if (arg->hidden)
                continue;
            m_Data->textFormatter.writeFormattedText(getArgumentName(*arg), true, true);
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
        std::multimap<std::string_view, std::pair<std::string, std::string_view>> sections;
        auto argSection = getCustomText(TextId::ARGUMENTS_SECTION);
        if (!argSection)
            argSection = "ARGUMENTS";
        for (auto& arg : m_Data->arguments)
        {
            if (arg->hidden)
                continue;
            auto& section = arg->section.empty() ? *argSection : arg->section;
            sections.insert({section, {getArgumentName(*arg), arg->text}});
        }
        auto optSection = getCustomText(TextId::OPTIONS_SECTION);
        if (!optSection)
            optSection = "OPTIONS";
        for (auto& opt : m_Data->options)
        {
            if (opt->hidden)
                continue;
            auto& section = opt->section.empty() ? *optSection : opt->section;
            sections.insert({section, {getLongOptionName(*opt), opt->text}});
        }

        std::string_view section;
        m_Data->textFormatter.pushIndentation(2);
        for (auto& [sec, txts] : sections)
        {
            if (sec != section)
            {
                section = sec;
                m_Data->textFormatter.popIndentation();
                m_Data->textFormatter.writeText(section);
                m_Data->textFormatter.newline();
                m_Data->textFormatter.pushIndentation(2);
            }
            m_Data->textFormatter.writeText(txts.first);
            m_Data->textFormatter.writeFormattedText(" ", false, false);
            m_Data->textFormatter.writeText(txts.second);
            m_Data->textFormatter.newline();
        }
        m_Data->textFormatter.popIndentation();
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
