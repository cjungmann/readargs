#include <stdio.h>

#include "readargs.h"

// Variables to be set with readopts library:
const char *filepath = NULL;
int        flag = 0;
int        number = 0;
const char *input_file = NULL;
int        repetitions = 1;

raOpt options[] = {
   {'h', "help",    NULL,    "This help output",     &ra_show_help_agent,  NULL },
   {'v', "version", NULL,    "Show current version.",NULL,                 NULL },
   {'s', "show",    NULL,    "Show option values",   &ra_show_values_agent,NULL },
   {'f', "flag",    NULL,    "set the flag.",        &ra_flag_agent,       &flag },
   {'n', "number",  "NUMBER","Set my_number value",  &ra_int_agent,        &number },
   {'p', "filepath","FILEPATH","Set file path.",     &ra_string_agent,     &filepath },
   { -1, "*input",  "FILE",  "input file",           &ra_string_agent,     &input_file },
   { -1, "*reps",   "NUMBER","repetitions",          &ra_int_agent,        &repetitions }
};

int main(int argc, const char **argv)
{
   // Prepare the static library environment.
   // The *scene* must be set before we can use
   // any of the library output, so we don't check
   // the argc value until the scene is set.
   ra_set_scene(argv, argc, options, OPTS_COUNT(options));

   if (argc > 1)
   {
      // Use the simplest processing
      if (ra_process_arguments())
      {
         printf("The arguments have been processed.\n"
                "\n"
                "The following report shows how the\n"
                "variables have been affected by your\n"
                "command line options.\n\n");

         ra_show_scene_values(stdout);
      }
   }
   else
      ra_show_no_args_message();

   return 0;
   
}


