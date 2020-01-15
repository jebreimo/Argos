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
#include "VisitorUtilities.hpp"

namespace Argos
{
    std::tuple<ArgumentOperation, OptionType, int, const std::string*>
    getArgumentParams(const ArgumentVariant & variant)
    {
        std::tuple<ArgumentOperation, OptionType, int, const std::string*> result;
        std::visit(overload {
                [&](const Argument* a)
                {
                    result = {a->operation,
                              OptionType::NORMAL,
                              a->m_InternalId,
                              nullptr};
                },
                [&](const Option* o)
                {
                    result = {o->operation,
                              o->optionType,
                              o->m_InternalId,
                              o->argument.empty() ? &o->value : nullptr};
                }
        }, variant);
        return result;
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

    ParserResult parseArguments(int argc, char** argv,
                                const std::shared_ptr<ArgumentData>& data)
    {
        if (argc == 0)
            ARGOS_THROW("argc is 0");
        if (!data)
            ARGOS_THROW("data is null");
        if (data->programName.empty())
            data->programName = argv[0];

        std::vector<std::string_view> args;
        for (int i = 1; i < argc; ++i)
            args.emplace_back(argv[i]);
        ArgumentIterator iterator(args);
        while (true)
        {
            auto arg = iterator.next();
            if (!arg)
                break;

            auto it = data->argumentMap.find(*arg);
            //if (it == data->argumentMap.end() && data->allowAbbreviatedOptions)
            //{
            //    it = data->argumentMap.lower_bound(*arg);
            //    //if (it != data->argumentMap.end()
            //    //    && startsWith(it->first, *arg)
            //    //    && next(it) == data->argumentMap.end()
            //    //    || !startsWith(next(it)->first, *arg))
            //}
            if (it != data->argumentMap.end())
            {
                auto params = getArgumentParams(it->second);
                switch (std::get<0>(params))
                {
                case ArgumentOperation::ASSIGN:
                    if (std::get<3>(params))
                    {
                        assign(data->values, std::get<2>(params),
                               *std::get<3>(params));
                    }
                    else if (auto value = iterator.nextValue(); value)
                    {
                        assign(data->values, std::get<2>(params), *value);
                    }
                    else
                    {
                        // TODO: Display error message about missing argument.
                    }
                case ArgumentOperation::APPEND:
                case ArgumentOperation::CLEAR:
                case ArgumentOperation::NONE:
                    break;
                }
            }
        }
        return ParserResult();
    }
}
