//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <string>

namespace Argos
{
    class ParserData;

    template <typename T>
    class ParsedArgumentValue
    {
    public:

    private:
        std::shared_ptr<ParserData> m_ParserData;
        T m_Value;
        int m_ValueId;
    };

    class RawArgumentValue
    {
    public:
        RawArgumentValue(std::shared_ptr<ParserData> parser, std::string_view value);

        bool hasValue() const;

        //template <typename T>
        //ParsedArgumentValue<T> parse()
        //{
        //
        //}
    private:
        std::shared_ptr<ParserData> m_ParserData;
        std::optional<std::string> m_Value;
        int m_ValueId;
    };
}
