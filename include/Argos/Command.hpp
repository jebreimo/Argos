//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argument.hpp"
#include "Option.hpp"

namespace argos
{
    struct CommandData;

    class Command
    {
    public:
        explicit Command(std::string name);

        Command(const Command&);

        Command(Command&&) noexcept;

        ~Command();

        Command& operator=(const Command&);

        Command& operator=(Command&&) noexcept;

        Command& add(Argument argument);

        Command& add(Option option);

        Command& add(Command command);

        Command& about(std::string text);

        Command& section(const std::string& name);

        Command& text(TextId textId, std::string text);

        Command& text(TextId textId, std::function<std::string()> callback);

        Command& visibility(Visibility visibility);

        Command& id(int id);

        std::unique_ptr<CommandData> release();
    private:
        void check_command() const;

        std::unique_ptr<CommandData> data_;
    };
}
