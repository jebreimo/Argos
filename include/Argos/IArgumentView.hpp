//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "ArgosEnums.hpp"

namespace Argos
{
    class IArgumentView
    {
    public:
        virtual ~IArgumentView() = default;

        virtual const std::string& text() const = 0;

        virtual const std::string& section() const = 0;

        virtual const std::string& valueName() const = 0;

        virtual bool hidden() const = 0;

        virtual int id() const = 0;
    };
}
