//****************************************************************************
// Copyright © 2020 Jan Erik Breimo-> All rights reserved->
// Created by Jan Erik Breimo on 2020-01-07->
//
// This file is distributed under the BSD License->
// License text is included with the source distribution->
//****************************************************************************
#include "Argos/Argument.hpp"

#include "ArgumentData.hpp"
#include "Argos/ArgosException.hpp"

#define CHECK_ARGUMENT_EXISTS() \
    if (!m_Argument) \
        ARGOS_THROW("Cannot use Argument instance after release() has been called.")

namespace Argos
{
    Argument::Argument()
        : m_Argument(std::make_shared<ArgumentData>())
    {}

    Argument::Argument(const std::string& name)
        : m_Argument(std::make_shared<ArgumentData>())
    {
        m_Argument->name = name;
    }

    Argument::Argument(Argument&& rhs) noexcept
        : m_Argument(std::move(rhs.m_Argument))
    {}

    Argument::~Argument() = default;

    Argument& Argument::operator=(Argument&& rhs) noexcept
    {
        m_Argument = std::move(rhs.m_Argument);
        return *this;
    }

    Argument& Argument::text(const std::string& text)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->text = text;
        return *this;
    }

    Argument& Argument::section(const std::string& name)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->section = name;
        return *this;
    }

    Argument& Argument::valueName(const std::string& id)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->valueName = id;
        return *this;
    }

    Argument& Argument::operation(ArgumentOperation operation)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->operation = operation;
        return *this;
    }

    Argument& Argument::hidden(bool hidden)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->hidden = hidden;
        return *this;
    }

    Argument& Argument::id(int id)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->id = id;
        return *this;
    }

    Argument& Argument::name(const std::string& name)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->name = name;
        return *this;
    }

    Argument& Argument::optional(bool optional)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->minCount = optional ? 0 : 1;
        return *this;
    }

    Argument& Argument::count(unsigned n)
    {
        if (n <= 0)
            ARGOS_THROW("Argument's count must be greater than 0.");
        CHECK_ARGUMENT_EXISTS();
        m_Argument->minCount = m_Argument->maxCount = n;
        return *this;
    }

    Argument& Argument::count(unsigned minCount, unsigned maxCount)
    {
        if (maxCount == 0)
            ARGOS_THROW("Argument's max count cannot be 0.");
        if (maxCount < minCount)
            ARGOS_THROW("Argument's max count cannot be less than min count.");
        CHECK_ARGUMENT_EXISTS();
        m_Argument->minCount = minCount;
        m_Argument->maxCount = maxCount;
        return *this;
    }

    std::shared_ptr<ArgumentData> Argument::release()
    {
        CHECK_ARGUMENT_EXISTS();
        return std::move(m_Argument);
    }
}
