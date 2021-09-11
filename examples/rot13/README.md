rot13
=====

Uses the [rot13 algorithm](https://en.wikipedia.org/wiki/ROT13) to obfuscate
(or reveal) the program's arguments.

Example:
~~~
~/Argos/examples/rot13/build % ./rot13 The key is hidden behind the painting.
Gur xrl vf uvqqra oruvaq gur cnvagvat.
~~~

Help text:

~~~
USAGE
  example2 --help
  example2 [-v] [-n <NUM>] [--] <WORD> [<WORD>]...

Obfuscates (or reveals) text with the rot-13 algorithm.

ARGUMENTS
  <WORD> [<WORD>]...
        One or more words.

OPTIONS
  -v, --verbose
        Display additional information.
  -n <NUM>, --number <NUM>
        Set the number letters are rotated by. Default is 13.
  --
        Marks the end of the options. Allows words to start with dashes
        ('-').
  -h, --help
        Display the help text.
~~~
