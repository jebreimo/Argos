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
        outStream() << msg << "\n";
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
        auto it = m_Data->helpSettings.m_Texts.find(TextId::USAGE_TITLE);
        //if (it->)
    }

    void HelpWriter::writeBriefUsage() const
    {

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

    std::ostream& HelpWriter::outStream() const
    {
        return m_Data->helpSettings.stream ? *m_Data->helpSettings.stream
                                           : std::cout;
    }

    std::ostream& HelpWriter::errStream() const
    {
        return m_Data->helpSettings.stream ? *m_Data->helpSettings.stream
                                           : std::cerr;
    }
}
