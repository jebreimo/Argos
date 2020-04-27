//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParsedArgumentsImpl.hpp"

#include <algorithm>
#include <cassert>
#include "Argos/ArgumentView.hpp"
#include "Argos/OptionView.hpp"
#include "ArgosThrow.hpp"
#include "HelpText.hpp"

namespace Argos
{
    namespace
    {
        template <typename It, typename Value, typename IsLess>
        It lowerBound(It begin, It end, Value&& v, IsLess isLess)
        {
            while (begin != end)
            {
                auto offset = std::distance(begin, end) / 2;
                auto mid = begin + offset;
                if (isLess(*mid, v))
                    begin = mid + 1;
                else
                    end = mid;
            }
            return begin;
        }
    }

    ParsedArgumentsImpl::ParsedArgumentsImpl(std::shared_ptr<ParserData> data)
        : m_Data(move(data))
    {
        assert(m_Data);
        for (auto& a : m_Data->arguments)
        {
            m_Ids.emplace_back(a->name, a->valueId, a->argumentId);
            if (!a->value.empty())
                m_Ids.emplace_back(a->value, a->valueId, a->argumentId);
        }
        for (auto& o : m_Data->options)
        {
            if (o->operation == OptionOperation::NONE)
                continue;

            for (auto& f : o->flags)
                m_Ids.emplace_back(f, o->valueId, o->argumentId);
            if (!o->value.empty())
                m_Ids.emplace_back(o->value, o->valueId, o->argumentId);
        }
        if (!m_Ids.empty())
        {
            using std::get;
            sort(m_Ids.begin(), m_Ids.end());
            for (auto it = next(m_Ids.begin()); it != m_Ids.end(); ++it)
            {
                auto p = prev(it);
                if (get<0>(*it) == get<0>(*p) && get<2>(*it) != get<2>(*p))
                    get<2>(*it) = get<2>(*p) = {};
            }
            m_Ids.erase(unique(m_Ids.begin(), m_Ids.end()), m_Ids.end());
        }
    }

    bool ParsedArgumentsImpl::has(ValueId valueId) const
    {
        return m_Values.find(valueId) != m_Values.end();
    }

    const std::vector<std::string>& ParsedArgumentsImpl::unprocessedArguments() const
    {
        return m_UnprocessedArguments;
    }

    void ParsedArgumentsImpl::addUnprocessedArgument(const std::string& arg)
    {
        m_UnprocessedArguments.push_back(arg);
    }

    std::string_view
    ParsedArgumentsImpl::assignValue(ValueId valueId,
                                     const std::string& value,
                                     ArgumentId argumentId)
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
            return appendValue(valueId, value, argumentId);

        it->second = {value, argumentId};
        auto nxt = next(it);
        while (nxt != m_Values.end() && nxt->first == valueId)
            m_Values.erase(nxt++);
        return it->second.first;
    }

    std::string_view
    ParsedArgumentsImpl::appendValue(ValueId valueId,
                                     const std::string& value,
                                     ArgumentId argumentId)
    {
        return m_Values.insert({valueId, {value, argumentId}})->second.first;
    }

    void ParsedArgumentsImpl::clearValue(ValueId valueId)
    {
        m_Values.erase(valueId);
    }

    ValueId ParsedArgumentsImpl::getValueId(std::string_view valueName) const
    {
        using std::get;
        auto it = lowerBound(m_Ids.begin(), m_Ids.end(), valueName,
                             [](auto& p, auto& s) {return get<0>(p) < s;});
        if (it == m_Ids.end() || get<0>(*it) != valueName)
            ARGOS_THROW("Unknown value: " + std::string(valueName));
        return get<1>(*it);
    }

    std::optional<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::getValue(ValueId valueId) const
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
            return {};
        auto nx = next(it);
        if (nx != m_Values.end() && nx->first == valueId)
            ARGOS_THROW("Attempt to read multiple values as a single value.");
        return it->second;
    }

    std::vector<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::getValues(ValueId valueId) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> result;
        for (auto it = m_Values.lower_bound(valueId);
             it != m_Values.end() && it->first == valueId; ++it)
            result.emplace_back(it->second);
        return result;
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ParsedArgumentsImpl::getArgumentViews(ValueId valueId) const
    {
        std::vector<std::unique_ptr<IArgumentView>> result;
        for (auto& a : m_Data->arguments)
        {
            if (a->valueId == valueId)
                result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        }
        for (auto& o : m_Data->options)
        {
            if (o->valueId == valueId)
                result.emplace_back(std::make_unique<OptionView>(o.get()));
        }
        return result;
    }

    std::unique_ptr<IArgumentView>
    ParsedArgumentsImpl::getArgumentView(ArgumentId argumentId) const
    {
        for (auto& a : m_Data->arguments)
        {
            if (a->argumentId == argumentId)
                return std::make_unique<ArgumentView>(a.get());
        }
        for (auto& o : m_Data->options)
        {
            if (o->argumentId == argumentId)
                return std::make_unique<OptionView>(o.get());
        }
        return {};
    }

    const std::shared_ptr<ParserData>& ParsedArgumentsImpl::parserData() const
    {
        return m_Data;
    }

    ParserResultCode ParsedArgumentsImpl::resultCode() const
    {
        return m_ResultCode;
    }

    void ParsedArgumentsImpl::setResultCode(ParserResultCode resultCode)
    {
        m_ResultCode = resultCode;
    }

    const OptionData* ParsedArgumentsImpl::stopOption() const
    {
        return m_StopOption;
    }

    void ParsedArgumentsImpl::setBreakingOption(const OptionData* option)
    {
        m_ResultCode = ParserResultCode::STOP;
        m_StopOption = option;
    }

    void ParsedArgumentsImpl::error(const std::string& message)
    {
        writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(m_Data->parserSettings.errorExitCode);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }

    void ParsedArgumentsImpl::error(const std::string& message, ArgumentId argumentId)
    {
        writeErrorMessage(*m_Data, message, argumentId);
        if (m_Data->parserSettings.autoExit)
            exit(m_Data->parserSettings.errorExitCode);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }
}
