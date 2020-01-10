//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ArgumentParser.hpp"

namespace Argos
{
    namespace
    {
        int insertArguments(const std::vector<Argument>& args,
                            std::vector<std::pair<Argument, int>>& argIds,
                            std::map<std::string, int>& nameIds,
                            int nextId)
        {
            for (auto& arg : args)
            {
                int id;
                if (arg.id.empty())
                {
                    id = nextId++;
                }
                else if (auto it = nameIds.find(arg.id); it != nameIds.end())
                {
                    id = it->second;
                }
                else
                {
                    id = nextId++;
                    nameIds.emplace(arg.id, id);
                }
                nameIds.emplace(arg.name, id);
                argIds.emplace_back(arg, id);
            }
            return nextId;
        }

        int insertOptions(const std::vector<Option>& opts,
                          std::vector<std::pair<Option, int>>& optIds,
                          std::map<std::string, int>& nameIds,
                          int nextId)
        {
            for (auto opt : opts)
            {
                if (opt.flags.empty())
                    continue;

                int id;
                if (opt.id.empty())
                {
                    id = nextId++;
                }
                else if (auto it = nameIds.find(opt.id); it != nameIds.end())
                {
                    id = it->second;
                }
                else
                {
                    id = nextId++;
                    nameIds.emplace(opt.id, id);
                }
                for (auto& flag : opt.flags)
                    nameIds.emplace(flag, id);
                optIds.emplace_back(opt, id);
            }
            return nextId;
        }
    }

    ArgumentParser::ArgumentParser(const std::vector<Argument>& arguments,
                                   const std::vector<Option>& options)
    {
        auto nextId = insertArguments(arguments, m_Arguments, m_NameIds, 1);
        insertOptions(options, m_Options, m_NameIds, nextId);
    }

    ParserResult ArgumentParser::parse(int argc, char** argv)
    {
        return ParserResult();
    }
}
