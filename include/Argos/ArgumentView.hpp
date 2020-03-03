//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "IArgumentView.hpp"

namespace Argos
{
    struct ArgumentData;

    class ArgumentView : public IArgumentView
    {
    public:
        explicit ArgumentView(const ArgumentData* data);

        const std::string& text() const final;

        const std::string& section() const final;

        const std::string& valueName() const final;

        Visibility visibility() const final;

        int id() const final;

        int valueId() const final;

        const std::string& name() const;

        bool optional() const;

        std::pair<unsigned, unsigned> count() const;
    private:
        const ArgumentData* m_Argument;
    };
}
