The Argos Command Line Parser
=============================

Argos is a C++ library providing classes for declaring and parsing a program's command line
interface, as well as displaying a useful help text and helpful error messages. It is intended to be
easy to use, yet support advanced command line interfaces.

Its features include:

- Automatic formatting of help texts, including word wrapping based on the terminal's width.
- A flexible parser that supports arguments of many different types like for instance `int`, `unsigned long long`, `float` and `double`.
- Takes care of most of the command line-related error handling automatically.
- Three different option styles:
    - a combination of short, `-o`, and long flags, `--option`, where multiple short
      options can be concatenated after a single dash.
    - Windows options: flags starting with a slash `/OPTION`, case sensitivity can be turned on and
      off.
    - Dash options: arbitrary flags preceded by a single dash.
- More powerful features like linking two or more options, e.g. `--quiet` and `--verbose`, are
  trivial
  with [aliases](https://barsken.no/apidoc/argos/classargos_1_1_option.html#aaca269866e4b341f215726511c2992c6)
  and [callbacks](https://barsken.no/apidoc/argos/classargos_1_1_option.html#a030255e044955d037aba30bc88b2da3f)
  .
- Tested on Linux, macOS and Windows.

# Documentation, source code and licence

The documentation for Argos can be found
at [barsken.no/apidoc/argos](https://barsken.no/apidoc/argos).

Argos is made available under
the [Zero-Clause BSD licence](https://raw.githubusercontent.com/jebreimo/Argos/master/LICENSE).

The source code can be cloned or forked on
GitHub: [github.com/jebreimo/Argos](https://github.com/jebreimo/Argos).

# Requirements

Argos is self-contained. It has no dependencies apart from a compiler that supports C++17 (
with `<string_view>`, `<variant>`, `<optional>`, structured bindings etc.). It has been tested with
recent versions gcc (9.3.0), clang (AppleClang 12) and msvc (19.29).

# Using Argos

The following is a simple "hello world" program using Argos.

~~~c++
#include <iostream>
#include <Argos/Argos.hpp>

int main(int argc, char* argv[])
{
    const argos::ParsedArguments args = argos::ArgumentParser("hello")
        .about("Displays a greeting to someone or something.")
        .add(argos::Argument("NAME").optional(true)
            .help("The person or thing to greet."))
        .add(argos::Option{"-n", "--number"}.argument("NUM")
            .help("The number of times to repeat the greeting."))
        .parse(argc, argv);

    int n = args.value("--number").as_int(1);
    for (int i = 0; i < n; ++i)
    {
        std::cout << "Hello "
                  << args.value("NAME").as_string("world")
                  << "!\n";
    }

    return 0;
}
~~~

By default it displays "Hello world!", but it also takes an optional argument, `NAME`, which
replaces `world` when it is provided (the complete example including build files can be found in
the [Argos repository](https://github.com/jebreimo/Argos/tree/master/examples/hello).

~~~
~/Argos/examples/tutorial/build $ ./hello
Hello world!
~/Argos/examples/tutorial/build $ ./hello planet  
Hello planet!
~/Argos/examples/tutorial/build $ ./hello -n 2                                                          
Hello world!
Hello world!
~/Argos/examples/tutorial/build $ ./hello -h
USAGE
  hello --help
  hello [-n <NUM>] [<NAME>]

Displays a greeting to someone or something.

ARGUMENTS
  [<NAME>]                 The person or thing to greet.

OPTIONS
  -n <NUM>, --number <NUM> The number of times to repeat the greeting.
  -h, --help               Display the help text.
~~~

The interface of Argos is centred upon two classes: `ArgumentParser` and `ParsedArguments`. `ArgumentParser`
is where the arguments and options are defined, as well as where various configuration settings are set (e.g. `ArgumentParser::option_style()` to for instance change to Windows-style options, or `ArgumentParser::allow_abbreviated_options()` to allow using less than the complete flag of options, as long as the
option is uniquely identified). `ParsedArguments` is the result of `ArgumentParser::parse()`, and is where
the values of the arguments and options can be retrieved.

Arguments and options are defined by adding instances of `Argument` and `Option` to the `ArgumentParser`.
`Argument`'s constructor takes the name that will appear in the help text as its argument, `Option`'s
constructor takes a list of flags. Both classes have several properties that define their behaviour,
important ones include `help` which sets the help text and `argument` which sets the name of an
option's argument.

Unlike some other argument parsers, the value types and default values are not declared along with
the arguments and options in the ArgumentParser, instead they are specified when the values are
retrieved. The line

~~~c++
    int n = args.value("--number").as_int(1);
~~~

converts the value of the `--number` option to int and assigns it to `n` (all values are stored as
strings internally). If the option was not given on the command line, the provided default
value, `1`, is assigned to `n` instead. `value("-n")` could also have been used in place
of `value("--number")`.

Similarly,

~~~c++
args.value("NAME").as_string("world")
~~~

returns the value of the argument `NAME` as a `std::string`, or "world" if it wasn't given.

Running the program with one of the two auto-generated help options, `-h` and `--help`, displays the help text.

The help text looks a little bit sparse when there are so few arguments and options as in the above example, but the empty
lines will start to make more sense when there are a more significant number of options and
arguments, here is for instance the help message for the `whereis` example (
in [examples/whereis](https://github.com/jebreimo/Argos/tree/master/examples/whereis)):

~~~
~/Argos/examples/whereis/build $ ./whereis -h
USAGE
  whereis --help
  whereis --version
  whereis [-p <PATH>[:<PATH>]...] [-e <EXT>[:<EXT>]...] [--] [-q] [-v]
          <FILE> [<FILE>]...

Searches the directories in the PATH environment variable for the given
file (or files).

ARGUMENTS
  <FILE> [<FILE>]...
        The file or files to locate.

MAIN OPTIONS
  -p <PATH>[:<PATH>]..., --paths <PATH>[:<PATH>]...
        Search the given path or paths rather than the ones in the PATH
        environment variable. Use : as separator between the different
        paths.
  -e <EXT>[:<EXT>]..., --extensions <EXT>[:<EXT>]...
        File name extensions to test while looking for FILE. Must include
        the leading '.'. This option can be used multiple times, multiple
        extensions can be set at once by separating them with ':'.
  --
        Marks the end of the options. Makes it possible to look for file
        names starting with dashes ('-').

OTHER OPTIONS
  -q, --quiet
        Do not show additional information (negates --verbose).
  -v, --verbose
        Show additional information.
  -h, --help
        Display the help text.
  --version
        Display the program version.
~~~

Running the program with any other flag, or incorrect number of arguments, results in an error
message followed by the USAGE section of the help text:

~~~
~/Argos/examples/tutorial/build $ ./hello --option
hello: Unknown option: --option
USAGE
  hello --help
  hello [<NAME>]

~/Argos/examples/tutorial/build % ./hello planet Earth
hello: Too many arguments, starting with "Earth".
USAGE
  hello --help
  hello [<NAME>]

~/Argos/examples/tutorial/build % ./hello -n foo
hello: -n, --number: Invalid value: foo.
USAGE
  hello --help
  hello [-n <NUM>] [<NAME>]
~~~

Notice how there is no error handling code in the program itself, everything is taken care of by
Argos. After Argos has displayed the error message (or help message) it calls `exit()` to end the
program. The automatic exit is convenient in many cases, but when this is undesired, it can easily
be turned off with

~~~c++
    const argos::ParsedArguments args = argos::ArgumentParser("hello")
        .auto_exit(false)
        ...
~~~

# More examples

There are several more examples on how to use Argos in
the [examples folder](https://github.com/jebreimo/Argos/tree/master/examples).

# Building Argos

Building the program, or any program using Argos, can be done in multiple ways. My preferred way in
CMake-based projects is to use the FetchContent module to automatically download Argos from its
GitHub repository. For the hello program, the CMakeLists.txt would have to contain the following:

~~~cmake
cmake_minimum_required(VERSION 3.14)
project(hello)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    GIT_TAG "master"
    )
FetchContent_MakeAvailable(argos)

add_executable(hello hello.cpp)

target_link_libraries(hello
    PRIVATE
    Argos::Argos
    )
~~~

Alternatively one can clone the git repository and build and install it with CMake, for instance:

~~~shell
git clone https://github.com/jebreimo/Argos.git
mkdir Argos/build
cd Argos/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make install
~~~

With CMake one can then
use `find_package(Argos)` and the same `target_link_libraries` as in the example above.

If one doesn't care about keeping a link to the Argos git repository, but just want to copy the Argos files
into the current project along with all the other files, there is a special two-file version of
Argos in the `single_source` directory, consisting of just `Argos.hpp` and `Argos.cpp`. They are the
concatenations of the files in the `include` and `src` respectively.
