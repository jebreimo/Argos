//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos.hpp"
#include "ArgumentData.hpp"

namespace Argos
{
    namespace
    {
    }

    Argos::Argos()
    {}

    Argos::Argos(const std::string& programName)
        : m_Data(std::make_shared<ArgumentData>())
    {
        m_Data->programName = programName;
    }

    Argos::~Argos() = default;

    Argos::Argos(Argos&&) = default;

    Argos& Argos::operator=(Argos&&) = default;

    void Argos::add(ArgumentBuilder builder)
    {
        m_Data->arguments.push_back(builder.get());
    }

    void Argos::add(OptionBuilder builder)
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

    ParserResult Argos::parse(int argc, char** argv, bool preserveArgParser)
    {
        auto argumentData = m_Data;
        if (preserveArgParser)
            m_Data = std::make_shared<ArgumentData>(*m_Data);
        finalizeArgumentData(*argumentData);
        ArgumentParser parser(argc, argv, argumentData);
        // parser.next(argc, argv, argumentData);
        return ParserResult(argumentData);
    }

    bool Argos::autoExit() const
    {
        return m_Data->autoExit;
    }

    void Argos::setAutoExit(bool autoExit)
    {
        m_Data->autoExit = autoExit;
    }

    const std::string& Argos::programName() const
    {
        return m_Data->programName;
    }

    void Argos::setProgramName(const std::string& programName)
    {
        m_Data->programName = programName;
    }

    void Argos::finalizeArgumentData(const ArgumentData& data) const
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
