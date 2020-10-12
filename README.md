# Project readargs

This is a command line arguments processing library written in,
and intended for use with, the C langauge.  It provides both
a means to parse and save command line arguments **and** to generate
a useful help screen to document the available actions.

My goal is to allow for a simple use case using only a few library
functions, but to also allow customization for special cases.

## Example

Let the library parse the command line argument.  Define an action
map of options, initialize the library, then call `ra_process_arguments()`.

~~~c
#include <stdio.h>
#include <readargs.h>

const char *greeting = "Hello";
const char *name = "World";

raAction action_map[] = {
   { 'g', "greeting", "Set greeting", &ra_string_agent, &greeting, "STRING" }
   { 'n', "name", "Set name", &ra_string_agent, &name },
   { 'h', "help", "This display", &ra_show_help_agent },
};

int main(int argc, const char **argv)
{
   ra_set_scene(argv, argc, action_map, ACTS_COUNT(action_map));

   if (ra_process_arguments())
   {
      printf("%s, %s.\n", greeting, name);
   }
   return 0;
}

~~~

## Why Use This Library?

With standard **getopt** and **getopt_long** and countless other
argument processing tools, one might not think we need another
library.  The benefits of this library are:

- **Follows Parsing Standards**  
  Command line arguments are parsed according to widely-recognized
  GNU long option extension to the [POSIX standards for handling
  command line arguments](http://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html\#Argument-Syntax).

- **Easy-to-scan Compact Code**  
  The basic code requirements are minimal.  The developer defines
  an *action map* which a library function uses to set *state variables*
  from the command line arguments.  Each line of an *action map*
  represents one *state variable* and is easier to read than a
  typical long *switch* statement.

- **Generates Help and Example Displays**  
  There are several tools from which the develper can generate
  help and usage displays.

- **Extensible**  
  There are hooks in the code by which a developer can handle
  unusual data or option types.

- **Debugging Tool**  
  The *state variables* registered in the *action map* can
  be displayed by simply including a special action in the
  *action map.*  This facility can be very useful for both
  the developer and the end user to confirm *state variable*
  settings.

- **Interactive Support**
  Elements of the *action map* can be used to request and
  process user input interactively.  This may be used if a
  user forgets to enter required information.

## Documentation

There is a set of **texinfo** files that generate an **info**
file in which the documentation can be found.

Beyond the **info** file, a developer can analyze included
compilable C source files that demonstrate how to accomplish
certain development goals.  These files are found with the
rest of the project's source code, and they all have a prefix
of **test_**.

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

raAction actions[] = {
   {'h', "help",     "This help output",   &ra_show_help_agent,   NULL,        NULL       },
   {'s', "show",     "Show action values", &ra_show_values_agent, NULL,        NULL       },
   {'p', "filepath", "Set file path.",     &ra_string_agent,      &filepath,   "FILEPATH" },
   { -1, "*iter",    "iterations",         &ra_int_agent,         &iterations, "NUMBER"   }
};

int main(int argc, const char **argv)
{
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));
   if (argc > 1)
   {
      if (ra_process_arguments())
      {
         // The variables are set, begin the program.

         // This function just shows the current action values.
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
by an '*=*' character and the option value, without intervening
spaces (`--action=simple`).  I noticed that **grep** recognizes
a long option whose value is in the following argument
(`grep --file patterns.txt` is the same as `grep --file=patterns.txt`).

### Usage Message Syntax

The most helpful document about usage syntax is a [wiki page](https://en.wikipedia.org/wiki/Usage_message)

### Standards Research

While I have used command line options for a long time, I have
never paid much attention to how command line options work.  While
I add features to my version, I am referring to other established
commands to see how they handle bad or incomplete options entry.

I have used the following examples.  Type the `highlighted text`
into a Linux console to see what I'm considering.

- `grep --help` for an example of *--help* output
- `help` alone to see builtin commands, from which some
  examples might be found.
- `help cd` get help on one of the *help* builtins
- `xsltproc --help` to see options, especially *--param*,
  which takes two values.

## Installation

At this point, this is designed on and for use in the Linux
environment.  If there is any interest, I may consider supporting
other environments.  Given an appropriate system, these steps
will install this library:

- `git clone https://github.com/cjungmann/readargs`
- `cd readargs`
- `make`
- Run test commands without further installation
- Install for system-wide use: `sudo make install`
- Install documentation only: `sudo make install-docs`
  - Call `info readargs` to read the info file.
- Uninstall with `sudo make uninstall`
