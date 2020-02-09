//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <Argos/ArgosException.hpp>
#include <array>
#include "TextFormatter.hpp"

namespace Argos
{
    namespace
    {
        constexpr auto SEPARATORS = " \n\r";

        std::string_view skipWhitespace(std::string_view text)
        {
            while (!text.empty() && text[0] == ' ')
                text = text.substr(1);
            return text;
        }

        std::pair<std::string_view, std::string_view>
        nextLine(std::string_view text)
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

        std::tuple<std::string_view, char, std::string_view>
        nextWord(std::string_view text)
        {
            auto pos = text.find_first_not_of(" ");
            if (pos != 0)
                text = text.substr(pos);
            pos = text.find_first_of(SEPARATORS);
            if (pos == std::string_view::npos)
                return {text, '\0', std::string_view()};
            auto nxt = pos + 1;
            if (text[pos] == ' ')
                return {text.substr(0, pos), ' ', text.substr(nxt)};
            if (text[pos] == '\r' && nxt != text.size() && text[nxt] == '\n')
                ++nxt;
            return {text.substr(0, pos), '\n', text.substr(nxt)};
        }
    }

    TextFormatter::TextFormatter()
            : TextFormatter(&std::cout, 0, 80)
    {}

    TextFormatter::TextFormatter(size_t indent, size_t width)
        : TextFormatter(&std::cout, indent, width)
    {}

    TextFormatter::TextFormatter(std::ostream* stream, size_t indent, size_t width)
        : m_Stream(stream)
    {
        m_Indents.emplace_back(indent, width);
    }

    std::ostream* TextFormatter::stream() const
    {
        return m_Stream;
    }

    void TextFormatter::setStream(std::ostream* stream)
    {
        m_Stream = stream;
    }

    void TextFormatter::pushIndentation(size_t indent, size_t width)
    {
        if (indent == CURRENT_COLUMN)
            indent = m_Line.size();
        if (width == 0)
            width = m_Indents.back().second;
        indent = std::min(indent, 2 * width / 3);
        m_Indents.emplace_back(indent, width);
    }

    void TextFormatter::popIndentation()
    {
        if (m_Indents.size() == 1)
            ARGOS_THROW("No more indentations to pop.");
        m_Indents.pop_back();
    }

    void TextFormatter::writeText(std::string_view text)
    {
        while (!text.empty())
        {
            auto [word, sep, rem] = nextWord(text);
            if (word.empty())
                break;

            appendWord(word);
            if (sep == '\n')
                newline();
            text = rem;
        }
    }

    void TextFormatter::writeFormattedText(std::string_view text,
                                           bool useIndentation,
                                           bool fitFirstLine)
    {
        auto remainder = text;
        while (!remainder.empty())
        {
            auto [lin, rem] = nextLine(remainder);
            if (!lin.empty())
            {
                if (!m_Line.empty() && fitFirstLine)
                {
                    auto length = m_Indents.back().second - m_Line.size()
                                  - (m_Line.back() != ' ' ? 1 : 0);
                    if (lin.size() > length)
                        newline();
                    else if (m_Line.back() != ' ')
                        m_Line.push_back(' ');
                }
                if (m_Line.empty() && useIndentation)
                    indent();
                m_Line.append(lin);
            }
            if (!rem.empty())
                newline();
            remainder = rem;
        }
        if (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            newline();
    }

    void TextFormatter::newline()
    {
        m_Line.push_back('\n');
        flush();
    }

    void TextFormatter::flush()
    {
        if (!m_Line.empty())
        {
            m_Stream->write(m_Line.data(), m_Line.size());
            m_Line.clear();
        }
    }

    void TextFormatter::indent()
    {
        m_Line.assign(m_Indents.back().first, ' ');
    }

    void TextFormatter::appendWord(std::string_view word)
    {
        auto remainder = word;
        while (!remainder.empty())
        {
            bool mustWrite = false;
            bool addSpace = false;
            size_t length;
            if (m_Line.empty())
            {
                indent();
                mustWrite = true;
                length = m_Indents.back().second - m_Line.size();
            }
            else
            {
                addSpace = true;
                length = m_Indents.back().second - m_Line.size() - 1;
            }
            if (remainder.size() <= length)
            {
                if (addSpace)
                    m_Line.push_back(' ');
                m_Line.append(remainder.begin(), remainder.end());
                remainder = {};
            }
            else
            {
                auto [w, s, r] = m_WordSplitter.split(
                        word,
                        word.size() - remainder.size(),
                        length,
                        mustWrite);
                if (!w.empty())
                {
                    if (addSpace)
                        m_Line.push_back(' ');
                    m_Line.append(w);
                    if (s)
                        m_Line.push_back(s);
                }
                newline();
                remainder = r;
            }
        }
    }
}
