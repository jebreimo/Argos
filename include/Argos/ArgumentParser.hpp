//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include "Argument.hpp"
#include "ArgumentIterator.hpp"
#include "Option.hpp"
#include "ParsedArguments.hpp"

namespace Argos
{
    struct ParserData;

    class ArgumentParser
    {
    public:
        ArgumentParser();

        explicit ArgumentParser(const std::string& programName);

        ArgumentParser(ArgumentParser&&) noexcept;

        ArgumentParser(const ArgumentParser&) = delete;

        ~ArgumentParser();

        ArgumentParser& operator=(ArgumentParser&&) noexcept;

        ArgumentParser& operator=(const ArgumentParser&) = delete;

        ArgumentParser& add(Argument& argument);

        ArgumentParser& add(Option& option);

        ParsedArguments parse(int argc, char* argv[]);

        ParsedArguments parse(int argc, char* argv[]) const;

        ArgumentIterator makeIterator(int argc, char* argv[]);

        ArgumentIterator makeIterator(int argc, char* argv[]) const;

        bool autoExitEnabled() const;

        ArgumentParser& autoExitEnabled(bool value);

        std::ostream* outputStream() const;

        ArgumentParser& outputStream(std::ostream* stream);

        const std::string& programName() const;

        ArgumentParser& programName(const std::string& name);
    private:
        const ParserData& data() const;

        ParserData& data();

        std::unique_ptr<ParserData> m_Data;
    };
}
