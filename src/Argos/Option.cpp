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
        : m_Option(std::make_shared<OptionData>())
    {}

    Option::Option(std::vector<std::string> flags)
        : m_Option(std::make_shared<OptionData>())
    {
        m_Option->flags = move(flags);
    }

    Option::Option(Option&& rhs) noexcept
        : m_Option(std::move(rhs.m_Option))
    {}

    Option::~Option() = default;

    Option& Option::operator=(Option&& rhs) noexcept
    {
        m_Option = std::move(rhs.m_Option);
        return *this;
    }

    const std::string& Option::text() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->text;
    }

    Option& Option::text(const std::string& text)
    {
        CHECK_OPTION_EXISTS();
        m_Option->text = text;
        return *this;
    }

    const std::string& Option::section() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->section;
    }

    Option& Option::section(const std::string& name)
    {
        CHECK_OPTION_EXISTS();
        m_Option->section = name;
        return *this;
    }

    const std::string& Option::valueName() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->valueName;
    }

    Option& Option::valueName(const std::string& id)
    {
        CHECK_OPTION_EXISTS();
        m_Option->valueName = id;
        return *this;
    }

    ArgumentOperation Option::operation() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->operation;
    }

    Option& Option::operation(ArgumentOperation operation)
    {
        CHECK_OPTION_EXISTS();
        m_Option->operation = operation;
        return *this;
    }

    bool Option::hidden() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->hidden;
    }

    Option& Option::hidden(bool hidden)
    {
        CHECK_OPTION_EXISTS();
        m_Option->hidden = hidden;
        return *this;
    }

    int Option::id() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->id;
    }

    Option& Option::id(int id)
    {
        CHECK_OPTION_EXISTS();
        m_Option->id = id;
        return *this;
    }

    const std::vector<std::string>& Option::flags() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->flags;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        CHECK_OPTION_EXISTS();
        m_Option->flags = std::move(f);
        return *this;
    }

    const std::string& Option::argument() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->argument;
    }

    Option& Option::argument(const std::string& name)
    {
        CHECK_OPTION_EXISTS();
        m_Option->argument = name;
        return *this;
    }

    const std::string& Option::value() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->value;
    }

    Option& Option::value(const std::string& value)
    {
        CHECK_OPTION_EXISTS();
        m_Option->value = value;
        return *this;
    }

    OptionType Option::type() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->optionType;
    }

    Option& Option::type(OptionType type)
    {
        CHECK_OPTION_EXISTS();
        m_Option->optionType = type;
        return *this;
    }

    bool Option::mandatory() const
    {
        CHECK_OPTION_EXISTS();
        return m_Option->mandatory;
    }

    Option& Option::mandatory(bool mandatory)
    {
        CHECK_OPTION_EXISTS();
        m_Option->mandatory = mandatory;
        return *this;
    }

    std::shared_ptr<OptionData> Option::release()
    {
        CHECK_OPTION_EXISTS();
        return std::move(m_Option);
    }
}
