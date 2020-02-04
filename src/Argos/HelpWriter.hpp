//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "ParserData.hpp"

namespace Argos
{
    class HelpWriter
    {
    public:
        explicit HelpWriter(std::shared_ptr<ParserData> data);

        void writeHelpText() const;

        void writeErrorMessage(const std::string& msg) const;

        void writeErrorMessage(const ArgumentData& argument,
                               const std::string& msg) const;

        void writeErrorMessage(const OptionData& option,
                               const std::string& msg) const;
    private:
        std::ostream& outStream() const;

        std::ostream& errStream() const;

        std::shared_ptr<ParserData> m_Data;
    };
}
