//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <algorithm>
#include <iostream>
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
            const auto pos = text.find_first_of("\n\r");
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
                return {'\0', text, {}};

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
                if (const auto n = text.find_first_not_of(' '); n != std::string_view::npos)
                    return {' ', text.substr(0, n), text.substr(n)};
                else
                    return {' ', text, {}};
            default:
                if (const auto n = text.find_first_of("\t\r\n "); n != std::string_view::npos)
                    return {'A', text.substr(0, n), text.substr(n)};
                else
                    return {'A', text, {}};
            }
        }

        bool is_list_marker(const std::string_view& str)
        {
            if (str.size() == 1)
                return  str[0] == '-' || str[0] == '*';

            if (str.size() > 1 && str.back() == '.')
            {
                return std::all_of(str.begin(), str.end() - 1,
                                   [](auto c) {return std::isdigit(c);});
            }

            return false;
        }
    }

    TextFormatter::TextFormatter()
        : TextFormatter(&std::cout, 0)
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, 0)
    {}

    TextFormatter::TextFormatter(std::ostream* stream, unsigned line_width)
        : m_writer(line_width ? line_width : get_console_width(32))
    {
        if (m_writer.line_width() <= 2)
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
                begin_alignment();
                break;
            case '\n':
                end_alignment();
                m_writer.newline();
                m_writer.set_spaces(0);
                break;
            case ' ':
                m_writer.set_spaces(static_cast<unsigned>(token.size()));
                begin_alignment();
                break;
            case 'A':
                update_alignment(token);
                append_word(token);
                break;
            default:
                break;
            }
            text = remainder;
        }
        end_alignment();
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

    void TextFormatter::append_word(std::string_view word)
    {
        auto remainder = word;
        while (!m_writer.write(remainder))
        {
            const auto width = m_writer.remaining_width();
            auto[w, s, r] = m_word_splitter.split(
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

    void TextFormatter::begin_alignment()
    {
        if (m_writer.is_current_line_empty()
            || m_state == State::UNALIGNED_MARKER)
        {
            push_indentation(CURRENT_COLUMN);
            m_state = State::ALIGNMENT;
        }
        else if (m_state == State::ALIGNED_MARKER)
        {
            pop_indentation();
            push_indentation(CURRENT_COLUMN);
            m_state = State::ALIGNMENT;
        }
    }

    void TextFormatter::update_alignment(const std::string_view& token)
    {
        if (m_writer.is_current_line_empty() && is_list_marker(token))
        {
            m_state = m_state == State::ALIGNMENT
                      ? State::ALIGNED_MARKER
                      : State::UNALIGNED_MARKER;
        }
    }

    void TextFormatter::end_alignment()
    {
        if (m_state == State::ALIGNMENT || m_state == State::ALIGNED_MARKER)
        {
            pop_indentation();
            m_state = State::NO_ALIGNMENT;
        }
    }
}
