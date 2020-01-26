//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IArgument.hpp"

#include <memory>

namespace Argos
{
    struct ArgumentData;

    class Argument : public IArgument
    {
    public:
        Argument();

        explicit Argument(const std::string& name);

        Argument(const Argument&) = delete;

        Argument(Argument&&) noexcept;

        ~Argument() override;

        Argument& operator=(const Argument&) = delete;

        Argument& operator=(Argument&&) noexcept;

        const std::string& text() const override;

        Argument& text(const std::string& text);

        const std::string& section() const override;

        Argument& section(const std::string& name);

        const std::string& valueName() const override;

        Argument& valueName(const std::string& id);

        ArgumentOperation operation() const override;

        Argument& operation(ArgumentOperation operation);

        bool hidden() const override;

        Argument& hidden(bool hidden);

        int id() const override;

        Argument& id(int id);

        const std::string& name() const;

        Argument& name(const std::string& name);

        bool optional() const;

        Argument& optional(bool optional);

        std::pair<unsigned, unsigned> count() const;

        Argument& count(unsigned n);

        Argument& count(unsigned minCount, unsigned maxCount);

        std::shared_ptr<ArgumentData> release();
    private:
        std::shared_ptr<ArgumentData> m_Argument;
    };
}
