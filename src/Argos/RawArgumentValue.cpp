//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/RawArgumentValue.hpp"

namespace Argos
{
    RawArgumentValue::RawArgumentValue(std::shared_ptr<ParserData> parser, std::string_view value)
    {

    }

    bool RawArgumentValue::hasValue() const
    {
        return false;
    }
}
