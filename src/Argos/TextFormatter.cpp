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

    TextFormatter::TextFormatter(size_t indent, size_t width)
        : TextFormatter(&std::cout, indent, width)
    {}

    TextFormatter::TextFormatter(std::ostream* stream, size_t indent, size_t width)
        : m_Stream(stream)
    {
        m_Indents.emplace_back(indent, width);
        m_Spaces.fill(' ');
    }

    void TextFormatter::pushIndentation(size_t indent, size_t width)
    {
        if (width == 0)
            width = m_Indents.back().second;
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
        if (text.empty())
            return;

        while (!text.empty())
        {
            auto [word, sep, rem] = nextWord(text);
            if (word.empty())
                break;

            appendWord(word);
            text = rem;
        }
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
