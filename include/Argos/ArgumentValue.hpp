//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "IArgumentView.hpp"

namespace Argos
{
    class ParserData;

    class ArgumentValue
    {
    public:
        ArgumentValue(std::optional<std::string_view> value,
                      std::shared_ptr<ParserData> data,
                      int valueId);

        ArgumentValue(const ArgumentValue&);

        ArgumentValue(ArgumentValue&&) noexcept;

        ~ArgumentValue();

        ArgumentValue& operator=(const ArgumentValue&);

        ArgumentValue& operator=(ArgumentValue&&) noexcept;

        std::unique_ptr<IArgumentView> argument() const;

        bool hasValue() const;

        std::optional<std::string_view> value() const;

        std::string stringValue(const std::string& defaultValue) const;

        void error(const std::string& message) const;
    private:
        std::optional<std::string_view> m_Value;
        std::shared_ptr<ParserData> m_Data;
        int m_ValueId;
    };

    class ArgumentValues
    {
    public:
        ArgumentValues(std::shared_ptr<ParserData> data, int valueId);

        ArgumentValues(const ArgumentValues&);

        ArgumentValues(ArgumentValues&&) noexcept;

        ~ArgumentValues();

        ArgumentValues& operator=(const ArgumentValues&);

        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        std::unique_ptr<IArgumentView> argument() const;

        void error(const std::string& message) const;
    private:
        std::vector<std::string_view> m_Values;
        std::shared_ptr<ParserData> m_Data;
        int m_ValueId;
    };
}
