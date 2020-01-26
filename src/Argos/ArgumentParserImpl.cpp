//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgosException.hpp"
#include "ArgumentParserImpl.hpp"
#include "ParserData.hpp"

//#define CHECK_PARSER_DATA_POINTER() \
//        if (!m_Data) \
//            ARGOS_THROW("ArgumentParser cannot be used after parsing arguments.");

namespace Argos
{
    //namespace
    //{
    //}
    //
    //ArgumentParserImpl::ArgumentParserImpl()
    //    : m_Data(std::make_shared<ParserData>())
    //{}
    //
    //ArgumentParserImpl::ArgumentParserImpl(const std::string& programName)
    //    : m_Data(std::make_shared<ParserData>())
    //{
    //    m_Data->programName = programName;
    //}
    //
    //void ArgumentParserImpl::add(Option& option)
    //{
    //    CHECK_PARSER_DATA_POINTER();
    //    // TODO: check if option is valid
    //    // TODO: check if flags are unique
    //}

    //void ArgumentParserImpl::add(Argument builder)
    //{
    //    m_Data->arguments.push_back(builder.get());
    //}
    //
    //void ArgumentParserImpl::add(Option builder)
    //{
    //    m_Data->options.push_back(builder.get());
    //}

    //ArgumentBuilder Argos::addArgument(const std::string& name,
    //                                   bool allowRedefinitions)
    //{
    //    //m_Data->arguments.emplace_back();
    //    //m_Data->arguments.back().name = name;
    //    return ArgumentBuilder(name); //&m_Data->arguments.back());
    //}
    //
    //OptionBuilder Argos::addOption(const std::vector<std::string>& flags,
    //                               bool allowRedefinitions)
    //{
    //    // TODO: check flag
    //    m_Data->options.emplace_back();
    //    m_Data->options.back().flags = flags;
    //    return OptionBuilder(&m_Data->options.back());
    //}

    //std::unique_ptr<ParsedArgumentsImpl> ArgumentParserImpl::parse(int argc, char** argv)
    //{
    //    CHECK_PARSER_DATA_POINTER();
    //    auto argumentData = std::move(m_Data);
    //    finalizeArgumentData(*argumentData);
    //    ArgumentIteratorImpl parser(argc, argv, argumentData);
    //    return ParsedArgumentsImpl(argumentData);
    //}
    //
    //const ParserData& ArgumentParserImpl::parserData() const
    //{
    //    if (!m_Data)
    //        ARGOS_THROW("ArgumentParser cannot be used after parsing arguments.");
    //    return *m_Data;
    //}
    //
    //ParserData& ArgumentParserImpl::parserData()
    //{
    //    if (!m_Data)
    //        ARGOS_THROW("ArgumentParser cannot be used after parsing arguments.");
    //    return *m_Data;
    //}
}
