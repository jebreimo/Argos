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

namespace argos
{
    class TextFormatter
    {
    public:
        TextFormatter();

        explicit TextFormatter(std::ostream* stream);

        TextFormatter(std::ostream* stream, unsigned line_width);

        WordSplitter& word_splitter();

        [[nodiscard]] std::ostream* stream() const;

        void set_stream(std::ostream* stream);

        [[nodiscard]] unsigned line_width() const;

        void set_line_width(unsigned line_width);

        [[nodiscard]] unsigned current_line_width() const;

        [[nodiscard]] bool is_current_line_empty() const;

        static constexpr unsigned CURRENT_COLUMN = UINT_MAX;

        void push_indentation(unsigned indent);

        void pop_indentation();

        void write_words(std::string_view text);

        void write_lines(std::string_view text);

        void newline();

        void flush();
    private:
        void append_word(std::string_view word);

        void begin_alignment();

        void update_alignment(const std::string_view& token);

        void end_alignment();

        TextWriter m_writer;
        std::vector<unsigned> m_indents;
        WordSplitter m_word_splitter;
        enum class State
        {
            NO_ALIGNMENT,
            ALIGNMENT,
            UNALIGNED_MARKER,
            ALIGNED_MARKER
        };
        State m_state = State::NO_ALIGNMENT;
    };
}
