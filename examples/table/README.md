table
=====

A command line utility that build an ASCII table from its arguments. It
demonstrates how option and argument callbacks can be used with Argos.

Example of use:

~~~
~/Argos/examples/table/build % ./table foo -c bar -r boom bang -r -cc trim --borders
foo |    |bar 
----+----+----
boom|bang|    
----+----+----
    |    |trim
~~~

Help text:

~~~
USAGE
  table --help
  table [-o <FILE>] [-r] [-c] [--borders] <TEXT> [<TEXT>]...

Prints the arguments as cells in a table. The program also demonstrates how
option and argument callbacks can be used.

ARGUMENTS
  <TEXT> [<TEXT>]...
        Text of the next table cell.

OPTIONS
  -o <FILE>, --output <FILE>
        File name for output. stdout is used by default.
  -r, --row
        Next file will be placed at the beginning of a new row.
  -c, --column
        Skip one column forward.
  --borders
        Print borders between cells.
  -h, --help
        Display the help text.
~~~
