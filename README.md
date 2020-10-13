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
The following code lising is a complete program that can be compiled with
the **gcc** `&#x2011;lreadargs` option.

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

- **Easy-to-scan, Compact Code**  
  Code all argument handling in a single, easy-to-scan structure.
  Short- and long-options and non-optional arguments handled
  in a consistent manner.

- **Generates Help and Usage Displays**  
  Allow display generator to provide help, or use library tools
  to build a custom help or usage display.

- **Flexible**  
  If builtin features neglect a project's needs, the developer
  can exploit library hooks to handle custom data types.  The
  documentation shows how to code for new data types, optional
  argument, and multiple argument options.

- **Easy to learn**  
  Info page document included that serves as a learning tool for
  novice users and as quick reference for experienced users.

- **Debugging Tool**  
  A feature that displays the values of the variables under
  the library's control is helpful to developers and end users
  to confirm that the program is running with appropriate
  settings.

- **Interactive Support**
  Elements of the *action map* can be used to request and
  process user input interactively.  This may be used if a
  user forgets to enter required information.

## Installation

At this point, this is designed on and for use in the Linux
environment.  If there is any interest, I may consider supporting
other environments.  Given an appropriate system, these steps
will install this library:

~~~sh
git clone https://github.com/cjungmann/readargs
cd readargs
make
sudo make install
~~~

For the undecided, there are program examples in the `src` directory
that are prefixed with *test_*.  These programs are compiled
along with the rest of the library when calling `sudo make install`.

The documentation can be installed separately for those who
would like to preview the software before installing it.  The documentation
refers to additional sample programs in the `samples` directory.

Install and read the info file without installing the library:

~~~sh
sudo make install-docs
info readargs
~~~

## Using the Library

There are three requirements for using the library:

1 Build and install the library.
2 Add `#include <readargs.h>` statement to your C or C++ code.
3 Add the `-lreadargs` to your link options.

Refer to `sample/*.c` and `src/test_*.c` files for development models.

## Standards and Conventions

I began by attempting to faithfully follow the guidelines presented in the
[GNU Arguments Standards](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html#Argument-Syntax).
I looked at various command line programs to find conventions where
the standards are slient or ambiguous.

For example, the standard says that a long option should be followed
by an '*=*' character and the option value, without intervening
spaces (`--action=simple`).  I noticed that **grep** recognizes
a long option whose value is in the following argument
(`grep --file patterns.txt` is the same as `grep --file=patterns.txt`).
The library handles long options like **grep**.

### Usage Message Syntax

The most helpful document about usage syntax is a [wiki page](https://en.wikipedia.org/wiki/Usage_message)

### Standards Research

While I have used command line options for a long time, I have
never paid much attention to how command line options work.  The
following command are among those considered when deciding how
to interpret various option constructions:

- **Commands References**  
  `help` without arguments shows a list of builtin commands.
  I experimented with commands in the list to see how GNU standard
  commands handle conform to the GNU argument syntax standards.

- **Extension: Multiple Argument Options**  
  `xsltproc` has options that takes two arguments.  I referred to
  `xsltproc --help` to see how these options, especially *--param*,
  are used and how they are documented.  Refer to
  `info readargs "Advanced Topics" "Multi-value Agent"` for an
  implementation of this feature.

- **Extension: Optional Argument Options**  
  I was unable to find examples of this construction.   Although
  this feature may be rare, the info file includes two examples
  of how one might implement optional value arguments.  Look at
  `info readargs "Advanced Topics" "Optional-value Agent"`

- **Repeated Options**  
  The GNU standards allow for repeated options.  By default, The
  *readargs* library overwrites the target variable with each
  repitition, ultimately leaving the target variable set to the
  last setting.

  `sed` recognizes multiple invocations of the `-e` option.  The info
  file includes an example of how a developer might process repeated
  options in `info readargs "Advanced Topics" "Repeating Option"`.
  

- **Help Display**  
  I looked at the `--help` option of `grep`, `sed`, `xsltproc`,
  and others, for help display models, especially for long options.

