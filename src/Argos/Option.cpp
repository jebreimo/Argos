//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/Option.hpp"

#include "Argos/ArgosException.hpp"
#include "OptionData.hpp"

#define CHECK_OPTION_EXISTS() \
    if (!m_Option) \
        ARGOS_THROW("Cannot use Option instance after release() has been called.")

namespace Argos
{
    Option::Option()
        : m_Option(std::make_unique<OptionData>())
    {}

    Option::Option(const Option& rhs)
        : m_Option(rhs.m_Option
                   ? std::make_unique<OptionData>(*rhs.m_Option)
                   : std::unique_ptr<OptionData>())
    {}

    Option::Option(std::vector<std::string> flags)
        : m_Option(std::make_unique<OptionData>())
    {
        m_Option->flags = move(flags);
    }

    Option::Option(Option&& rhs) noexcept
        : m_Option(std::move(rhs.m_Option))
    {}

    Option::~Option() = default;

    Option& Option::operator=(const Option& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_Option)
                m_Option = std::make_unique<OptionData>(*rhs.m_Option);
            else
                m_Option = {};
        }
        return *this;
    }

    Option& Option::operator=(Option&& rhs) noexcept
    {
        m_Option = std::move(rhs.m_Option);
        return *this;
    }

    Option& Option::text(const std::string& text)
    {
        CHECK_OPTION_EXISTS();
        m_Option->text = text;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        CHECK_OPTION_EXISTS();
        m_Option->section = name;
        return *this;
    }

    Option& Option::valueName(const std::string& id)
    {
        CHECK_OPTION_EXISTS();
        m_Option->valueName = id;
        return *this;
    }

    Option& Option::operation(ArgumentOperation operation)
    {
        CHECK_OPTION_EXISTS();
        m_Option->operation = operation;
        return *this;
    }

    Option& Option::hidden(bool hidden)
    {
        CHECK_OPTION_EXISTS();
        m_Option->hidden = hidden;
        return *this;
    }

    Option& Option::id(int id)
    {
        CHECK_OPTION_EXISTS();
        m_Option->id = id;
        return *this;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        CHECK_OPTION_EXISTS();
        m_Option->flags = std::move(f);
        return *this;
    }

    Option& Option::argument(const std::string& name)
    {
        CHECK_OPTION_EXISTS();
        m_Option->argument = name;
        return *this;
    }

    Option& Option::value(const std::string& value)
    {
        CHECK_OPTION_EXISTS();
        m_Option->value = value;
        return *this;
    }

    Option& Option::value(bool value)
    {
        return this->value(value ? 1 : 0);
    }

    Option& Option::value(int value)
    {
        CHECK_OPTION_EXISTS();
        m_Option->value = std::to_string(value);
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        CHECK_OPTION_EXISTS();
        m_Option->optionType = type;
        return *this;
    }

    Option& Option::mandatory(bool mandatory)
    {
        CHECK_OPTION_EXISTS();
        m_Option->mandatory = mandatory;
        return *this;
    }

    const OptionData& Option::data() const
    {
        CHECK_OPTION_EXISTS();
        return *m_Option;
    }

    std::unique_ptr<OptionData> Option::release()
    {
        CHECK_OPTION_EXISTS();
        return std::move(m_Option);
    }
}
