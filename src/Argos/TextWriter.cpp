//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TextWriter.hpp"
#include "StringUtilities.hpp"

#include <algorithm>
#include <iostream>

namespace Argos
{
    TextWriter::TextWriter(unsigned lineWidth)
        : m_Stream(&std::cout),
          m_LineWidth(lineWidth)
    {}

    std::ostream* TextWriter::stream() const
    {
        return m_Stream;
    }

    void TextWriter::setStream(std::ostream* stream)
    {
        m_Stream = stream;
    }

    unsigned TextWriter::indentation() const
    {
        return m_Indent;
    }

    bool TextWriter::setIndentation(unsigned indent)
    {
        if (m_Indent >= m_LineWidth)
            return false;
        m_Indent = indent;
        return true;
    }

    bool TextWriter::write(std::string_view str, bool force)
    {
        auto width = currentWidth();
        auto remaining = std::max(width, m_LineWidth) - width;
        auto strWidth = countCodePoints(str);
        if (!force && strWidth > remaining)
            return false;
        m_Line.append(width - m_CurrentLineWidth, ' ');
        m_Spaces = 0;
        m_Line.append(str);
        m_CurrentLineWidth += width - m_CurrentLineWidth + strWidth;
        return true;
    }

    void TextWriter::newline()
    {
        m_Line.push_back('\n');
        m_CurrentLineWidth = 0;
        flush();
    }

    void TextWriter::flush()
    {
        m_Stream->write(m_Line.data(), m_Line.size());
        m_Line.clear();
    }

    void TextWriter::tab()
    {
        m_Spaces += m_TabSize - (currentWidth() % m_TabSize);
    }

    unsigned TextWriter::spaces() const
    {
        return m_Spaces;
    }

    void TextWriter::setSpaces(unsigned n)
    {
        m_Spaces = n;
    }

    unsigned TextWriter::currentWidth() const
    {
        return std::max(m_CurrentLineWidth, m_Indent) + m_Spaces;
    }

    unsigned TextWriter::remainingWidth() const
    {
        return m_LineWidth - std::min(currentWidth(), m_LineWidth);
    }

    bool TextWriter::isCurrentLineEmpty() const
    {
        return m_Line.empty();
    }

    unsigned TextWriter::lineWidth() const
    {
        return m_LineWidth;
    }

    void TextWriter::setLineWidth(unsigned width)
    {
        m_LineWidth = width;
    }

    std::string_view TextWriter::currentLine() const
    {
        return m_Line;
    }
}
