//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iostream>
#include <array>
#include "Argos/ArgosException.hpp"
#include "ConsoleWidth.hpp"
#include "TextFormatter.hpp"

namespace Argos
{
    namespace
    {
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
            pos = text.find_first_of(" \n\r");
            if (pos == std::string_view::npos)
                return {text, '\0', std::string_view()};
            auto nxt = pos + 1;
            if (text[pos] == ' ')
                return {text.substr(0, pos), ' ', text.substr(nxt)};
            if (text[pos] == '\r' && nxt != text.size() && text[nxt] == '\n')
                ++nxt;
            return {text.substr(0, pos), '\n', text.substr(nxt)};
        }

        std::tuple<char, std::string_view, std::string_view>
        nextToken(std::string_view text)
        {
            if (text.empty())
                return {'0', text, {}};

            switch (text[0])
            {
            case '\t':
                return {'\t', text.substr(0, 1), text.substr(1)};
            case '\r':
                if (text.size() > 1 || text[1] == '\n')
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
        : TextFormatter(&std::cout, getConsoleWidth(20))
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, getConsoleWidth(20))
    {}

    TextFormatter::TextFormatter(std::ostream* stream, size_t lineWidth)
        : m_Writer(lineWidth)
    {
        if (lineWidth <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_Writer.setStream(stream);
        m_Indents.push_back(0);
    }

    std::ostream* TextFormatter::stream() const
    {
        return m_Writer.stream();
    }

    void TextFormatter::setStream(std::ostream* stream)
    {
        m_Writer.setStream(stream);
    }

    void TextFormatter::pushIndentation(size_t indent)
    {
        if (indent == CURRENT_COLUMN)
        {
            indent = m_Writer.currentWidth();
            m_Writer.setSpaces(0);
        }
        m_Indents.push_back(indent);
        m_Writer.setIndentation(indent);
    }

    void TextFormatter::popIndentation()
    {
        if (m_Indents.size() == 1)
            ARGOS_THROW("No more indentations to pop.");
        m_Indents.pop_back();
        m_Writer.setIndentation(m_Indents.back());
    }

    void TextFormatter::writeText(std::string_view text)
    {
        while (!text.empty())
        {
            auto [type, token, remainder] = nextToken(text);
            switch (type)
            {
            case '\t':
                m_Writer.tab();
                break;
            case '\n':
                m_Writer.newline();
                break;
            case ' ':
                m_Writer.setSpaces(token.size());
                break;
            default:
                appendWord(token);
                break;
            }
            text = remainder;
        }
    }

    void TextFormatter::writePreformattedText(std::string_view text)
    {
        auto remainder = text;
        while (!remainder.empty())
        {
            auto [line, rem] = nextLine(remainder);
            if (!line.empty())
                appendWord(line);
            if (!rem.empty())
                newline();
            remainder = rem;
        }
        if (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            newline();
    }

    void TextFormatter::newline()
    {
        m_Writer.newline();
        m_Writer.setSpaces(0);
    }

    void TextFormatter::flush()
    {
        m_Writer.flush();
    }

    void TextFormatter::appendWord(std::string_view word)
    {
        auto remainder = word;
        while (!m_Writer.write(remainder))
        {
            auto width = m_Writer.remainingWidth();
            auto [w, s, r] = m_WordSplitter.split(
                    word,
                    word.size() - remainder.size(),
                    width,
                    m_Writer.empty());
            if (!w.empty())
            {
                m_Writer.write(w);
                if (s)
                    m_Writer.write(std::string_view(&s, 1));
                newline();
                remainder = r;
            }
            else if (m_Writer.empty())
            {
                if (m_Writer.spaces() != 0)
                {
                    m_Writer.setSpaces(0);
                }
                else
                {
                    m_Writer.write(remainder, true);
                    return;
                }
            }
            else
            {
                newline();
                m_Writer.setSpaces(0);
            }
        }
    }

    size_t TextFormatter::lineWidth() const
    {
        return m_Writer.lineWidth();
    }

    void TextFormatter::setLineWidth(size_t lineWidth)
    {
        if (lineWidth <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_Writer.setLineWidth(lineWidth);
    }
}
