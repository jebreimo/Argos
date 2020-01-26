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

    }

    void HelpWriter::writeErrorMessage(const std::string& msg) const
    {
        std::cerr << msg << "\n";
    }

    void HelpWriter::writeErrorMessage(const ArgumentData& argument, const std::string& msg) const
    {

    }

    void HelpWriter::writeErrorMessage(const OptionData& option, const std::string& msg) const
    {

    }
}
