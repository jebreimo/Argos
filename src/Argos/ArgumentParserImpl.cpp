//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentParserImpl.hpp"
#include "ParserData.hpp"

namespace Argos
{
    namespace
    {
    }

    ArgumentParserImpl::ArgumentParserImpl()
    {}

    ArgumentParserImpl::ArgumentParserImpl(const std::string& programName)
        : m_Data(std::make_shared<ParserData>())
    {
        m_Data->programName = programName;
    }

    ArgumentParserImpl::~ArgumentParserImpl() = default;

    ArgumentParserImpl::ArgumentParserImpl(ArgumentParserImpl&&) = default;

    ArgumentParserImpl& ArgumentParserImpl::operator=(ArgumentParserImpl&&) = default;

    void ArgumentParserImpl::add(Argument builder)
    {
        m_Data->arguments.push_back(builder.get());
    }

    void ArgumentParserImpl::add(Option builder)
    {
        m_Data->options.push_back(builder.get());
    }

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

    ParserResultImpl ArgumentParserImpl::parse(int argc, char** argv, bool preserveArgParser)
    {
        auto argumentData = m_Data;
        if (preserveArgParser)
            m_Data = std::make_shared<ParserData>(*m_Data);
        finalizeArgumentData(*argumentData);
        ArgumentIteratorImpl parser(argc, argv, argumentData);
        // parser.next(argc, argv, argumentData);
        return ParserResultImpl(argumentData);
    }

    bool ArgumentParserImpl::autoExit() const
    {
        return m_Data->autoExit;
    }

    void ArgumentParserImpl::setAutoExit(bool autoExit)
    {
        m_Data->autoExit = autoExit;
    }

    const std::string& ArgumentParserImpl::programName() const
    {
        return m_Data->programName;
    }

    void ArgumentParserImpl::setProgramName(const std::string& programName)
    {
        m_Data->programName = programName;
    }

    void ArgumentParserImpl::finalizeArgumentData(const ParserData& data) const
    {
        struct InternalIdMaker
        {
            int n = 1;
            std::map<std::string, int> explicitIds;

            std::pair<int, bool> makeNumericId(const std::string& stringId)
            {
                if (!stringId.empty())
                    return {n++, false};

                auto it = explicitIds.find(stringId);
                if (it != explicitIds.end())
                    return {it->second, false};

                it = explicitIds.emplace(stringId, n++).first;
                return {it->second, true};
            }
        };

        InternalIdMaker idMaker;
        for (auto& a : m_Data->arguments)
        {
            auto[internalId, newId] = idMaker.makeNumericId(a.valueName);
            a.valueId_ = internalId;
            //if (newId)
            //    m_Data->argumentMap.emplace(a.id, &a);
            //m_Data->argumentMap.emplace(a.name, &a);
        }
        for (auto& o : m_Data->options)
        {
            auto[internalId, newId] = idMaker.makeNumericId(o.valueName);
            o.valueId_ = internalId;
            //if (newId)
            //    m_Data->argumentMap.emplace(o.id, &o);
            //for (auto& flag : o.flags)
            //    m_Data->argumentMap.emplace(flag, &o);
        }
    }
}
