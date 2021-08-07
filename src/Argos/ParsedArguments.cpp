//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Argos/ParsedArguments.hpp"

#include "ArgosThrow.hpp"
#include "ParsedArgumentsImpl.hpp"

namespace Argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_Impl(move(impl))
    {}

    ParsedArguments::ParsedArguments(ParsedArguments&& rhs) noexcept
        : m_Impl(move(rhs.m_Impl))
    {}

    ParsedArguments::~ParsedArguments() = default;

    ParsedArguments& ParsedArguments::operator=(ParsedArguments&& rhs) noexcept
    {
        m_Impl = move(rhs.m_Impl);
        return *this;
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArguments::has(const IArgumentView& arg) const
    {
        return m_Impl->has(arg.valueId());
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (value)
            return {value->first, m_Impl, id, value->second};
        else
            return {{}, m_Impl, id, {}};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        auto value = m_Impl->getValue(arg.valueId());
        if (value)
            return {value->first, m_Impl, arg.valueId(), arg.argumentId()};
        else
            return {{}, m_Impl, arg.valueId(), arg.argumentId()};
    }

    ArgumentValues ParsedArguments::values(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        return {values, m_Impl, id};
    }

    ArgumentValues ParsedArguments::values(const IArgumentView& arg) const
    {
        auto values = m_Impl->getValues(arg.valueId());
        return {values, m_Impl, arg.valueId()};
    }

    std::vector<std::unique_ptr<ArgumentView>>
    ParsedArguments::allArguments() const
    {
        std::vector<std::unique_ptr<ArgumentView>> result;
        for (auto& a : m_Impl->parserData()->arguments)
            result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        return result;
    }

    std::vector<std::unique_ptr<OptionView>>
    ParsedArguments::allOptions() const
    {
        std::vector<std::unique_ptr<OptionView>> result;
        for (auto& o : m_Impl->parserData()->options)
            result.emplace_back(std::make_unique<OptionView>(o.get()));
        return result;
    }

    ParserResultCode ParsedArguments::resultCode() const
    {
        return m_Impl->resultCode();
    }

    OptionView ParsedArguments::stopOption() const
    {
        const auto* option = m_Impl->stopOption();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }

    const std::vector<std::string>&
    ParsedArguments::unprocessedArguments() const
    {
        return m_Impl->unprocessedArguments();
    }

    void ParsedArguments::filterParsedArguments(int& argc, char**& argv)
    {
        if (argc <= 1)
            return;
        const auto& unprocessed = m_Impl->unprocessedArguments();
        auto it = unprocessed.begin();
        int out = 1;
        if (!unprocessed.empty())
        {
            for (int in = 1; in < argc; ++in)
            {
                if (argv[in] == *it)
                {
                    argv[out++] = argv[in];
                    if (++it == unprocessed.end())
                        break;
                }
            }
        }
        argc = out;
    }

    void ParsedArguments::error(const std::string& msg)
    {
        m_Impl->error(msg);
    }

    namespace
    {
        std::string getName(const IArgumentView& arg)
        {
            if (const auto* a = dynamic_cast<const ArgumentView*>(&arg))
                return a->name();

            if (const auto* o = dynamic_cast<const OptionView*>(&arg))
            {
                std::string s;
                for (const auto& f : o->flags())
                {
                    if (!s.empty())
                        s += ", ";
                    s += f;
                }
                return s;
            }
            return {};
        }

        void printArgument(std::ostream& stream,
                           const std::string& label,
                           const ArgumentValues& values)
        {
            stream << label << ":";
            for (const auto value : values)
                stream << " \"" << value.asString() << "\"";
            stream << "\n";
        }
    }

    void print(std::ostream& stream, const ParsedArguments& args)
    {
        std::vector<const IArgumentView*> argViews;
        auto a = args.allArguments();
        std::transform(a.begin(), a.end(), back_inserter(argViews),
                       [](auto& av) {return av.get();});
        auto o = args.allOptions();
        std::transform(o.begin(), o.end(), back_inserter(argViews),
                       [](auto& ov) {return ov.get();});

        stable_sort(argViews.begin(), argViews.end(),
                    [](auto& a, auto& b) {return a->valueId() < b->valueId();});

        std::vector<std::pair<const IArgumentView*, std::string>> labels;
        for (const auto* arg : argViews)
        {
            if (!labels.empty() && labels.back().first->valueId() == arg->valueId())
                labels.back().second += ", " + getName(*arg);
            else
                labels.emplace_back(arg, getName(*arg));
        }

        for (const auto&[arg, label] : labels)
            printArgument(stream, label, args.values(*arg));
    }
}
