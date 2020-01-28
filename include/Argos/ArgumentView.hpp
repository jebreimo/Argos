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

    class ArgumentView : IArgumentView
    {
    public:
        explicit ArgumentView(const ArgumentData* data);

        const std::string& text() const override;

        const std::string& section() const override;

        const std::string& valueName() const override;

        ArgumentOperation operation() const override;

        bool hidden() const override;

        int id() const override;

        const std::string& name() const;

        bool optional() const;

        std::pair<unsigned, unsigned> count() const;

        std::shared_ptr<ArgumentData> release();
    private:
        const ArgumentData* m_Argument;
    };
}
