//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseArguments.hpp"

#include <algorithm>
#include <iostream>

#include "ArgosThrow.hpp"
#include "Argos/Option.hpp"
#include "ArgumentIteratorImpl.hpp"
#include "StringUtilities.hpp"

namespace argos
{
    ParsedArguments parse_arguments(std::vector<std::string_view> args,
                                    const std::shared_ptr<ParserData>& data)
    {
        finish_initialization(*data);
        return ParsedArguments(
            ArgumentIteratorImpl::parse(std::move(args), data));
    }

    ArgumentIterator
    make_argument_iterator(std::vector<std::string_view> args,
                           const std::shared_ptr<ParserData>& data)
    {
        finish_initialization(*data);
        return {std::move(args), data};
    }
}
