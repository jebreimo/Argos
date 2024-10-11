//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argument.hpp"
#include "Option.hpp"

namespace argos
{
    struct CommandData;

    /**
     * @brief Represents a command or sub-command that can be parsed by
     *  ArgumentParser.
     *
     * Commands can have arguments, options, and sub-commands.
     *
     * An example of a program that uses commands are for instance git:
     * where "git" is the main command and "commit", "push", "pull", etc.
     * are sub-commands.
     */
    class Command
    {
    public:
        /**
         * @brief Creates a new unnamed command.
         *
         * Unnamed commands can not be added to an ArgumentParser, but they
         * can be used to hold sets of common arguments and options and even
         * sub-commands that can be added to actual sub-commands with the
         * copy_from method.
         */
        Command();

        /**
         * @brief Creates a new command with the given name.
         */
        explicit Command(std::string name);

        /**
         * @brief Creates a complete copy of the given command.
         */
        Command(const Command&);

        /**
         * @brief Moves the innards of the given command to the new object.
         *
         * Attempts to use the old object will result in an exception.
         */
        Command(Command&&) noexcept;

        ~Command();

        /**
         * @brief Copies everything in the given command to this object.
         */
        Command& operator=(const Command&);

        /**
         * @brief Moves the innards of the given command to this object.
         *
         * Attempts to use the old object will result in an exception.
         */
        Command& operator=(Command&&) noexcept;

        /**
         * @brief Adds an argument to the command.
         *
         * @a argument will be moved from, and can not be used
         * afterwards. To avoid this, create a copy of @a argument and
         * add that to the command instead.
         */
        Command& add(Argument& argument);

        /**
         * @brief Adds an argument to the command.
         *
         * @a argument will be moved from, and can not be used afterwards.
         */
        Command& add(Argument&& argument);

        /**
         * @brief Adds an option to the command.
         *
         * @a option will be moved from, and can not be used afterwards.
         *
         * To avoid this, create a copy of @a option and
         * add that to the command instead.
         */
        Command& add(Option& option);

        /**
         * @brief Adds an option to the command.
         *
         * @a option will be moved from, and can not be used afterwards.
         */
        Command& add(Option&& option);

        /**
         * @brief Adds a sub-command to the command.
         *
         * @a command will be moved from, and can not be used afterwards.
         */
        Command& add(Command& command);

        /**
         * @brief Adds a sub-command to the command.
         *
         * @a command will be moved from, and can not be used afterwards.
         */
        Command& add(Command&& command);

        /**
         * @brief Set the name of the command.
         *
         * The name is used to identify the command in the help text and
         * when retrieving the command's values from ParsedArguments.
         */
        Command& name(std::string name);

        /**
         * @brief Set the help text for the command.
         *
         * The help text is displayed in the auto-generated help text of the
         * parent command. It is also displayed in the command's own help text
         * if about() has not been set.
         */
        Command& help(std::string text);

        Command& about(std::string text);

        Command& section(const std::string& name);

        Command& text(TextId textId, std::string text);

        Command& text(TextId textId, std::function<std::string()> callback);

        /**
         * @brief Set restrictions for where this command is displayed in the
         *      auto-generated help text.
         * @param visibility
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Command& visibility(Visibility visibility);

        /**
         * @brief Set a custom id that can be used in callback functions etc.
         *      to quickly distinguish between different commands.
         *
         * The id purely is intended for client code, Argos itself ignores
         * this value.
         *
         * @param id Can be any integer value.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Command& id(int id);

        Command& allow_multiple_subcommands(bool multi_command);

        /**
         * @brief Add copies of all arguments, options and sub-commands in
         *  @a command.
         */
        Command& copy_from(Command& command);

        std::unique_ptr<CommandData> release();

    private:
        friend class ArgumentParser;

        void check_command() const;

        const CommandData& internal_ref() const;

        std::unique_ptr<CommandData> data_;
    };
}
