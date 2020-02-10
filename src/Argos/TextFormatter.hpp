//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <array>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>
#include "WordSplitter.hpp"

namespace Argos
{
    class TextFormatter
    {
    public:
        TextFormatter();

        TextFormatter(size_t indent, size_t width);

        explicit TextFormatter(std::ostream* stream, size_t indent, size_t lineWidth);

        std::ostream* stream() const;

        void setStream(std::ostream* stream);

        size_t lineWidth() const;

        void setLineWidth(size_t lineWidth);

        static constexpr size_t CURRENT_COLUMN = SIZE_MAX;

        void pushIndentation(size_t indent);

        void popIndentation();

        void writeText(std::string_view text);

        void writePreformattedText(std::string_view text);

        void newline();

        void flush();
    private:
        void indent();

        void appendWord(std::string_view word);

        std::ostream* m_Stream;
        std::vector<size_t> m_Indents;
        size_t m_LineWidth;
        std::string m_Line;
        WordSplitter m_WordSplitter;
    };
}
