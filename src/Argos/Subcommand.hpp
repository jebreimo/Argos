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
    struct CommandData;

    class Subcommand
    {
    public:
        Subcommand(std::string name);

        Subcommand(const Subcommand&);

        Subcommand(Subcommand&&) noexcept;

        ~Subcommand();

        Subcommand& operator=(const Subcommand&);

        Subcommand& operator=(const Subcommand&&);

        Subcommand& add(Argument argument);

        Subcommand& add(Option option);

        Subcommand& add(Subcommand parser);

        Subcommand& about(std::string text);

        Subcommand& section(const std::string& name);

        Subcommand& text(TextId textId, std::string text);

        Subcommand& text(TextId textId, std::function<std::string()> callback);

        std::unique_ptr<CommandData> release();
    private:
        std::unique_ptr<CommandData> data_;
    };
}
