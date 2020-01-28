//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentParser.hpp"

#include <algorithm>
#include "Argos/ArgosException.hpp"
#include "ArgumentIteratorImpl.hpp"
#include "ParserData.hpp"

namespace Argos
{
    namespace
    {
        std::unique_ptr<ParserData> makeCopy(const ParserData& data)
        {
            auto result = std::make_unique<ParserData>();
            result->parserSettings = data.parserSettings;
            result->helpSettings = data.helpSettings;
            result->arguments.reserve(data.arguments.size());
            for (auto& a : data.arguments)
                result->arguments.push_back(std::make_shared<ArgumentData>(*a));
            result->options.reserve(data.options.size());
            for (auto& o : data.options)
                result->options.push_back(std::make_shared<OptionData>(*o));
            return result;
        }

        void generateInternalIds(const ParserData& data)
        {
            struct InternalIdMaker
            {
                int n = 1;
                std::map<std::string, int> explicitIds;

                int makeNumericId(const std::string& stringId)
                {
                    if (!stringId.empty())
                        return n++;

                    auto it = explicitIds.find(stringId);
                    if (it == explicitIds.end())
                        it = explicitIds.emplace(stringId, n++).first;
                    return it->second;
                }
            };
            InternalIdMaker idMaker;
            for (auto& a : data.arguments)
                a->valueId_ = idMaker.makeNumericId(a->valueName);
            for (auto& o : data.options)
                o->valueId_ = idMaker.makeNumericId(o->valueName);
        }

        ParsedArguments parseArguments(int argc, char** argv,
                                       std::shared_ptr<ParserData> data)
        {
            if (argc >= 1 && data->helpSettings.programName.empty())
                data->helpSettings.programName = argv[0];
            generateInternalIds(*data);
            return ParsedArguments(
                    ArgumentIteratorImpl::parse(argc, argv, move(data)));
        }
    }
    ArgumentParser::ArgumentParser()
        : ArgumentParser(std::string())
    {}

    ArgumentParser::ArgumentParser(const std::string& programName)
        : m_Data(std::make_unique<ParserData>())
    {
        data().helpSettings.programName = programName;
    }

    ArgumentParser::ArgumentParser(ArgumentParser&& rhs) noexcept
        : m_Data(move(rhs.m_Data))
    {}

    ArgumentParser::~ArgumentParser() = default;

    ArgumentParser& ArgumentParser::operator=(ArgumentParser&& rhs) noexcept
    {
        m_Data = move(rhs.m_Data);
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option& option)
    {
        // TODO: check that either argument or value is set if operation is ASSIGN or APPEND.
        // TODO: check if flags have the right prefix (- or /)
        // TODO: check that short flags are two characters long.
        // TODO: check if flags contain = anywhere, but the end.
        data().options.push_back(option.release());
        return *this;
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv) const
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return parseArguments(argc, argv, makeCopy(data()));
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv)
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return parseArguments(argc, argv, std::move(m_Data));
    }

    bool ArgumentParser::autoExitEnabled() const
    {
        return data().parserSettings.autoExit;
    }

    ArgumentParser& ArgumentParser::autoExitEnabled(bool value)
    {
        data().parserSettings.autoExit = value;
        return *this;
    }

    const std::string& ArgumentParser::programName() const
    {
        return data().helpSettings.programName;
    }

    ArgumentParser& ArgumentParser::programName(const std::string& name)
    {
        data().helpSettings.programName = name;
        return *this;
    }

    const ParserData& ArgumentParser::data() const
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return *m_Data;
    }

    ParserData& ArgumentParser::data()
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return *m_Data;
    }
}
