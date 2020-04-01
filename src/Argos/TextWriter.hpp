//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <string>
#include <string_view>

namespace Argos
{
    class TextWriter
    {
    public:
        explicit TextWriter(unsigned lineWidth = 80);

        std::ostream* stream() const;

        void setStream(std::ostream* stream);

        unsigned indentation() const;

        bool setIndentation(unsigned indent);

        bool write(std::string_view str, bool force = false);

        void newline();

        void flush();

        void tab();

        unsigned spaces() const;

        void setSpaces(unsigned n);

        unsigned currentWidth() const;

        unsigned remainingWidth() const;

        bool isCurrentLineEmpty() const;

        unsigned lineWidth() const;

        void setLineWidth(unsigned width);

        std::string_view currentLine() const;
    private:
        std::ostream* m_Stream;
        std::string m_Line;
        unsigned m_LineWidth;
        unsigned m_TabSize = 4;
        unsigned m_Indent = 0;
        unsigned m_Spaces = 0;
    };
}
