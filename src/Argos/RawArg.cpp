//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "RawArg.hpp"

namespace Argos
{
    RawArg::RawArg(std::shared_ptr<ArgumentIterator> parser, std::string_view value)
    {

    }

    bool RawArg::hasValue() const
    {
        return false;
    }
}
