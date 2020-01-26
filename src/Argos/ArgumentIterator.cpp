//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentIterator.hpp"
#include "ArgumentIteratorImpl.hpp"

namespace Argos
{
    ArgumentIterator::ArgumentIterator(int argc, char* argv[],
                                       std::shared_ptr<ParserData> parserData)
        : m_Impl(std::make_unique<ArgumentIteratorImpl>(argc, argv,
                                                        move(parserData)))
    {}


}
