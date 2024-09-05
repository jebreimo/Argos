//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Argos/Argument.hpp>
#include <Argos/Option.hpp>

namespace argos
{
    struct SubparserData;

    class Subparser
    {
    public:
        Subparser(std::string name);

        Subparser(const Subparser&);

        Subparser(Subparser&&) noexcept;

        ~Subparser();

        Subparser& operator=(const Subparser&);

        Subparser& operator=(const Subparser&&);

        Subparser& add(Argument argument);

        Subparser& add(Option option);

        Subparser& add(Subparser parser);

        Subparser& about(std::string text);

        Subparser& section(const std::string& name);

        Subparser& text(TextId textId, std::string text);

        Subparser& text(TextId textId, std::function<std::string()> callback);

        std::unique_ptr<SubparserData> release();
    private:
        std::unique_ptr<SubparserData> data_;
    };
}
