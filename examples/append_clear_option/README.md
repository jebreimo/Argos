Append and clear options
========================

This project demonstrates how to make an option, `--include`,
where `--include=VALUE` appends `VALUE` to a list, and just `--include`
on its own clears the same list. Having an option negate or clear
the result of other options can make sense when a program is run via
an alias or script.

~~~
$ ./append_clear_option --include=foo --include=bar
foo
bar
$ ./append_clear_option --include=foo --include=bar --include
$ ./append_clear_option --include=foo --include=bar --include --include=baz
baz
~~~
