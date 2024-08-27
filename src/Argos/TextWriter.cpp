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

namespace argos
{
    TextWriter::TextWriter(unsigned line_width)
        : m_stream(&std::cout),
          m_line_width(line_width)
    {}

    std::ostream* TextWriter::stream() const
    {
        return m_stream;
    }

    void TextWriter::set_stream(std::ostream* stream)
    {
        m_stream = stream;
    }

    unsigned TextWriter::indentation() const
    {
        return m_indent;
    }

    bool TextWriter::set_indentation(unsigned indent)
    {
        if (m_indent >= m_line_width)
            return false;
        m_indent = indent;
        return true;
    }

    bool TextWriter::write(std::string_view str, bool force)
    {
        const auto width = current_width();
        const auto remaining = std::max(width, m_line_width) - width;
        const auto str_width = static_cast<unsigned>(count_code_points(str));
        if (!force && str_width > remaining)
            return false;
        m_line.append(width - m_current_line_width, ' ');
        m_spaces = 0;
        m_line.append(str);
        m_current_line_width += width - m_current_line_width + str_width;
        return true;
    }

    void TextWriter::newline()
    {
        m_line.push_back('\n');
        m_current_line_width = 0;
        flush();
    }

    void TextWriter::flush()
    {
        m_stream->write(m_line.data(), std::streamsize(m_line.size()));
        m_line.clear();
    }

    void TextWriter::tab()
    {
        m_spaces += m_tab_size - (current_width() % m_tab_size);
    }

    unsigned TextWriter::spaces() const
    {
        return m_spaces;
    }

    void TextWriter::set_spaces(unsigned n)
    {
        m_spaces = n;
    }

    unsigned TextWriter::current_width() const
    {
        return std::max(m_current_line_width, m_indent) + m_spaces;
    }

    unsigned TextWriter::remaining_width() const
    {
        return m_line_width - std::min(current_width(), m_line_width);
    }

    bool TextWriter::is_current_line_empty() const
    {
        return m_line.empty();
    }

    unsigned TextWriter::line_width() const
    {
        return m_line_width;
    }

    void TextWriter::set_line_width(unsigned width)
    {
        m_line_width = width;
    }

    std::string_view TextWriter::currentLine() const
    {
        return m_line;
    }
}
