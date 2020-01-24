//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentParser.hpp"
#include "ArgumentIterator.hpp"
#include "ArgosException.hpp"
#include "StringUtilities.hpp"
#include "HelpTextWriter.hpp"

namespace Argos
{
    namespace
    {
        std::vector<std::string_view> makeStringViewVector(
                int count, char** strings, bool ignoreFirst)
        {
            std::vector<std::string_view> result;
            for (int i = ignoreFirst ? 1 : 0; i < count; ++i)
                result.emplace_back(strings[i]);
            return result;
        }

        std::vector<std::string_view> makeStringViewVector(
                const std::vector<std::string>& strings)
        {
            std::vector<std::string_view> result;
            for (auto& s : strings)
                result.emplace_back(s);
            return result;
        }

        using OptionTable = std::vector<std::pair<std::string_view, const Option*>>;

        OptionTable makeOptionIndex(const std::vector<Option>& options,
                                    bool caseInsensitive)
        {
            OptionTable index;
            for (auto& option : options)
            {
                for (auto& flag : option.flags)
                    index.emplace_back(flag, &option);
            }
            if (caseInsensitive)
            {
                std::sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return isLessCI(a.first, b.first);
                });
            }
            else
            {
                std::sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return a.first < b.first;
                });
            }
            return index;
        }

        OptionTable::const_iterator findOptionCS(const OptionTable& options,
                                                 std::string_view arg)
        {
            OptionTable::value_type key = {arg, nullptr};
            return std::lower_bound(
                    options.begin(), options.end(),
                    key,
                    [&](auto& a, auto& b) {return a.first < b.first;});
        }

        OptionTable::const_iterator findOptionCI(const OptionTable& options,
                                                 std::string_view arg)
        {
            OptionTable::value_type key = {arg, nullptr};
            return std::lower_bound(
                    options.begin(), options.end(),
                    key,
                    [&](auto& a, auto& b) {return isLessCI(a.first, b.first);});
        }

        const Option* findOption(const OptionTable& options,
                                 std::string_view arg,
                                 bool allowAbbreviations,
                                 bool caseInsensitive)
        {
            OptionTable::value_type key = {arg, nullptr};
            auto it = caseInsensitive ? findOptionCI(options, arg)
                                      : findOptionCS(options, arg);
            if (it == options.end())
            {
                if (arg.size() > 2 && arg.back() == '=')
                    arg = arg.substr(arg.size() - 1);
                it = caseInsensitive ? findOptionCI(options, arg)
                                     : findOptionCS(options, arg);
            }
            if (it == options.end())
                return nullptr;
            if (it->first == arg)
                return it->second;
            if (caseInsensitive && areEqualCI(it->first, arg))
                return it->second;
            if (!allowAbbreviations)
                return nullptr;
            if (!startsWith(it->first, arg, caseInsensitive))
                return nullptr;
            auto nxt = next(it);
            if (nxt != options.end()
                && startsWith(nxt->first, arg, caseInsensitive))
                return nullptr;
            return it->second;
        }

        void assign(std::multimap<int, std::string>& map,
                    int key, const std::string& value)
        {
            auto it = map.lower_bound(key);
            if (it == map.end() || it->first != key)
            {
                map.emplace(key, value);
                return;
            }

            it->second = value;
            ++it;
            while (it != map.end() && it->first == key)
                map.erase(it++);
        }

        bool isOption(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            if (style == OptionStyle::WINDOWS)
                return s[0] == '/';
            else
                return s[0] == '-';
        }
    }

    ArgumentParser::ArgumentParser(int argc, char** argv, std::shared_ptr<ArgumentData> data)
            : m_Data(move(data)),
              m_Options(makeOptionIndex(m_Data->options, m_Data->caseInsensitive)),
              m_ParserResult(m_Data),
              m_ArgumentIterator(makeStringViewVector(argc, argv, true))
    {
        if (!ArgumentCounter::requiresArgumentCount(m_Data->arguments))
            m_ArgumentCounter.emplace(m_Data->arguments);
    }

    ArgumentParser::ArgumentParser(const std::vector<std::string>& args,
                                   std::shared_ptr<ArgumentData> data)
            : m_Data(move(data)),
              m_Options(makeOptionIndex(m_Data->options, m_Data->caseInsensitive)),
              m_ParserResult(m_Data),
              m_ArgumentIterator(makeStringViewVector(args))
    {
        if (!ArgumentCounter::requiresArgumentCount(m_Data->arguments))
            m_ArgumentCounter.emplace(m_Data->arguments);
    }

    std::optional<int> ArgumentParser::next()
    {
        return next(true);
    }

    void ArgumentParser::processOption(const Option& option)
    {
        switch (option.operation)
        {
        case ArgumentOperation::ASSIGN:
            if (!option.value.empty())
            {
                assign(m_Data->values, option.m_InternalId, option.value);
            }
            else if (auto value = m_ArgumentIterator.nextValue(); value)
            {
                assign(m_Data->values, option.m_InternalId, *value);
            }
            else
            {
                // TODO: Display error message about missing argument.
            }
            break;
        case ArgumentOperation::APPEND:
            if (!option.value.empty())
            {
                m_Data->values.emplace(option.m_InternalId, option.value);
            }
            else if (auto value = m_ArgumentIterator.nextValue(); value)
            {
                m_Data->values.emplace(option.m_InternalId, *value);
            }
            else
            {
                // TODO: Display error message about missing argument.
            }
            break;
        case ArgumentOperation::CLEAR:
            m_Data->values.erase(option.m_InternalId);
            break;
        case ArgumentOperation::NONE:
            break;
        }
        if (m_ArgumentIterator.hasRemainder())
        {
            // TODO: Display error message about incorrect argument.
        }
    }

    std::optional<int> ArgumentParser::next(bool requiresArgumentId)
    {
        auto arg = m_ArgumentIterator.next();
        if (!arg)
            return {};

        auto option = findOption(m_Options, *arg,
                                 m_Data->allowAbbreviatedOptions,
                                 m_Data->caseInsensitive);
        if (option)
        {
            processOption(*option);
        }
        else if (isOption(*arg, m_Data->optionStyle))
        {
            if (m_Data->ignoreUndefinedArguments && *arg == m_ArgumentIterator.current())
            {
                m_ParserResult.addUnprocessedArgument(*arg);
            }
            else
            {
                HelpTextWriter writer(m_Data);
                writer.writeErrorMessage(
                        "Invalid option: " + std::string(m_ArgumentIterator.current()));
            } // TODO Show error and return
        }
        else if (m_ArgumentCounter)
        {

        }
        else
        {

        }
        return {};
    }
}
