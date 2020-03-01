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
        HelpWriter();

        void writeHelpText(ParserData& data) const;

        void writeErrorMessage(ParserData& data,
                               const std::string& msg) const;
    private:
        void writeUsage(ParserData& data) const;

        void writeBriefUsage(ParserData& data) const;

        void writeStopAndHelpUsage(ParserData& data) const;

        void writeArgumentSections(ParserData& data) const;

        void writeEndText(ParserData& data) const;

        std::optional<std::string> getCustomText(ParserData& data,
                                                 TextId textId) const;
    };
}
