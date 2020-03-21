//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ArgumentParser.hpp"

#include <algorithm>
#include "ArgosThrow.hpp"
#include "ArgumentIteratorImpl.hpp"
#include "HelpText.hpp"
#include "ParserData.hpp"
#include "StringUtilities.hpp"

namespace Argos
{
    namespace
    {
        bool checkFlagWithEqual(const std::string& flag, const OptionData& od)
        {
            auto eqPos = flag.find('=');
            if (eqPos == std::string::npos)
                return true;
            if (eqPos != flag.size() - 1)
                return false;
            if (od.argument.empty())
                ARGOS_THROW("Options ending with '=' must take an argument: " + flag);
            return true;
        }

        bool checkStandardFlag(const std::string& flag, const OptionData& od)
        {
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() < 2)
                return false;
            if (flag[0] != '-')
                return false;
            if (flag.size() == 2)
                return true;
            if (flag[1] != '-')
                return false;
            return checkFlagWithEqual(flag, od);
        }

        bool checkFlag(const std::string& flag, char prefix, const OptionData& od)
        {
            if (flag.size() < 2 || flag[0] != prefix)
                return false;
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() == 2)
                return true;
            return checkFlagWithEqual(flag, od);
        }

        std::unique_ptr<ParserData> makeCopy(const ParserData& data)
        {
            auto result = std::make_unique<ParserData>();
            result->parserSettings = data.parserSettings;
            result->helpSettings = data.helpSettings;
            result->arguments.reserve(data.arguments.size());
            for (auto& a : data.arguments)
                result->arguments.push_back(std::make_unique<ArgumentData>(*a));
            result->options.reserve(data.options.size());
            for (auto& o : data.options)
                result->options.push_back(std::make_unique<OptionData>(*o));
            return result;
        }

        void setValueIds(const ParserData& data)
        {
            struct InternalIdMaker
            {
                ValueId n = ValueId(0);
                std::map<std::string_view, ValueId> explicitIds;

                ValueId makeValueId(std::string_view valueName)
                {
                    if (valueName.empty())
                    {
                        n = ValueId(n + 1);
                        return n;
                    }

                    auto it = explicitIds.find(valueName);
                    if (it == explicitIds.end())
                    {
                        n = ValueId(n + 1);
                        it = explicitIds.emplace(valueName, n).first;
                    }
                    return it->second;
                }
            };
            InternalIdMaker idMaker;
            for (auto& a : data.arguments)
            {
                if (!a->valueName.empty())
                {
                    a->valueId = idMaker.makeValueId(a->valueName);
                    idMaker.explicitIds.emplace(a->name, a->valueId);
                }
                else
                {
                    a->valueId = idMaker.makeValueId(a->name);
                }
            }
            for (auto& o : data.options)
            {
                if (o->operation == OptionOperation::NONE)
                    continue;
                o->valueId = idMaker.makeValueId(o->valueName);
                for (auto& f : o->flags)
                    idMaker.explicitIds.emplace(f, o->valueId);
            }
        }

        inline bool hasHelpOption(const ParserData& data)
        {
            for (auto& o : data.options)
                if (o->type == OptionType::HELP)
                    return true;
            return false;
        }

        inline bool hasFlag(const ParserData& data, std::string_view flag)
        {
            for (auto& o : data.options)
                for (auto& f : o->flags)
                    if (areEqual(f, flag, data.parserSettings.caseInsensitive))
                        return true;
            return false;
        }

        void addMissingHelpOption(ParserData& data)
        {
            if (!data.parserSettings.generateHelpOption)
                return;
            if (hasHelpOption(data))
                return;
            std::string flag;
            switch (data.parserSettings.optionStyle)
            {
            case OptionStyle::STANDARD:
                flag = "--help";
                break;
            case OptionStyle::SLASH:
                flag = "/?";
                break;
            case OptionStyle::DASH:
                flag = "-help";
                break;
            }
            if (hasFlag(data, flag))
                return;

            auto opt = Option{flag}.type(OptionType::HELP)
                .text("Show help text.")
                .value("1").release();
            opt->argumentId = ArgumentId(data.options.size()
                                         + data.arguments.size() + 1);
            data.options.push_back(move(opt));
        }

        ParsedArguments parseImpl(std::vector<std::string_view> args,
                                  const std::shared_ptr<ParserData>& data)
        {
            addMissingHelpOption(*data);
            setValueIds(*data);
            return ParsedArguments(
                    ArgumentIteratorImpl::parse(std::move(args), data));
        }

        ArgumentIterator
        makeIteratorImpl(std::vector<std::string_view> args,
                         const std::shared_ptr<ParserData>& data)
        {
            addMissingHelpOption(*data);
            setValueIds(*data);
            return ArgumentIterator(std::move(args), data);
        }
    }

    ArgumentParser::ArgumentParser()
            : ArgumentParser("UNINITIALIZED")
    {}

    ArgumentParser::ArgumentParser(const std::string& programName)
        : m_Data(std::make_unique<ParserData>())
    {
        m_Data->helpSettings.programName = programName;
    }

    ArgumentParser::ArgumentParser(ArgumentParser&& rhs) noexcept
        : m_Data(std::move(rhs.m_Data))
    {}

    ArgumentParser::~ArgumentParser() = default;

    ArgumentParser& ArgumentParser::operator=(ArgumentParser&& rhs) noexcept
    {
        m_Data = std::move(rhs.m_Data);
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Argument argument)
    {
        checkData();
        auto ad = argument.release();
        if (ad->name.empty())
            ARGOS_THROW("Argument must have a name.");
        ad->argumentId = nextArgumentId();
        m_Data->arguments.emplace_back(std::move(ad));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option option)
    {
        checkData();
        auto od = option.release();
        if (od->flags.empty())
            ARGOS_THROW("Option must have one or more flags.");
        for (auto& flag : od->flags)
        {
            bool ok;
            switch (m_Data->parserSettings.optionStyle)
            {
            case OptionStyle::STANDARD:
                ok = checkStandardFlag(flag, *od);
                break;
            case OptionStyle::SLASH:
                ok = checkFlag(flag, '/', *od);
                break;
            case OptionStyle::DASH:
                ok = checkFlag(flag, '-', *od);
                break;
            default:
                ok = false;
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!od->argument.empty() && !od->value.empty())
            ARGOS_THROW("Option cannot have both argument and value set.");
        switch (od->operation)
        {
        case OptionOperation::NONE:
            if (!od->value.empty())
                ARGOS_THROW("NONE-options cannot have value set.");
            if (!od->valueName.empty())
                ARGOS_THROW("NONE-options cannot have valueName set.");
            if (od->mandatory)
                ARGOS_THROW("NONE-options cannot be mandatory.");
            break;
        case OptionOperation::ASSIGN:
            if (od->argument.empty() && od->value.empty())
                od->value = "1";
            break;
        case OptionOperation::APPEND:
            if (od->argument.empty() && od->value.empty())
                ARGOS_THROW("Options that appends must have either value or argument set.");
            break;
        case OptionOperation::CLEAR:
            if (!od->argument.empty() ||!od->value.empty())
                od->value = "1";
            if (od->mandatory)
                ARGOS_THROW("CLEAR-options cannot be mandatory.");
            break;
        }
        od->argumentId = nextArgumentId();
        m_Data->options.push_back(std::move(od));
        return *this;
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv)
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv) const
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args)
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return parseImpl(std::move(args), std::move(m_Data));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args) const
    {
        checkData();
        return parseImpl(std::move(args), makeCopy(*m_Data));
    }

    ArgumentIterator ArgumentParser::makeIterator(int argc, char** argv)
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return makeIterator(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ArgumentIterator ArgumentParser::makeIterator(int argc, char** argv) const
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return makeIterator(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ArgumentIterator ArgumentParser::makeIterator(
            std::vector<std::string_view> args)
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return makeIteratorImpl(std::move(args), std::move(m_Data));
    }

    ArgumentIterator ArgumentParser::makeIterator(std::vector<std::string_view> args) const
    {
        checkData();
        return makeIteratorImpl(std::move(args), makeCopy(*m_Data));
    }

    bool ArgumentParser::allowAbbreviatedOptions() const
    {
        checkData();
        return m_Data->parserSettings.allowAbbreviatedOptions;
    }

    ArgumentParser& ArgumentParser::allowAbbreviatedOptions(bool value)
    {
        checkData();
        m_Data->parserSettings.allowAbbreviatedOptions = value;
        return *this;
    }

    bool ArgumentParser::autoExit() const
    {
        checkData();
        return m_Data->parserSettings.autoExit;
    }

    ArgumentParser& ArgumentParser::autoExit(bool value)
    {
        checkData();
        m_Data->parserSettings.autoExit = value;
        return *this;
    }

    bool ArgumentParser::caseInsensitive() const
    {
        checkData();
        return m_Data->parserSettings.caseInsensitive;
    }

    ArgumentParser& ArgumentParser::caseInsensitive(bool value)
    {
        checkData();
        m_Data->parserSettings.caseInsensitive = value;
        return *this;
    }

    bool ArgumentParser::generateHelpOption() const
    {
        checkData();
        return m_Data->parserSettings.generateHelpOption;
    }

    ArgumentParser& ArgumentParser::generateHelpOption(bool value)
    {
        checkData();
        m_Data->parserSettings.generateHelpOption = value;
        return *this;
    }

    OptionStyle ArgumentParser::optionStyle() const
    {
        checkData();
        return m_Data->parserSettings.optionStyle;
    }

    ArgumentParser& ArgumentParser::optionStyle(OptionStyle value)
    {
        checkData();
        if (value != m_Data->parserSettings.optionStyle)
        {
            if (!m_Data->options.empty())
                ARGOS_THROW("Can't change option style after"
                            " options have been added.");
            m_Data->parserSettings.optionStyle = value;
        }
        return *this;
    }

    bool ArgumentParser::ignoreUndefinedArguments() const
    {
        checkData();
        return m_Data->parserSettings.ignoreUndefinedArguments;
    }

    ArgumentParser& ArgumentParser::ignoreUndefinedArguments(bool value)
    {
        checkData();
        m_Data->parserSettings.ignoreUndefinedArguments = value;
        return *this;
    }

    bool ArgumentParser::ignoreUndefinedOptions() const
    {
        checkData();
        return m_Data->parserSettings.ignoreUndefinedOptions;
    }

    ArgumentParser& ArgumentParser::ignoreUndefinedOptions(bool value)
    {
        checkData();
        m_Data->parserSettings.ignoreUndefinedOptions = value;
        return *this;
    }

    const ArgumentCallback& ArgumentParser::argumentCallback() const
    {
        checkData();
        return m_Data->parserSettings.argumentCallback;
    }

    ArgumentParser& ArgumentParser::argumentCallback(ArgumentCallback callback)
    {
        checkData();
        m_Data->parserSettings.argumentCallback = std::move(callback);
        return *this;
    }

    const OptionCallback& ArgumentParser::optionCallback() const
    {
        checkData();
        return m_Data->parserSettings.optionCallback;
    }

    ArgumentParser& ArgumentParser::optionCallback(OptionCallback callback)
    {
        checkData();
        m_Data->parserSettings.optionCallback = std::move(callback);
        return *this;
    }

    std::ostream* ArgumentParser::outputStream() const
    {
        checkData();
        return m_Data->textFormatter.stream();
    }

    ArgumentParser& ArgumentParser::outputStream(std::ostream* stream)
    {
        checkData();
        m_Data->textFormatter.setStream(stream);
        return *this;
    }

    const std::string& ArgumentParser::programName() const
    {
        checkData();
        return m_Data->helpSettings.programName;
    }

    ArgumentParser& ArgumentParser::programName(const std::string& name)
    {
        checkData();
        m_Data->helpSettings.programName = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::text(std::string text)
    {
        return this->text(TextId::TEXT, std::move(text));
    }

    ArgumentParser& ArgumentParser::text(TextId textId, std::string text)
    {
        checkData();
        m_Data->helpSettings.texts[textId] = std::move(text);
        return *this;
    }

    ArgumentParser&& ArgumentParser::move()
    {
        return std::move(*this);
    }

    void ArgumentParser::writeHelpText()
    {
        checkData();
        Argos::writeHelpText(*m_Data);
    }

    void ArgumentParser::checkData() const
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
    }

    ArgumentId ArgumentParser::nextArgumentId() const
    {
        auto& d = *m_Data;
        return ArgumentId(d.options.size() + d.arguments.size() + 1);
    }
}
