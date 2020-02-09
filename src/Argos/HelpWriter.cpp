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
        for (auto& opt : m_Data->options)
        {
            if (opt->hidden)
                continue;

            auto flag = min_element(opt->flags.begin(), opt->flags.end(),
                                    [](auto& a, auto& b)
                                    {return a.size() < b.size();});
            auto optTxt = "[" + *flag;
            if (!opt->argument.empty())
            {
                optTxt += " <";
                optTxt += opt->argument;
                optTxt.push_back('>');
            }
            optTxt.push_back(']');
            m_Data->textFormatter.writeFormattedText(optTxt, true, true);
        }
        for (auto& arg : m_Data->arguments)
        {
            if (arg->hidden)
                continue;
            if (arg->name[0] == '<' || arg->name[0] == '[')
                m_Data->textFormatter.writeFormattedText(arg->name, true, true);
            else if (arg->minCount == 0)
                m_Data->textFormatter.writeFormattedText("[<" + arg->name + ">]", true, true);
            else
                m_Data->textFormatter.writeFormattedText("<" + arg->name + ">", true, true);
        }
        m_Data->textFormatter.newline();
        m_Data->textFormatter.popIndentation();
    }

    std::string HelpWriter::generateUsage() const
    {

        return {};
    }

    void HelpWriter::writeArgumentSections() const
    {

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
