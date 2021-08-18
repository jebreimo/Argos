//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentView.hpp"

#include "ArgosThrow.hpp"
#include "ArgumentData.hpp"

namespace Argos
{
    ArgumentView::ArgumentView(const ArgumentData* data)
            : m_Argument(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    const std::string& ArgumentView::help() const
    {
        return m_Argument->help;
    }

    const std::string& ArgumentView::section() const
    {
        return m_Argument->section;
    }

    const std::string& ArgumentView::value() const
    {
        return m_Argument->value;
    }

    Visibility ArgumentView::visibility() const
    {
        return m_Argument->visibility;
    }

    int ArgumentView::id() const
    {
        return m_Argument->id;
    }

    const std::string& ArgumentView::name() const
    {
        return m_Argument->name;
    }

    bool ArgumentView::optional() const
    {
        return m_Argument->minCount == 0;
    }

    std::pair<unsigned, unsigned> ArgumentView::count() const
    {
        return {m_Argument->minCount, m_Argument->maxCount};
    }

    ValueId ArgumentView::valueId() const
    {
        return m_Argument->valueId;
    }

    ArgumentId ArgumentView::argumentId() const
    {
        return m_Argument->argumentId;
    }
}
