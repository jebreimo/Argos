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

namespace argos
{
    class TextWriter
    {
    public:
        explicit TextWriter(unsigned line_width = 80);

        std::ostream* stream() const;

        void set_stream(std::ostream* stream);

        unsigned indentation() const;

        bool set_indentation(unsigned indent);

        bool write(std::string_view str, bool force = false);

        void newline();

        void flush();

        void tab();

        unsigned spaces() const;

        void set_spaces(unsigned n);

        unsigned current_width() const;

        unsigned remaining_width() const;

        bool is_current_line_empty() const;

        unsigned line_width() const;

        void set_line_width(unsigned width);

        std::string_view currentLine() const;
    private:
        std::ostream* m_stream;
        std::string m_line;
        unsigned m_line_width;
        unsigned m_current_line_width = 0;
        unsigned m_tab_size = 4;
        unsigned m_indent = 0;
        unsigned m_spaces = 0;
    };
}
