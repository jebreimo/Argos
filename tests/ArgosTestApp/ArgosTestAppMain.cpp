//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Argos/ArgumentParser.hpp>
#include <iostream>
#include <Argos/ParseValue.hpp>

struct Rectangle
{
    int width = 0;
    int height = 0;
};

std::vector<std::string_view> splitString(std::string_view s, char delimiter)
{
    std::vector<std::string_view> result;
    size_t pos = 0;
    while (true)
    {
        auto nextPos = s.find(delimiter, pos);
        result.push_back(s.substr(pos, nextPos - pos));
        if (nextPos == std::string_view::npos)
            break;
        pos = nextPos + 1;
    }
    return result;
}

Rectangle parseResolution(const Argos::ArgumentValue& v)
{
    auto s = v.value();
    if (!s)
        return {640, 480};
    auto parts = splitString(*s, ',');
    if (parts.size() != 2)
        v.error("Resolution must be two comma-separated integers.");
    auto hor = Argos::parseValue<int>(parts[0]);
    auto ver = Argos::parseValue<int>(parts[1]);
    if (!hor || *hor < 640)
        v.error("Horizontal resolution must be at least 640");
    if (!ver || *ver < 480)
        v.error("Vertical resolution must be at least 480");
    return {*hor, *ver};
}

int main(int argc, char* argv[])
{
    using namespace Argos;
    auto args = ArgumentParser("ArgosTestApp")
            .allowAbbreviatedOptions(true)
            .add(Argument("file").text("A file of some kind."))
            .add(Option({"--help"}).type(OptionType::HELP).text("Show help."))
            .add(Option({"-r", "--resolution"}).argument("HOR,VER").text("Set screen resolution."))
            .add(Option({"--fullscreen"}).value(true).text("Run in fullscreen mode."))
            .add(Option({"--windowed"}).valueName("--fullscreen").value(false).text("Run in windowed mode."))
            .add(Option({"--list-interfaces"}).type(OptionType::LAST_ARGUMENT).text("Display list of available graphics interfaces."))
            .add(Option({"--"}).type(OptionType::LAST_OPTION).text("Mark end of options. Allows arguments starting with '-'."))
            .add(Option({"--version"}).type(OptionType::STOP).text("Show version."))
            .add(Option({"-a", "--anonymous"}).visibility(Visibility::USAGE))
            .parse(argc, argv);

    auto allArgs = args.allArguments();
    stable_sort(allArgs.begin(), allArgs.end(),
                [](auto& a, auto& b) {return a->valueId() < b->valueId();});

    std::string line;
    ArgumentView* prevArg = nullptr;
    for (auto& a : allArgs)
    {
        if (prevArg && prevArg->valueId() != a->valueId())
        {
            std::cout << line << ": \"" << args.value(*prevArg).asString() << "\"\n";
            line.clear();
        }
        if (!line.empty())
            line += ", ";
        line += a->name();
        prevArg = a.get();
    }
    if (prevArg)
        std::cout << line << ": \"" << args.value(*prevArg).asString() << "\"\n";
    line.clear();

    auto allOpts = args.allOptions();
    stable_sort(allOpts.begin(), allOpts.end(),
                [](auto& a, auto& b) {return a->valueId() < b->valueId();});
    OptionView* prevOpt = nullptr;
    for (auto& o : allOpts)
    {
        if (prevOpt && prevOpt->valueId() != o->valueId())
        {
            std::cout << line << ": \"" << args.value(*prevOpt).asString() << "\"\n";
            line.clear();
        }
        for (auto& f : o->flags())
        {
            if (!line.empty())
                line += ", ";
            line += f;
        }
        prevOpt = o.get();
    }
    if (prevOpt)
        std::cout << line << ": \"" << args.value(*prevOpt).asString() << "\"\n";
    line.clear();

    auto res = parseResolution(args.value("--resolution"));
    std::cout << "resolution: " << res.width << 'x' << res.height << '\n';

    return 0;
}
