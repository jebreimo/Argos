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
#include "ParserResult.hpp"
#include "Option.hpp"
#include "ArgumentData.hpp"

namespace Argos
{
    ParserResult parseArguments(int argc, char* argv[],
                                const std::shared_ptr<ArgumentData>& data);
}
