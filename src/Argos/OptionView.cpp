//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/OptionView.hpp"

#include "ArgosThrow.hpp"
#include "OptionData.hpp"

namespace argos
{
    OptionView::OptionView(const OptionData* data)
        : m_option(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    std::string OptionView::help() const
    {
        return get_text(m_option->help);
    }

    const std::string& OptionView::section() const
    {
        return m_option->section;
    }

    const std::string& OptionView::value() const
    {
        return m_option->alias;
    }

    OptionOperation OptionView::operation() const
    {
        return m_option->operation;
    }

    Visibility OptionView::visibility() const
    {
        return m_option->visibility;
    }

    int OptionView::id() const
    {
        return m_option->id;
    }

    ValueId OptionView::value_id() const
    {
        return m_option->value_id;
    }

    const std::vector<std::string>& OptionView::flags() const
    {
        return m_option->flags;
    }

    const std::string& OptionView::argument() const
    {
        return m_option->argument;
    }

    const std::string& OptionView::initial_value() const
    {
        return m_option->initial_value;
    }

    const std::string& OptionView::constant() const
    {
        return m_option->constant;
    }

    OptionType OptionView::type() const
    {
        return m_option->type;
    }

    bool OptionView::optional() const
    {
        return m_option->optional;
    }

    ArgumentId OptionView::argument_id() const
    {
        return m_option->argument_id;
    }
}
