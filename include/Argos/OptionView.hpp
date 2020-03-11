//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "IArgumentView.hpp"

#include <vector>

namespace Argos
{
    struct OptionData;

    class OptionView : public IArgumentView
    {
    public:
        explicit OptionView(const OptionData* data);

        const std::string& text() const final;

        const std::string& section() const final;

        const std::string& valueName() const final;

        Visibility visibility() const final;

        int id() const final;

        ValueId valueId() const final;

        ArgumentId argumentId() const final;

        OptionOperation operation() const;

        const std::vector<std::string>& flags() const;

        const std::string& argument() const;

        const std::string& value() const;

        OptionType type() const;

        bool mandatory() const;
    private:
        const OptionData* m_Option;
    };
}
