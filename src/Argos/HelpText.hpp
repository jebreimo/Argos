//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "ParserData.hpp"

namespace Argos
{
    void writeHelpText(ParserData& data);

    void writeErrorMessage(ParserData& data, const std::string& msg);

    void writeErrorMessage(ParserData& data,
                           const std::string& msg,
                           ArgumentId argumentId);
}
