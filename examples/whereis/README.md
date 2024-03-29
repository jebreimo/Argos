whereis
=======

This program demonstrates some of Argos' capabilities, including:

- `--verbose` and `--quiet` options that toggle the same value
- the automatic version option
- use of list options and splitting strings on a separator
- variable number of arguments
- use of `Option::initial_value`
- use of option type LAST_OPTION to allow arguments starting with dashes
- use of ArgumentParser::section to set section headings for the options 

To build it with CMake on Unix-like operating systems (e.g Mac or Linux), cd
into folder where whereis is located and run the following commands:

~~~shell
mkdir build
cd build
cmake ..
make
~~~

The result will be a program, `whereis`, which can be used to locate where
programs are located among the folders in the current PATH environment
variable.

Running the program will yield results similar to this this:

~~~s
$ ./whereis python
/usr/local/bin/python
/usr/bin/python
~~~

and giving the help option (either `-h` or `--help`):

~~~
$ ./whereis -h
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
