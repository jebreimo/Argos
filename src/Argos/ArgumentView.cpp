//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentView.hpp"

#include "ArgosThrow.hpp"
#include "ArgumentData.hpp"

namespace argos
{
    ArgumentView::ArgumentView(const ArgumentData* data)
            : m_argument(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    std::string ArgumentView::help() const
    {
        return get_text(m_argument->help);
    }

    const std::string& ArgumentView::section() const
    {
        return m_argument->section;
    }

    const std::string& ArgumentView::alias() const
    {
        return m_argument->alias;
    }

    Visibility ArgumentView::visibility() const
    {
        return m_argument->visibility;
    }

    int ArgumentView::id() const
    {
        return m_argument->id;
    }

    const std::string& ArgumentView::name() const
    {
        return m_argument->name;
    }

    bool ArgumentView::optional() const
    {
        return m_argument->min_count == 0;
    }

    std::pair<unsigned, unsigned> ArgumentView::count() const
    {
        return {m_argument->min_count, m_argument->max_count};
    }

    ValueId ArgumentView::value_id() const
    {
        return m_argument->value_id;
    }

    ArgumentId ArgumentView::argument_id() const
    {
        return m_argument->argument_id;
    }
}
