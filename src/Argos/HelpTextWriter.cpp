//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "HelpTextWriter.hpp"

#include <iostream>

namespace Argos
{
    HelpTextWriter::HelpTextWriter(std::shared_ptr<ArgumentData> data)
        : m_Data(move(data))
    {}

    void HelpTextWriter::writeHelpText() const
    {

    }

    void HelpTextWriter::writeErrorMessage(const std::string& msg) const
    {
        std::cerr << msg << "\n";
    }

    void HelpTextWriter::writeErrorMessage(const Argument& argument, const std::string& msg) const
    {

    }

    void HelpTextWriter::writeErrorMessage(const Option& option, const std::string& msg) const
    {

    }
}
