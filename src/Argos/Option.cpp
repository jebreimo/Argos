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

namespace Argos
{
    Option::Option()
        : m_Option(std::make_unique<OptionData>())
    {}

    Option::Option(std::initializer_list<std::string> flags)
        : m_Option(std::make_unique<OptionData>())
    {
        m_Option->flags = flags;
    }

    Option::Option(const Option& rhs)
        : m_Option(rhs.m_Option
                   ? std::make_unique<OptionData>(*rhs.m_Option)
                   : std::unique_ptr<OptionData>())
    {}

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
        checkOption();
        m_Option->text = text;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        checkOption();
        m_Option->section = name;
        return *this;
    }

    Option& Option::value(const std::string& id)
    {
        checkOption();
        m_Option->value = id;
        return *this;
    }

    Option& Option::operation(OptionOperation operation)
    {
        checkOption();
        m_Option->operation = operation;
        return *this;
    }

    Option& Option::visibility(Visibility visibility)
    {
        checkOption();
        m_Option->visibility = visibility;
        return *this;
    }

    Option& Option::id(int id)
    {
        checkOption();
        m_Option->id = id;
        return *this;
    }

    Option& Option::flag(const std::string& f)
    {
        checkOption();
        m_Option->flags = {f};
        return *this;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        checkOption();
        m_Option->flags = std::move(f);
        return *this;
    }

    Option& Option::argument(const std::string& name)
    {
        checkOption();
        m_Option->argument = name;
        return *this;
    }

    Option& Option::constant(const char* value)
    {
        return this->constant(std::string(value));
    }

    Option& Option::constant(const std::string& value)
    {
        checkOption();
        m_Option->constant = value;
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
        checkOption();
        m_Option->constant = std::to_string(value);
        return *this;
    }

    Option& Option::callback(OptionCallback callback)
    {
        checkOption();
        m_Option->callback = move(callback);
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        checkOption();
        m_Option->type = type;
        return *this;
    }

    Option& Option::optional(bool optional)
    {
        checkOption();
        m_Option->optional = optional;
        return *this;
    }

    const OptionData& Option::data() const
    {
        checkOption();
        return *m_Option;
    }

    std::unique_ptr<OptionData> Option::release()
    {
        checkOption();
        return std::move(m_Option);
    }

    void Option::checkOption() const
    {
        if (!m_Option)
            ARGOS_THROW("Cannot use Option instance after"
                        " release() has been called.");
    }
}
