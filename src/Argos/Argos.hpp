//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "ArgumentBuilder.hpp"

#include <memory>
#include <string>
#include "ArgumentBuilder.hpp"
#include "OptionBuilder.hpp"
#include "ParserResult.hpp"

namespace Argos
{
    class ArgumentParser;

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

        ArgumentBuilder& addArgument(const std::string& name);

        OptionBuilder& addOption(const std::string& name);

        ArgumentParser makeParser() const;

        ParserResult parse(int argc, char* argv[]);

        bool autoExit() const;

        void setAutoExit(bool autoExit);

        const std::string& programName() const;

        void setProgramName(const std::string& programName);
    private:
        std::vector<Argument> m_Arguments;
        std::vector<Option> m_Options;
        std::string m_ProgramName;
        bool m_AutoExit;
    };
}
