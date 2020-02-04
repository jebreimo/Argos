//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParsedArgumentsBuilder.hpp"

#include "ParsedArgumentsImpl.hpp"

namespace Argos
{
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(ParsedArgumentsImpl* impl)
        : m_Impl(impl)
    {}
}
