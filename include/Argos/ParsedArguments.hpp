//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include "ArgumentValue.hpp"
#include "OptionView.hpp"
#include "RawArgumentValue.hpp"

namespace Argos
{
    class ParsedArgumentsImpl;

    class ParsedArguments
    {
    public:
        ParsedArguments();

        ParsedArguments(std::unique_ptr<ParsedArgumentsImpl> impl);

        ParsedArguments(const ParsedArguments&) = delete;

        ParsedArguments(ParsedArguments&&) noexcept;

        ~ParsedArguments();

        ParsedArguments& operator=(const ParsedArguments&) = delete;

        ParsedArguments& operator=(ParsedArguments&&) noexcept;

        bool has(const std::string& name) const;

        bool getBool(const std::string& name,
                     bool defaultValue = false) const;

        int32_t getInt32(const std::string& name,
                         int32_t defaultValue = {}) const;

        int64_t getInt64(const std::string& name,
                         int64_t defaultValue = {}) const;

        double getDouble(const std::string& name,
                         double defaultValue = {}) const;

        std::string getString(const std::string& name,
                              const std::string& defaultValue = {}) const;

        std::vector<std::string>
        getStrings(const std::string& name,
                   const std::vector<std::string>& defaultValue = {}) const;

        ArgumentValue value(const std::string& name) const;

        ArgumentValues values(const std::string& name) const;
        //
        //std::pair<std::vector<std::string>, ArgumentValue>
        //getVector(const std::string& name,
        //          const std::vector<std::string>& defaultValue = {}) const;

        ParserResultCode resultCode() const;

        OptionView specialOption() const;
    private:
        template <typename T>
        T getValue(const std::string& name, const T& defaultValue) const;

        std::unique_ptr<ParsedArgumentsImpl> m_Impl;
    };
}
