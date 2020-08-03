# Project readargs

This is a command line arguments processing library, written in
and intended for use with, the C langauge.  It provides both
a means to parse and save command line arguments **and** to generate
a useful help screen to document the available options.

My goal is to allow for a simple use case using only a few library
functions, but to also allow customization for special cases.

At first, the "documentation" will be sample C programs that
demonstrate various customizations.  <strike>Eventually, I'll make
simple **man** pages to help me remember syntax and variations.</strike>
I am working on an **info** manual according to the recommendation
in the [GNU Standards Document](https://www.gnu.org/prep/standards/html_node/GNU-Manuals.html)

## Sample Files

The sample source files will be named **test_.c**, starting with
**test_simple.c** for a standard case.

## Standards Followed

This project processes the command line arguments strictly
according to the rules set out for GNU option processing,
which handles single-letter options preceded by a single dash,
as well as more free-form option names that are preceded by
a double-dash.

[GNU Arguments Standards](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html#Argument-Syntax)

The library allows for an exception from these standards:
the standard says that a long option should be followed
by an *'='* character and the option value, without intervening
spaces (`--option=simple`).  I noticed that **grep** recognizes
a long option whose value is in the following argument
(`grep --file patterns.txt` is the same as `grep --file=patterns.txt`).

### Examples Representing Standards

While I have used command line options for a long time, I have never
paid much attention to how command line options work.  While I
add features to my version, I refer to how other well-known
commands handle unusual options.

Try the following in a console:

- `grep --help` for an example of *--_help* output
- `help` alone to see builtin commands, from which some
  examples might be found.
- `help cd` get help on one of the *help* builtins
- `xsltproc --help` to see options, especially *--param*,
  which takes two values.


