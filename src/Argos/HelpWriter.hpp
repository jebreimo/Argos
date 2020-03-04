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
        static void writeUsage(ParserData& data);

        static void writeBriefUsage(ParserData& data);

        static void writeStopAndHelpUsage(ParserData& data);

        static void writeArgumentSections(ParserData& data);

        static std::optional<std::string> getCustomText(ParserData& data,
                                                        TextId textId);

        static bool writeCustomText(ParserData& data, TextId textId);
    };
}
