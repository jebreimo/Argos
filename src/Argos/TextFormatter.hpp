//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace Argos
{
    class TextFormatter
    {
    public:
        TextFormatter(size_t indent, size_t width);

        explicit TextFormatter(std::ostream* stream, size_t indent, size_t width);

        void pushIndentation(size_t indent, size_t width = 0);

        void popIndentation();

        void writeText(std::string_view text);

        void newline();

        void flush();
    private:
        void indent();

        void appendWord(std::string_view word);

        std::ostream* m_Stream;
        std::vector<std::pair<size_t, size_t>> m_Indents;
        std::string m_Line;
    };
}
