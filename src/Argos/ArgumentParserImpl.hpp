//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include <set>
#include "ParserBuilder.hpp"
#include "ParserResultImpl.hpp"

namespace Argos
{
    class ParserData;

    class ArgumentParserImpl
    {
    public:
        ArgumentParserImpl();

        ArgumentParserImpl(const std::string& programName);

        ArgumentParserImpl(ArgumentParserImpl&&);

        ArgumentParserImpl(const ArgumentParserImpl&) = delete;

        ~ArgumentParserImpl();

        ArgumentParserImpl& operator=(ArgumentParserImpl&&);

        ArgumentParserImpl& operator=(const ArgumentParserImpl&) = delete;

        void add(Argument builder);

        void add(Option builder);

        ParserResultImpl parse(int argc, char* argv[], bool preserveArgParser = false);

        bool autoExit() const;

        void setAutoExit(bool autoExit);

        const std::string& programName() const;

        void setProgramName(const std::string& programName);
    private:
        void finalizeArgumentData(const ParserData& data) const;

        std::shared_ptr<ParserData> m_Data;
        std::set<std::string> m_Names;
        //ParserBuilder m_ParserBuilder;
        //std::string m_ProgramName;
        //bool m_AutoExit;
    };
}
