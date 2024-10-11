//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/Command.hpp"

#include "ArgosThrow.hpp"
#include "CommandData.hpp"

namespace argos
{
    Command::Command()
        : data_(std::make_unique<CommandData>())
    {}

    Command::Command(std::string name)
        : data_(std::make_unique<CommandData>())
    {
        data_->name = std::move(name);
    }

    Command::Command(const Command& rhs)
        : data_(rhs.data_ ? std::make_unique<CommandData>(*rhs.data_) : nullptr)
    {
    }

    Command::Command(Command&& rhs) noexcept
        : data_(std::move(rhs.data_))
    {
    }

    Command::~Command() = default;

    Command& Command::operator=(const Command& rhs)
    {
        if (this != &rhs)
        {
            data_ = rhs.data_
                        ? std::make_unique<CommandData>(*rhs.data_)
                        : nullptr;
        }
        return *this;
    }

    Command& Command::operator=(Command&& rhs) noexcept
    {
        data_ = std::move(rhs.data_);
        return *this;
    }

    Command& Command::add(Argument& argument)
    {
        return add(std::move(argument));
    }

    Command& Command::add(Argument&& argument)
    {
        check_command();
        data_->add(argument.release());
        return *this;
    }

    Command& Command::add(Option& option)
    {
        return add(std::move(option));
    }

    Command& Command::add(Option&& option)
    {
        check_command();
        data_->add(option.release());
        return *this;
    }

    Command& Command::add(Command& command)
    {
        return add(std::move(command));
    }

    Command& Command::add(Command&& command)
    {
        check_command();
        data_->add(command.release());
        return *this;
    }

    Command& Command::name(std::string name)
    {
        check_command();
        data_->name = std::move(name);
        return *this;
    }

    Command& Command::about(std::string text)
    {
        check_command();
        data_->texts[TextId::ABOUT] = std::move(text);
        return *this;
    }

    Command& Command::section(const std::string& name)
    {
        check_command();
        data_->current_section = name;
        return *this;
    }

    Command& Command::text(TextId textId, std::string text)
    {
        check_command();
        data_->texts[textId] = std::move(text);
        return *this;
    }

    Command& Command::text(TextId textId, std::function<std::string()> callback)
    {
        check_command();
        data_->texts[textId] = std::move(callback);
        return *this;
    }

    Command& Command::visibility(Visibility visibility)
    {
        check_command();
        data_->visibility = visibility;
        return *this;
    }

    Command& Command::id(int id)
    {
        check_command();
        data_->id = id;
        return *this;
    }

    Command& Command::multi_command(bool multi_command)
    {
        check_command();
        data_->multi_command = multi_command;
        return *this;
    }

    Command& Command::copy_from(Command& command)
    {
        check_command();
        data_->copy_from(*command.data_);
        return *this;
    }

    std::unique_ptr<CommandData> Command::release()
    {
        return std::move(data_);
    }

    const CommandData& Command::internal_ref() const
    {
        check_command();
        return *data_;
    }

    void Command::check_command() const
    {
        if (!data_)
            ARGOS_THROW("Command has been moved.");
    }
}
