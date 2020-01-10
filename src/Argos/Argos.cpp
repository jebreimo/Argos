//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos.hpp"
#include "ArgumentParser.hpp"

namespace Argos
{
    Argos::Argos()
    {}

    Argos::Argos(const std::string& programName)
        : m_ProgramName(programName)
    {}

    Argos::~Argos() = default;

    Argos::Argos(Argos&&) = default;

    Argos& Argos::operator=(Argos&&) = default;

    ArgumentParser Argos::makeParser() const
    {
        return ArgumentParser(m_Arguments, m_Options);
    }

    ParserResult Argos::parse(int argc, char** argv)
    {
        auto parser = makeParser();
        return parser.parse(argc, argv);
    }

    bool Argos::autoExit() const
    {
        return m_AutoExit;
    }

    void Argos::setAutoExit(bool autoExit)
    {
        m_AutoExit = autoExit;
    }

    const std::string& Argos::programName() const
    {
        return m_ProgramName;
    }

    void Argos::setProgramName(const std::string& programName)
    {
        m_ProgramName = programName;
    }
}
