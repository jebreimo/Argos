//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-04-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <catch2/catch_test_macros.hpp>
#include "Argos/Argos.hpp"

#include "Argv.hpp"

TEST_CASE("Test filter argc/argv")
{
    using namespace argos;
    auto parser = ArgumentParser("test")
        .auto_exit(false)
        .ignore_undefined_options(true)
        .ignore_undefined_arguments(true)
        .add(Argument("FILE"))
        .add(Option("-f"))
        .add(Option{"--g"})
        .move();
    SECTION("filter all")
    {
        Argv argv{{"test", "-f", "file"}};
        int c = argv.size();
        char** v = argv.data();
        auto args = parser.parse(c, v);
        args.filter_parsed_arguments(c, v);
        REQUIRE(c == 1);
        REQUIRE(std::string(v[0]) == "test");
    }
    SECTION("filter some")
    {
        Argv argv{{"test", "-f", "-p", "--g", "--q", "file", "text"}};
        int c = argv.size();
        char** v = argv.data();
        auto args = parser.parse(c, v);
        args.filter_parsed_arguments(c, v);
        REQUIRE(c == 4);
        REQUIRE(std::string(v[0]) == "test");
        REQUIRE(std::string(v[1]) == "-p");
        REQUIRE(std::string(v[2]) == "--q");
        REQUIRE(std::string(v[3]) == "text");
    }
}

TEST_CASE("Check options of arguments of main command and sub-commands")
{
    using namespace argos;
    auto args = ArgumentParser()
        .add(Opt("-f", "--flag"))
        .add(Cmd("cmd")
            .add(Opt("-o", "--option"))
            .add(Arg("ARG")))
        .parse({"-f", "cmd", "-o", "arg"});
    auto all_opts = args.all_options();
    REQUIRE(all_opts.size() == 2);
    REQUIRE(all_opts[0]->flags().size() == 2);
    REQUIRE(all_opts[0]->flags()[0] == "-f");
    REQUIRE(all_opts[0]->flags()[1] == "--flag");
    REQUIRE(all_opts[1]->flags().size() == 2);
    REQUIRE(all_opts[1]->flags()[0] == "-h");;
    REQUIRE(all_opts[1]->flags()[1] == "--help");
    REQUIRE(args.all_arguments().empty());
    auto cmds = args.subcommands();
    REQUIRE(cmds.size() == 1);
    auto cmd = cmds[0];
    REQUIRE(cmd.name() == "cmd");
    all_opts = cmd.all_options();
    REQUIRE(all_opts.size() == 2);
    REQUIRE(all_opts[0]->flags().size() == 2);
    REQUIRE(all_opts[0]->flags()[0] == "-o");
    REQUIRE(all_opts[0]->flags()[1] == "--option");
    REQUIRE(all_opts[1]->flags().size() == 2);
    REQUIRE(all_opts[1]->flags()[0] == "-h");
    REQUIRE(all_opts[1]->flags()[1] == "--help");
    auto all_args = cmd.all_arguments();
    REQUIRE(all_args.size() == 1);
    REQUIRE(all_args[0]->name() == "ARG");
}
