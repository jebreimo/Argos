//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <array>
#include "ArgosThrow.hpp"
#include "ConsoleWidth.hpp"
#include "TextFormatter.hpp"

namespace argos
{
    namespace
    {
        std::pair<std::string_view, std::string_view>
        next_line(std::string_view text)
        {
            auto pos = text.find_first_of("\n\r");
            if (pos == std::string_view::npos)
                return {text, {}};
            if (text[pos] == '\n'
                || (pos + 1 != text.size() && text[pos + 1] != '\n'))
            {
                return {text.substr(0, pos), text.substr(pos + 1)};
            }
            return {text.substr(0, pos), text.substr(pos + 2)};
        }

        std::tuple<char, std::string_view, std::string_view>
        next_token(std::string_view text)
        {
            if (text.empty())
                return {'0', text, {}};

            switch (text[0])
            {
            case '\t':
                return {'\t', text.substr(0, 1), text.substr(1)};
            case '\r':
                if (text.size() > 1 && text[1] == '\n')
                    return {'\n', text.substr(0, 2), text.substr(2)};
                [[fallthrough]];
            case '\n':
                return {'\n', text.substr(0, 1), text.substr(1)};
            case ' ':
                if (auto n = text.find_first_not_of(' '); n != std::string_view::npos)
                    return {' ', text.substr(0, n), text.substr(n)};
                else
                    return {' ', text, {}};
            default:
                if (auto n = text.find_first_of("\t\r\n "); n != std::string_view::npos)
                    return {'A', text.substr(0, n), text.substr(n)};
                else
                    return {'A', text, {}};
            }
        }
    }

    TextFormatter::TextFormatter()
        : TextFormatter(&std::cout, get_console_width(32))
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, get_console_width(32))
    {}

    TextFormatter::TextFormatter(std::ostream* stream, unsigned line_width)
        : m_writer(line_width)
    {
        if (line_width <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_writer.set_stream(stream);
        m_indents.push_back(0);
    }

    WordSplitter& TextFormatter::word_splitter()
    {
        return m_word_splitter;
    }

    std::ostream* TextFormatter::stream() const
    {
        return m_writer.stream();
    }

    void TextFormatter::set_stream(std::ostream* stream)
    {
        m_writer.set_stream(stream);
    }

    void TextFormatter::push_indentation(unsigned indent)
    {
        if (indent == CURRENT_COLUMN)
        {
            indent = m_writer.current_width();
            m_writer.set_spaces(0);
        }
        m_indents.push_back(indent);
        m_writer.set_indentation(indent);
    }

    void TextFormatter::pop_indentation()
    {
        if (m_indents.size() == 1)
            ARGOS_THROW("No more indentations to pop.");
        m_indents.pop_back();
        m_writer.set_indentation(m_indents.back());
    }

    void TextFormatter::write_words(std::string_view text)
    {
        while (!text.empty())
        {
            auto [type, token, remainder] = next_token(text);
            switch (type)
            {
            case '\t':
                m_writer.tab();
                break;
            case '\n':
                m_writer.newline();
                break;
            case ' ':
                m_writer.set_spaces(static_cast<unsigned>(token.size()));
                break;
            default:
                append_word(token);
                break;
            }
            text = remainder;
        }
    }

    void TextFormatter::write_lines(std::string_view text)
    {
        auto remainder = text;
        while (!remainder.empty())
        {
            auto [line, rem] = next_line(remainder);
            if (!line.empty())
                append_word(line);
            if (!rem.empty())
                newline();
            remainder = rem;
        }
        if (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            newline();
    }

    void TextFormatter::newline()
    {
        m_writer.newline();
        m_writer.set_spaces(0);
    }

    void TextFormatter::flush()
    {
        m_writer.flush();
    }

    void TextFormatter::append_word(std::string_view word)
    {
        auto remainder = word;
        while (!m_writer.write(remainder))
        {
            auto width = m_writer.remaining_width();
            auto [w, s, r] = m_word_splitter.split(
                word,
                word.size() - remainder.size(),
                width,
                m_writer.is_current_line_empty());
            if (!w.empty())
            {
                m_writer.write(w);
                if (s)
                    m_writer.write(std::string_view(&s, 1));
                newline();
                remainder = r;
            }
            else if (m_writer.is_current_line_empty())
            {
                if (m_writer.spaces() != 0)
                {
                    m_writer.set_spaces(0);
                }
                else
                {
                    m_writer.write(remainder, true);
                    return;
                }
            }
            else
            {
                newline();
                m_writer.set_spaces(0);
            }
        }
    }

    unsigned TextFormatter::line_width() const
    {
        return m_writer.line_width();
    }

    void TextFormatter::set_line_width(unsigned line_width)
    {
        if (line_width <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_writer.set_line_width(line_width);
    }

    unsigned int TextFormatter::current_line_width() const
    {
        return m_writer.current_width();
    }

    bool TextFormatter::is_current_line_empty() const
    {
        return m_writer.is_current_line_empty();
    }
}
