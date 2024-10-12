//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-09-21.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IArgumentView.hpp"

namespace argos
{
    struct CommandData;

    class CommandView : public IArgumentView
    {
    public:
        explicit CommandView(const CommandData* command);

        [[nodiscard]] std::string help() const override;

        [[nodiscard]] const std::string& section() const override;

        [[nodiscard]] const std::string& alias() const override;

        [[nodiscard]] Visibility visibility() const override;

        [[nodiscard]] int id() const override;

        [[nodiscard]] ValueId value_id() const override;

        [[nodiscard]] ArgumentId argument_id() const override;
    private:
        const CommandData* m_command;
    };
}
