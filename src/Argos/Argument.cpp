//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/Argument.hpp"

#include "ArgosThrow.hpp"
#include "ArgumentData.hpp"

namespace argos
{
    Argument::Argument()
        : m_argument(std::make_unique<ArgumentData>())
    {}

    Argument::Argument(const std::string& name)
        : m_argument(std::make_unique<ArgumentData>())
    {
        m_argument->name = name;
    }

    Argument::Argument(const Argument& rhs)
        : m_argument(rhs.m_argument
                     ? std::make_unique<ArgumentData>(*rhs.m_argument)
                     : std::unique_ptr<ArgumentData>())
    {}

    Argument::Argument(Argument&& rhs) noexcept
        : m_argument(std::move(rhs.m_argument))
    {}

    Argument::~Argument() = default;

    Argument& Argument::operator=(const Argument& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_argument)
                m_argument = std::make_unique<ArgumentData>(*rhs.m_argument);
            else
                m_argument = {};
        }
        return *this;
    }

    Argument& Argument::operator=(Argument&& rhs) noexcept
    {
        m_argument = std::move(rhs.m_argument);
        return *this;
    }

    Argument& Argument::help(const std::string& text)
    {
        check_argument();
        m_argument->help = text;
        return *this;
    }

    Argument& Argument::help(TextCallback callback)
    {
        check_argument();
        m_argument->help = callback;
        return *this;
    }

    Argument& Argument::section(const std::string& name)
    {
        check_argument();
        m_argument->section = name;
        return *this;
    }

    Argument& Argument::alias(const std::string& id)
    {
        check_argument();
        m_argument->value = id;
        return *this;
    }

    Argument& Argument::callback(ArgumentCallback callback)
    {
        check_argument();
        m_argument->callback = std::move(callback);
        return *this;
    }

    Argument& Argument::visibility(Visibility visibility)
    {
        check_argument();
        m_argument->visibility = visibility;
        return *this;
    }

    Argument& Argument::id(int id)
    {
        check_argument();
        m_argument->id = id;
        return *this;
    }

    Argument& Argument::name(const std::string& name)
    {
        check_argument();
        m_argument->name = name;
        return *this;
    }

    Argument& Argument::optional(bool optional)
    {
        check_argument();
        if (optional)
            m_argument->min_count = 0;
        else if (m_argument->min_count == 0)
            m_argument->min_count = 1;
        return *this;
    }

    Argument& Argument::mandatory(bool mandatory)
    {
        return optional(!mandatory);
    }

    Argument& Argument::count(unsigned n)
    {
        if (n <= 0)
            ARGOS_THROW("Argument's count must be greater than 0.");
        check_argument();
        m_argument->min_count = m_argument->max_count = n;
        return *this;
    }

    Argument& Argument::count(unsigned min_count, unsigned max_count)
    {
        if (max_count == 0)
            ARGOS_THROW("Argument's max count must be greater than 0.");
        if (max_count < min_count)
            ARGOS_THROW("Argument's max count cannot be less than its min count.");
        check_argument();
        m_argument->min_count = min_count;
        m_argument->max_count = max_count;
        return *this;
    }

    std::unique_ptr<ArgumentData> Argument::release()
    {
        check_argument();
        return std::move(m_argument);
    }

    void Argument::check_argument() const
    {
        if (!m_argument)
            ARGOS_THROW("Argument has been moved.");
    }
}
