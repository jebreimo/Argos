//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include <string>
#include "ArgosEnums.hpp"

namespace Argos
{
    struct ArgumentData;

    class Argument
    {
    public:
        Argument();

        explicit Argument(const std::string& name);

        Argument(const Argument&) = delete;

        Argument(Argument&&) noexcept;

        ~Argument();

        Argument& operator=(const Argument&) = delete;

        Argument& operator=(Argument&&) noexcept;

        Argument& text(const std::string& text);

        Argument& section(const std::string& name);

        Argument& valueName(const std::string& id);

        Argument& operation(ArgumentOperation operation);

        Argument& hidden(bool hidden);

        Argument& id(int id);

        Argument& name(const std::string& name);

        Argument& optional(bool optional);

        Argument& count(unsigned n);

        Argument& count(unsigned minCount, unsigned maxCount);

        std::shared_ptr<ArgumentData> release();
    private:
        std::shared_ptr<ArgumentData> m_Argument;
    };
}
