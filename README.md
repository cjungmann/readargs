# Project readargs

This is a command line arguments processing library, written in
and intended for use with, the C langauge.

It is the result of the development of ideas through a series
of similar libraries.

## Standards Followed

This project processes the command line arguments strictly
according to the rules set out for GNU option processing,
which handles single-letter options preceded by a single dash,
as well as more free-form option names that are preceded by
a double-dash.

[GNU Arguments Standards](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html#Argument-Syntax)

### Examples Representing Standards

Try the following in a console:

- `grep --help` for the useful command
- `help` alone to see builtin commands
- `help cd` get help on one of the `help` builtins


