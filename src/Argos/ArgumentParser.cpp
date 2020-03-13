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
#include "ParserData.hpp"
#include "HelpText.hpp"

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
                ARGOS_THROW(flag + ": options ending with '=' must take an argument.");
            return true;
        }

        bool checkStandardFlag(const std::string& flag, const OptionData& od)
        {
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
                std::map<std::string, ValueId> explicitIds;

                ValueId makeValueId(const std::string& valueName)
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

        ParsedArguments parseImpl(std::vector<std::string_view> args,
                                  const std::shared_ptr<ParserData>& data)
        {
            setValueIds(*data);
            return ParsedArguments(
                    ArgumentIteratorImpl::parse(std::move(args), data));
        }

        ArgumentIterator
        makeIteratorImpl(std::vector<std::string_view> args,
                         const std::shared_ptr<ParserData>& data)
        {
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
        data().helpSettings.programName = programName;
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
        auto ad = argument.release();
        if (ad->name.empty())
            ARGOS_THROW("Argument must have a name.");
        ad->argumentId = nextArgumentId();
        data().arguments.emplace_back(std::move(ad));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option option)
    {
        auto od = option.release();

        if (od->flags.empty())
            ARGOS_THROW("Option must have one or more flags.");
        for (auto& flag : od->flags)
        {
            bool ok;
            switch (data().parserSettings.optionStyle)
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
        data().options.push_back(std::move(od));
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
        return parseImpl(std::move(args), makeCopy(data()));
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
        return makeIteratorImpl(std::move(args), makeCopy(data()));
    }

    bool ArgumentParser::allowAbbreviatedOptions() const
    {
        return data().parserSettings.allowAbbreviatedOptions;
    }

    ArgumentParser& ArgumentParser::allowAbbreviatedOptions(bool value)
    {
        data().parserSettings.allowAbbreviatedOptions = value;
        return *this;
    }

    bool ArgumentParser::autoExit() const
    {
        return data().parserSettings.autoExit;
    }

    ArgumentParser& ArgumentParser::autoExit(bool value)
    {
        data().parserSettings.autoExit = value;
        return *this;
    }

    bool ArgumentParser::caseInsensitive() const
    {
        return data().parserSettings.caseInsensitive;
    }

    ArgumentParser& ArgumentParser::caseInsensitive(bool value)
    {
        data().parserSettings.caseInsensitive = value;
        return *this;
    }

    OptionStyle ArgumentParser::optionStyle() const
    {
        return data().parserSettings.optionStyle;
    }

    ArgumentParser& ArgumentParser::optionStyle(OptionStyle value)
    {
        if (value != data().parserSettings.optionStyle)
        {
            if (!data().options.empty())
                ARGOS_THROW("Can't change option style after"
                            " options have been added.");
            data().parserSettings.optionStyle = value;
        }
        return *this;
    }

    bool ArgumentParser::ignoreUndefinedArguments() const
    {
        return data().parserSettings.ignoreUndefinedArguments;
    }

    ArgumentParser& ArgumentParser::ignoreUndefinedArguments(bool value)
    {
        data().parserSettings.ignoreUndefinedArguments = value;
        return *this;
    }

    bool ArgumentParser::ignoreUndefinedOptions() const
    {
        return data().parserSettings.ignoreUndefinedOptions;
    }

    ArgumentParser& ArgumentParser::ignoreUndefinedOptions(bool value)
    {
        data().parserSettings.ignoreUndefinedOptions = value;
        return *this;
    }

    const ArgumentCallback& ArgumentParser::argumentCallback() const
    {
        return data().parserSettings.argumentCallback;
    }

    ArgumentParser& ArgumentParser::argumentCallback(ArgumentCallback callback)
    {
        data().parserSettings.argumentCallback = std::move(callback);
        return *this;
    }

    const OptionCallback& ArgumentParser::optionCallback() const
    {
        return data().parserSettings.optionCallback;
    }

    ArgumentParser& ArgumentParser::optionCallback(OptionCallback callback)
    {
        data().parserSettings.optionCallback = std::move(callback);
        return *this;
    }

    std::ostream* ArgumentParser::outputStream() const
    {
        return m_Data->textFormatter.stream();
    }

    ArgumentParser& ArgumentParser::outputStream(std::ostream* stream)
    {
        m_Data->textFormatter.setStream(stream);
        return *this;
    }

    const std::string& ArgumentParser::programName() const
    {
        return data().helpSettings.programName;
    }

    ArgumentParser& ArgumentParser::programName(const std::string& name)
    {
        data().helpSettings.programName = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::text(std::string text)
    {
        return this->text(TextId::TEXT, std::move(text));
    }

    ArgumentParser& ArgumentParser::text(TextId textId, std::string text)
    {
        data().helpSettings.texts[textId] = std::move(text);
        return *this;
    }

    ArgumentParser&& ArgumentParser::move()
    {
        return std::move(*this);
    }

    void ArgumentParser::writeHelpText()
    {
        Argos::writeHelpText(data());
    }

    const ParserData& ArgumentParser::data() const
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return *m_Data;
    }

    ParserData& ArgumentParser::data()
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return *m_Data;
    }

    ArgumentId ArgumentParser::nextArgumentId() const
    {
        auto& d = data();
        return ArgumentId(d.options.size() + d.arguments.size() + 1);
    }
}
