//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Argos/Argos.hpp>

#include <algorithm>
#include <iostream>

std::string getName(const Argos::IArgumentView& arg)
{
    if (const auto* a = dynamic_cast<const Argos::ArgumentView*>(&arg))
        return a->name();

    if (const auto* o = dynamic_cast<const Argos::OptionView*>(&arg))
    {
        std::string s;
        for (auto& f : o->flags())
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
                   const Argos::ArgumentValues& values)
{
    stream << label << ":";
    for (auto value : values)
        stream << " \"" << value.asString() << "\"";
    stream << "\n";
}

void printArguments(std::ostream& stream, const Argos::ParsedArguments& args)
{
    std::vector<const Argos::IArgumentView*> argViews;
    auto a = args.allArguments();
    std::transform(a.begin(), a.end(), back_inserter(argViews),
                   [](auto& av) {return av.get();});
    auto o = args.allOptions();
    std::transform(o.begin(), o.end(), back_inserter(argViews),
                   [](auto& ov) {return ov.get();});

    stable_sort(argViews.begin(), argViews.end(),
                [](auto& a, auto& b) {return a->valueId() < b->valueId();});

    std::vector<std::pair<const Argos::IArgumentView*, std::string>> labels;
    for (const auto* arg : argViews)
    {
        if (!labels.empty() && labels.back().first->valueId() == arg->valueId())
            labels.back().second += ", " + getName(*arg);
        else
            labels.push_back({arg, getName(*arg)});
    }

    for (const auto& [arg, label] : labels)
        printArgument(stream, label, args.values(*arg));
}

int main(int argc, char* argv[])
{
    using namespace Argos;
    auto args = ArgumentParser("ArgosTestApp")
            .allowAbbreviatedOptions(true)
            .add(Argument("file").count(1, UINT_MAX)
                .text("A file of some kind."))
            .add(Option({"-r", "--resolution"}).argument("HOR,VER")
                .text("Set screen resolution."))
            .add(Option({"--fullscreen"}).constant(true)
                .text("Run in fullscreen mode."))
            .add(Option({"--windowed"}).alias("--fullscreen").constant(false)
                .text("Run in windowed mode."))
            .add(Option({"--list-interfaces"}).type(OptionType::LAST_ARGUMENT)
                .text("Display list of available graphics interfaces."))
            .add(Option({"--"}).type(OptionType::LAST_OPTION)
                .text("Mark end of options. Allows arguments starting with '-'."))
            .add(Option({"--version"}).type(OptionType::STOP)
                .text("Show version."))
            .add(Option({"-a", "--anonymous"}).visibility(Visibility::USAGE))
            .parse(argc, argv);

    printArguments(std::cout, args);

    return 0;
}
