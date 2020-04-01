//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <climits>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>
#include "TextWriter.hpp"
#include "WordSplitter.hpp"

namespace Argos
{
    class TextFormatter
    {
    public:
        TextFormatter();

        explicit TextFormatter(std::ostream* stream);

        TextFormatter(std::ostream* stream, unsigned lineWidth);

        std::ostream* stream() const;

        void setStream(std::ostream* stream);

        unsigned lineWidth() const;

        void setLineWidth(unsigned lineWidth);

        unsigned currentLineWidth() const;

        bool isCurrentLineEmpty() const;

        static constexpr unsigned CURRENT_COLUMN = UINT_MAX;

        void pushIndentation(unsigned indent);

        void popIndentation();

        void writeText(std::string_view text);

        void writePreformattedText(std::string_view text);

        void newline();

        void flush();
    private:
        void appendWord(std::string_view word);

        TextWriter m_Writer;
        std::vector<unsigned> m_Indents;
        WordSplitter m_WordSplitter;
    };
}
