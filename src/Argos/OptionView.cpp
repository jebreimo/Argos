//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/OptionView.hpp"

#include "OptionData.hpp"

namespace Argos
{
    OptionView::OptionView(const OptionData* data)
        : m_Option(data)
    {}

    const std::string& OptionView::text() const
    {
        return m_Option->text;
    }

    const std::string& OptionView::section() const
    {
        return m_Option->section;
    }

    const std::string& OptionView::valueName() const
    {
        return m_Option->valueName;
    }

    OptionOperation OptionView::operation() const
    {
        return m_Option->operation;
    }

    Visibility OptionView::visibility() const
    {
        return m_Option->visibility;
    }

    int OptionView::id() const
    {
        return m_Option->id;
    }

    ValueId OptionView::valueId() const
    {
        return m_Option->valueId;
    }

    const std::vector<std::string>& OptionView::flags() const
    {
        return m_Option->flags;
    }

    const std::string& OptionView::argument() const
    {
        return m_Option->argument;
    }

    const std::string& OptionView::value() const
    {
        return m_Option->value;
    }

    OptionType OptionView::type() const
    {
        return m_Option->type;
    }

    bool OptionView::mandatory() const
    {
        return m_Option->mandatory;
    }

    ArgumentId OptionView::argumentId() const
    {
        return m_Option->argumentId;
    }
}
