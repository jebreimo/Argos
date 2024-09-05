//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <memory>
#include "ArgumentData.hpp"
#include "OptionData.hpp"

namespace argos
{
    struct SubparserData
    {
        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;
        std::vector<std::unique_ptr<SubparserData>> sub_parsers;
        std::string name;
        std::map<TextId, TextSource> texts;
        std::string current_section;
    };
}
