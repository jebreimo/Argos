//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "ParserData.hpp"

namespace argos
{
    void write_help_text(const ParserData& data, const CommandData& cmd);

    void write_error_message(const ParserData& data,
                             const CommandData& cmd,
                             const std::string& msg);

    void write_error_message(const ParserData& data,
                             const CommandData& cmd,
                             const std::string& msg,
                             ArgumentId argument_id);
}
