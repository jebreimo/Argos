//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "IArgument.hpp"

#include <memory>
#include <vector>
#include "OptionType.hpp"

namespace Argos
{
    class OptionData;

    class Option : public IArgument
    {
    public:
        Option();

        explicit Option(std::vector<std::string> flags);

        Option(const Option&) = delete;

        Option(Option&&) noexcept;

        ~Option() override;

        Option& operator=(const Option&) = delete;

        Option& operator=(Option&&) noexcept;

        const std::string& text() const override;

        Option& text(const std::string& text);

        const std::string& section() const override;

        Option& section(const std::string& name);

        const std::string& valueName() const override;

        Option& valueName(const std::string& id);

        ArgumentOperation operation() const override;

        Option& operation(ArgumentOperation operation);

        bool hidden() const override;

        Option& hidden(bool hidden);

        int id() const override;

        Option& id(int id);

        Option& flag(const std::string& f);

        const std::vector<std::string>& flags() const;

        Option& flags(std::vector<std::string> f);

        const std::string& argument() const;

        Option& argument(const std::string& name);

        const std::string& value() const;

        Option& value(const std::string& value);

        OptionType type() const;

        Option& type(OptionType type);

        bool mandatory() const;

        Option& mandatory(bool mandatory);

        std::shared_ptr<OptionData> release();
    private:
        std::shared_ptr<OptionData> m_Option;
    };
}
