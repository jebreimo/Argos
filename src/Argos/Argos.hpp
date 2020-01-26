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
#include "ParserResult.hpp"

namespace Argos
{
    class ArgumentData;

    class Argos
    {
    public:
        Argos();

        Argos(const std::string& programName);

        Argos(Argos&&);

        Argos(const Argos&) = delete;

        ~Argos();

        Argos& operator=(Argos&&);

        Argos& operator=(const Argos&) = delete;

        void add(ArgumentBuilder builder);

        void add(OptionBuilder builder);

        ParserResult parse(int argc, char* argv[], bool preserveArgParser = false);

        bool autoExit() const;

        void setAutoExit(bool autoExit);

        const std::string& programName() const;

        void setProgramName(const std::string& programName);
    private:
        void finalizeArgumentData(const ArgumentData& data) const;

        std::shared_ptr<ArgumentData> m_Data;
        std::set<std::string> m_Names;
        //ParserBuilder m_ParserBuilder;
        //std::string m_ProgramName;
        //bool m_AutoExit;
    };
}
