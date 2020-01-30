//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "OptionView.hpp"
#include "ParsedArgumentsBuilder.hpp"

namespace Argos
{
    using OptionCallback = std::function<bool(OptionView, std::string_view, ParsedArgumentsBuilder)>;

    class Option
    {
    public:
        Option();

        explicit Option(std::vector<std::string> flags);

        Option(const Option&) = delete;

        Option(Option&&) noexcept;

        ~Option();

        Option& operator=(const Option&) = delete;

        Option& operator=(Option&&) noexcept;

        Option& text(const std::string& text);

        Option& section(const std::string& name);

        Option& valueName(const std::string& id);

        Option& operation(ArgumentOperation operation);

        Option& hidden(bool hidden);

        Option& id(int id);

        Option& flag(const std::string& f);

        Option& flags(std::vector<std::string> f);

        Option& argument(const std::string& name);

        Option& value(const std::string& value);

        Option& value(bool value);

        Option& value(int value);

        Option& value(double value);

        Option& type(OptionType type);

        Option& mandatory(bool mandatory);

        std::shared_ptr<OptionData> release();
    private:
        std::shared_ptr<OptionData> m_Option;
    };
}
