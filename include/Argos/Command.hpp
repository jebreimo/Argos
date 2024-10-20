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
         *
         * @note A command cannot have both arguments and sub-commands.
         *
         * @throw ArgosException if @a argument has been moved-from,
         *  doesn't have a name, or sub-commands have already been added.
         */
        Command& add(Argument& argument);

        /**
         * @brief Adds an argument to the command.
         *
         * @a argument will be moved from, and can not be used afterwards.
         *
         * @note A command cannot have both arguments and sub-commands.
         *
         * @throw ArgosException if @a argument has been moved-from,
         *  doesn't have a name, or sub-commands have already been added.
         */
        Command& add(Argument&& argument);

        /**
         * @brief Adds an option to the command.
         *
         * @a option will be moved from, and can not be used afterwards.
         * To avoid this, create a copy of @a option and add that to the
         * command instead.
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
         *
         * @note A command cannot have both arguments and sub-commands.
         *
         * @throw ArgosException if @a command has been moved-from,
         *  doesn't have a name, or arguments have already been added.
         */
        Command& add(Command& command);

        /**
         * @brief Adds a sub-command to the command.
         *
         * @a command will be moved from, and can not be used afterwards.
         *
         * @note A command cannot have both arguments and sub-commands.
         *
         * @throw ArgosException if @a command has been moved-from,
         *  doesn't have a name, or arguments have already been added.
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

        /**
         * @brief Set the about text for the command.
         *
         * The about text is displayed in the command's own help text between
         * the USAGE section and the sections describing the command's
         * sub-commands, arguments, and options.
         */
        Command& about(std::string text);

        /**
         * @brief Sets the heading that the command will be listed under in
         *      the parent command's help text.
         *
         * The default heading for commands is "COMMANDS".
         * @param name All arguments, options and commands that share the same
         *      section name will be listed under the same heading.
         * @return Reference to itself. This makes it possible to chain
         *      method calls.
         */
        Command& section(std::string name);

        /**
         * @brief Sets a section (or heading) that is automatically assigned
         *   to arguments, sub-commands and options when they are added.
         *
         * This value only applies to arguments, sub-commands and options that
         * have not been assigned a section with Argument::section or Option::section.
         * If this value is an empty string, the values from
         * TextId::ARGUMENTS_TITLE, TextId::SUBCOMMANDS_TITLE and
         * TextId::OPTIONS_TITLE are used.
         *
         * @param name All arguments, sub-commands and options with the same
         *  section name will be listed under the same heading.
         */
        Command& current_section(std::string name);

        /**
         * @brief Set the given part of the help text.
         *
         * With this function it is possible to override otherwise
         * auto-generated parts of the text, e.g. TextId::USAGE, or
         * add additional text, e.g. TextId::INITIAL_TEXT and
         * TextId::FINAL_TEXT.
         */
        Command& text(TextId textId, std::string text);

        /**
         * @brief Set a function that will produce the given part of
         *  the help text.
         *
         * With this function it is possible to override otherwise
         * auto-generated parts of the text, e.g. TextId::USAGE, or
         * add additional text, e.g. TextId::INITIAL_TEXT and
         * TextId::FINAL_TEXT.
         */
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

        /**
         * @brief Set whether the command can accept multiple sub-commands.
         *
         * If this property is true, a sequence of sub-commands can be given.
         * Each sub-command can be followed by a new one when it has been
         * given all the arguments it requires.
         */
        Command& allow_multiple_subcommands(bool multi_command);

        /**
         * @brief Set whether the commands requires one or more sub-commands.
         *
         * If this property is true, the command requires that any options
         * or arguments to the main program is followed by a sub-command,
         * and will exit with an error message if not.
         *
         * This property is only relevant if the command has sub-commands,
         * and it is automatically set to true if it is unassigned and the
         * command has sub-commands, but no arguments.
         */
        Command& require_subcommand(bool value);

        /**
         * @brief Add copies of all arguments, options and sub-commands in
         *  @a command.
         *
         * Any texts (help, about, etc.) set in @a command will be copied
         * as well.
         *
         * @throw ArgosException if this command already has any of the texts
         *  in @a command.
         */
        Command& copy_from(Command& command);

        /**
         * @private
         * @brief Used internally in Argos.
         *
         * The object is no longer usable after this function has
         * been called.
         * @return Pointer to the argument implementation.
         */
        std::unique_ptr<CommandData> release();

    private:
        friend class ArgumentParser;

        void check_command() const;

        const CommandData& internal_ref() const;

        std::unique_ptr<CommandData> data_;
    };

    /**
     * @brief A convenient short alias for Command.
     */
    using Cmd = Command;
}
