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

        void build_option_index(bool case_insensitive);

        const OptionData* find_option(std::string_view flag,
                                      bool allow_abbreviations,
                                      bool case_insensitive) const;

        const CommandData* find_command(std::string_view name,
                                        bool case_insensitive) const;

        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;
        std::vector<std::unique_ptr<CommandData>> commands;
        std::string name;
        std::map<TextId, TextSource> texts;
        std::string current_section;
        Visibility visibility = Visibility::NORMAL;
        std::optional<bool> require_command;
        /**
         * The heading the command is listed under in the parent
         * command's help.
         */
        std::string section;

    private:
        const OptionData* find_option_impl(std::string_view flag,
                                           bool allow_abbreviations,
                                           bool case_insensitive) const;

        std::vector<std::pair<std::string_view, const OptionData*>> option_index;
    };

    struct ParserSettings;

    /**
     * Finish the initialization of this command and any subcommands, and
     * make them ready for parsing arguments.
     */
    void finish_initialization(CommandData& cmd,
                               const ParserData& data,
                               ValueId start_id = ValueId(0));

    bool has_flag(const CommandData& cmd,
                  std::string_view flag,
                  const ParserSettings& settings);
}
