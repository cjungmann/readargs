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

## Simple Example

The following is a minimal example of a program using
**readargs** to process the command line arguments. It is
a shortened version of the **test_simple.c** file included
in the project.

One of the features of this library is generated documentation
displays.  There are functions for generating a usage display
and a help display.  In this program listing, these functions
called by the *ra_show_help_agent* which is invoked when a user
enters a *-h* or *--help** option.

~~~c
#include <stdio.h>
#include <readargs.h>

const char *filepath = NULL;
int        iterations = 1;

raOpt options[] = {
   {'h', "help",     "This help output",   &ra_show_help_agent,   NULL,        NULL       },
   {'s', "show",     "Show option values", &ra_show_values_agent, NULL,        NULL       },
   {'p', "filepath", "Set file path.",     &ra_string_agent,      &filepath,   "FILEPATH" },
   { -1, "*iter",    "iterations",         &ra_int_agent,         &iterations, "NUMBER"   }
};

int main(int argc, const char **argv)
{
   ra_set_scene(argv, argc, options, OPTS_COUNT(options));
   if (argc > 1)
   {
      if (ra_process_arguments())
      {
         // The variables are set, begin the program.

         // This function just shows the current option values.
         ra_show_scene_values(stdout);
      }
   }
   else
      ra_show_no_args_message();

   return 0;
}
~~~

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


