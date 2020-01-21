//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>
#include "ArgumentBuilder.hpp"
#include "ArgumentData.hpp"
#include "ArgumentIterator.hpp"
#include "Option.hpp"
#include "ParserResult.hpp"

namespace Argos
{
    class ArgumentParser
    {
    public:
        ArgumentParser(int argc, char* argv[],
                       std::shared_ptr<ArgumentData> data);

        ArgumentParser(const std::vector<std::string>& args,
                       std::shared_ptr<ArgumentData> data);

        std::optional<int> next();
    private:
        std::shared_ptr<ArgumentData> m_Data;
        std::optional<std::vector<std::pair<size_t, const Argument*>>> m_Arguments;
        std::vector<std::pair<std::string_view, const Option*>> m_Options;
        ParserResult m_ParserResult;
        ArgumentIterator m_ArgumentIterator;
    };

    ParserResult parseArguments(int argc, char* argv[],
                                const std::shared_ptr<ArgumentData>& data);
}
