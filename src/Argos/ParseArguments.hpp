//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argos/ArgumentIterator.hpp"
#include "ParserData.hpp"

namespace argos
{
    ParsedArguments parse_arguments(std::vector<std::string_view> args,
                                    const std::shared_ptr<ParserData>& data);

    ArgumentIterator
    make_argument_iterator(std::vector<std::string_view> args,
                           const std::shared_ptr<ParserData>& data);

    void add_missing_options(ParserData& data);
}
