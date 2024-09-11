//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <memory>
#include "ArgumentData.hpp"
#include "OptionData.hpp"

namespace argos
{
    struct ParserData;

    struct CommandData
    {
        CommandData();

        CommandData(const CommandData&);

        CommandData(CommandData&&) noexcept;

        ~CommandData();

        CommandData& operator=(const CommandData&);

        CommandData& operator=(CommandData&&) noexcept;

        /**
         * Complete the definition of this command and any subcommands.
         */
        void complete_definition(const ParserData& data);

        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;
        std::vector<std::unique_ptr<CommandData>> commands;
        std::string name;
        std::map<TextId, TextSource> texts;
        std::string current_section;
        std::optional<bool> require_command;
        /**
         * The section the command is listed in in the parent command's help.
         */
        std::string section;
    };
}
