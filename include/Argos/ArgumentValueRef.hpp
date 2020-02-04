//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include <string>

namespace Argos
{
    class ParserData;

    class ArgumentValueRef
    {
    public:
        ArgumentValueRef(std::shared_ptr<ParserData> data, int valueId);

        ArgumentValueRef(const ArgumentValueRef&);

        ArgumentValueRef(ArgumentValueRef&&) noexcept;

        ~ArgumentValueRef();

        ArgumentValueRef& operator=(const ArgumentValueRef&);

        ArgumentValueRef& operator=(ArgumentValueRef&&) noexcept;

        void error(const std::string& message) const;
    private:
        std::shared_ptr<ParserData> m_Data;
        int m_ValueId;
    };
}
