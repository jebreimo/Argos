//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Callbacks.hpp"
#include "Enums.hpp"

namespace Argos
{
    class OptionData;

    class Option
    {
    public:
        Option();

        explicit Option(std::vector<std::string> flags);

        Option(const Option&);

        Option(Option&&) noexcept;

        ~Option();

        Option& operator=(const Option&);

        Option& operator=(Option&&) noexcept;

        Option& text(const std::string& text);

        Option& section(const std::string& name);

        Option& valueName(const std::string& id);

        Option& callback(OptionCallback callback);

        Option& operation(OptionOperation operation);

        Option& visibility(Visibility visibility);

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

        const OptionData& data() const;

        std::unique_ptr<OptionData> release();
    private:
        std::unique_ptr<OptionData> m_Option;
    };
}
