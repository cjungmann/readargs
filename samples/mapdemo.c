// Build with "gcc -o mapdemo mapdemo.c -lreadargs
#include <stdio.h>
#include <readargs.h>

// State Variables definition.
int        nage = 0;
int        fcolor = 0;
const char *greeting = "Hello";
const char *sname = NULL;

// *Note 2: Action Map definition.
raAction actions[] = {
   { 'a', NULL,      "Set age",      &ra_int_agent,    &nage ,    "NUMBER" },
   { 'c', "colorize","Colorize text",&ra_flag_agent,   &fcolor,   "TRUE"   },
   { 'g', "",      "Set greeting" ,&ra_string_agent, &greeting },
   { -1,  "*name",   "Set name",     &ra_string_agent, &sname,    "STRING" },
   { 'h', "help",    "This display", &ra_show_help_agent   },
   { 's', "show",    "Show state",   &ra_show_values_agent }
};

int main(int argc, const char **argv)
{
   const char *coloron = "";
   const char *coloroff = "\x1b[m";

   // *Note 3: Initialize the library.
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));

   // *Note 4: Call library to process arguments.
   if (ra_process_arguments())
   {
      if (fcolor)
         coloron = "\x1b[32;1m";

      printf("%s%s%s, %d-year-old %s%s%s.\n",
             coloron, greeting, coloroff,
             nage,
             coloron, sname, coloroff);
   }
   return 0;
}

/* Local Variables: */
/* compile-command: "b=mapdemo; \*/
/*  cc -Wall -Werror -ggdb      \*/
/*  -std=c99 -pedantic          \*/
/*  -o $b ${b}.c                \*/
/*  ../libreadargs.so"          \*/
/* End: */

