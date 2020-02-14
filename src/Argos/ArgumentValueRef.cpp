//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentValueRef.hpp"

#include "ParserData.hpp"
#include "HelpWriter.hpp"

namespace Argos
{
    ArgumentValueRef::ArgumentValueRef(std::shared_ptr <ParserData> data, int valueId)
        : m_Data(std::move(data)),
          m_ValueId(valueId)
    {}

    ArgumentValueRef::ArgumentValueRef(const ArgumentValueRef&) = default;

    ArgumentValueRef::ArgumentValueRef(ArgumentValueRef&& ) noexcept = default;

    ArgumentValueRef::~ArgumentValueRef() = default;

    ArgumentValueRef& ArgumentValueRef::operator=(const ArgumentValueRef&) = default;

    ArgumentValueRef& ArgumentValueRef::operator=(ArgumentValueRef&&) noexcept = default;

    void ArgumentValueRef::error(const std::string& message) const
    {
        // TODO: find argument or option and pass it to HelpWriter.
        HelpWriter(m_Data).writeErrorMessage(message);
        if (m_Data->parserSettings.autoExit)
            exit(1);
    }
}
