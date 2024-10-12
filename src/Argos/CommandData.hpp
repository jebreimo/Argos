//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include <memory>
#include "ArgumentData.hpp"
#include "OptionData.hpp"

namespace argos
{
    struct CommandData
    {
        CommandData();

        CommandData(const CommandData&);

        CommandData(CommandData&&) noexcept;

        ~CommandData();

        CommandData& operator=(const CommandData&);

        CommandData& operator=(CommandData&&) noexcept;

        void add(std::unique_ptr<ArgumentData> arg);

        void add(std::unique_ptr<OptionData> opt);

        void add(std::unique_ptr<CommandData> cmd);

        void copy_from(const CommandData& cmd);

        void build_option_index(bool case_insensitive);

        [[nodiscard]] const OptionData*
        find_option(std::string_view flag,
                    bool allow_abbreviations,
                    bool case_insensitive) const;

        [[nodiscard]] const CommandData*
        find_command(std::string_view name,
                     bool case_insensitive) const;

        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;
        std::vector<std::unique_ptr<CommandData>> commands;
        std::string current_section;

        std::string name;
        std::string full_name;
        std::map<TextId, TextSource> texts;
        Visibility visibility = Visibility::NORMAL;
        std::optional<bool> require_subcommand;
        bool multi_command = false;
        /**
         * The heading the command is listed under in the parent
         * command's help.
         */
        std::string section;
        int id = 0;
        ArgumentId argument_id = {};

    private:
        [[nodiscard]] const OptionData*
        find_option_impl(std::string_view flag,
                         bool allow_abbreviations,
                         bool case_insensitive) const;

        std::vector<std::pair<std::string_view, const OptionData*>> option_index;
    };

    struct ParserData;

    /**
     * Finish the initialization of this command and any subcommands, and
     * make them ready for parsing arguments.
     */
    void finish_initialization(CommandData& cmd,
                               const ParserData& data,
                               ValueId start_id = {},
                               ArgumentId argument_id = {});

    struct ParserSettings;

    bool has_flag(const CommandData& cmd,
                  std::string_view flag,
                  const ParserSettings& settings);
}
