//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/Option.hpp"

#include "ArgosThrow.hpp"
#include "OptionData.hpp"

namespace argos
{
    Option::Option()
        : m_option(std::make_unique<OptionData>())
    {}

    Option::Option(std::initializer_list<std::string> flags)
        : m_option(std::make_unique<OptionData>())
    {
        m_option->flags = flags;
    }

    Option::Option(const Option& rhs)
        : m_option(rhs.m_option
                   ? std::make_unique<OptionData>(*rhs.m_option)
                   : std::unique_ptr<OptionData>())
    {}

    Option::Option(Option&& rhs) noexcept
        : m_option(std::move(rhs.m_option))
    {}

    Option::~Option() = default;

    Option& Option::operator=(const Option& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_option)
                m_option = std::make_unique<OptionData>(*rhs.m_option);
            else
                m_option = {};
        }
        return *this;
    }

    Option& Option::operator=(Option&& rhs) noexcept
    {
        m_option = std::move(rhs.m_option);
        return *this;
    }

    Option& Option::help(const std::string& text)
    {
        check_option();
        m_option->help = text;
        return *this;
    }

    Option& Option::help(TextCallback callback)
    {
        check_option();
        m_option->help = callback;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        check_option();
        m_option->section = name;
        return *this;
    }

    Option& Option::alias(const std::string& id)
    {
        check_option();
        m_option->alias = id;
        return *this;
    }

    Option& Option::operation(OptionOperation operation)
    {
        check_option();
        m_option->operation = operation;
        return *this;
    }

    Option& Option::visibility(Visibility visibility)
    {
        check_option();
        m_option->visibility = visibility;
        return *this;
    }

    Option& Option::id(int id)
    {
        check_option();
        m_option->id = id;
        return *this;
    }

    Option& Option::flag(const std::string& f)
    {
        check_option();
        m_option->flags = {f};
        return *this;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        check_option();
        m_option->flags = std::move(f);
        return *this;
    }

    Option& Option::argument(const std::string& name)
    {
        check_option();
        m_option->argument = name;
        return *this;
    }

    Option& Option::initial_value(const std::string& value)
    {
        check_option();
        m_option->initial_value = value;
        return *this;
    }

    Option& Option::constant(const char* value)
    {
        return this->constant(std::string(value));
    }

    Option& Option::constant(const std::string& value)
    {
        check_option();
        m_option->constant = value;
        return *this;
    }

    Option& Option::constant(bool value)
    {
        return this->constant(value ? 1LL : 0LL);
    }

    Option& Option::constant(int value)
    {
        return this->constant(static_cast<long long>(value));
    }

    Option& Option::constant(long long value)
    {
        check_option();
        m_option->constant = std::to_string(value);
        return *this;
    }

    Option& Option::callback(OptionCallback callback)
    {
        check_option();
        m_option->callback = std::move(callback);
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        check_option();
        m_option->type = type;
        return *this;
    }

    Option& Option::optional(bool optional)
    {
        check_option();
        m_option->optional = optional;
        return *this;
    }

    Option& Option::mandatory(bool mandatory)
    {
        return optional(!mandatory);
    }

    const OptionData& Option::data() const
    {
        check_option();
        return *m_option;
    }

    std::unique_ptr<OptionData> Option::release()
    {
        check_option();
        return std::move(m_option);
    }

    void Option::check_option() const
    {
        if (!m_option)
            ARGOS_THROW("Cannot use Option instance after"
                        " release() has been called.");
    }
}
