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
#include "ArgumentIteratorImpl.hpp"
#include "Argos/Argument.hpp"
#include "Argos/Option.hpp"

namespace Argos
{
    class ParserData;

    class ParsedArguments;

    class ArgumentParserImpl
    {
    public:
        //ArgumentParserImpl();
        //
        //explicit ArgumentParserImpl(const std::string& programName);
        //
        //void add(Argument& argument);
        //
        //void add(Option& option);
        //
        //std::unique_ptr<ParsedArgumentsImpl> parse(int argc, char* argv[]);
        //
        //std::unique_ptr<ParsedArgumentsImpl> parse(int argc, char* argv[]) const;
        //
        //std::unique_ptr<ArgumentIteratorImpl> makeIterator(int argc, char* argv[]);
        //
        //std::unique_ptr<ArgumentIteratorImpl> makeIterator(int argc, char* argv[]) const;
        //
        //const ParserData& parserData() const;
        //
        //ParserData& parserData();

        //bool autoExit() const;
        //
        //void setAutoExit(bool autoExit);
        //
        //const std::string& programName() const;
        //
        //void setProgramName(const std::string& programName);
    private:
        //void finalizeArgumentData(const ParserData& data) const;
        //
        //std::shared_ptr<ParserData> m_Data;
        //std::set<std::string> m_Names;
        //ParserBuilder m_ParserBuilder;
        //std::string m_ProgramName;
        //bool m_AutoExit;
    };
}
