//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IArgumentView.hpp"

namespace Argos
{
    class ParsedArgumentsImpl;

    class ArgumentValues
    {
    public:
        ArgumentValues(std::vector<std::string_view> values,
                       std::shared_ptr<ParsedArgumentsImpl> args,
                       int valueId);

        ArgumentValues(const ArgumentValues&);

        ArgumentValues(ArgumentValues&&) noexcept;

        ~ArgumentValues();

        ArgumentValues& operator=(const ArgumentValues&);

        ArgumentValues& operator=(ArgumentValues&&) noexcept;

        std::vector<std::unique_ptr<IArgumentView>> arguments() const;

        void error(const std::string& message) const;

        bool empty() const;

        size_t size() const;

        const std::vector<std::string_view>& values() const;

        std::vector<int8_t> asInt8s(
                const std::vector<int8_t>& defaultValue = {}) const;

        std::vector<int16_t> asInt16s(
                const std::vector<int16_t>& defaultValue = {}) const;

        std::vector<int32_t> asInt32s(
                const std::vector<int32_t>& defaultValue = {}) const;

        std::vector<int64_t> asInt64s(
                const std::vector<int64_t>& defaultValue = {}) const;

        std::vector<uint8_t> asUint8s(
                const std::vector<uint8_t>& defaultValue = {}) const;

        std::vector<uint16_t> asUint16s(
                const std::vector<uint16_t>& defaultValue = {}) const;

        std::vector<uint32_t> asUint32s(
                const std::vector<uint32_t>& defaultValue = {}) const;

        std::vector<uint64_t> asUint64s(
                const std::vector<uint64_t>& defaultValue = {}) const;

        std::vector<float> asFloats(
                const std::vector<float>& defaultValue = {}) const;

        std::vector<double> asDoubles(
                const std::vector<double>& defaultValue = {}) const;

        std::vector<long double> asLongDoubles(
                const std::vector<long double>& defaultValue = {}) const;

        std::vector<std::string> asStrings(
                const std::vector<std::string>& defaultValue = {}) const;
    private:
        template <typename T>
        std::vector<T> getValues(const std::vector<T>& defaultValue) const;

        std::vector<std::string_view> m_Values;
        std::shared_ptr<ParsedArgumentsImpl> m_Args;
        int m_ValueId;
    };
}
