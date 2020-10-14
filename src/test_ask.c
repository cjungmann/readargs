#include <stdio.h>
#include <string.h>
#include <alloca.h>

#include "readargs.h"

// Variables to be set with readacts library:
const char *password = NULL;
int        flag = 0;
int        number = 0;
const char *input_file = NULL;
int        repetitions = 1;

raAction actions[] = {
   {'h', "help",    "This help output",     &ra_show_help_agent,  NULL,        NULL       },
   {'v', "version", "Show current version.",NULL,                 NULL,        NULL       },
   {'s', "show",    "Show action values",   &ra_show_values_agent,NULL,        NULL       },
   {'f', "flag",    "set the flag.",        &ra_flag_agent,       &flag,       NULL       },
   {'n', "number",  "Set my_number value",  &ra_int_agent,        &number,     "NUMBER"   },
   {'p', "password","Account password.",    &ra_string_agent,     &password,   "PASSWORD" },
   { -1, "*input",  "input file",           &ra_string_agent,     &input_file, "FILE"     },
   { -1, "*reps",   "repetitions",          &ra_int_agent,        &repetitions,"NUMBER"   }
};

int main(int argc, const char **argv)
{
   // Prepare the static library environment.
   // The *scene* must be set before we can use
   // any of the library output, so we don't check
   // the argc value until the scene is set.
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));

   if (argc > 1)
   {
      // Use the simplest processing
      if (ra_process_arguments())
      {
         printf("[34;1mBefore getting extras.[m\n");
         ra_show_scene_values(stdout);

         printf("[34;1mGetting extras.[m\n");
         RA_XA_ENV env;
         if (!password)
         {
            const raAction *ract = ra_seek_raAction_by_letter('p');
            ra_print_interactive_prompt(ract);
            RA_READ_XA_RESPONSE(env);
            RA_PROCESS_XA_ACTION(ract, env);
         }

         printf("[34;1mJust got extras[m\n");
         ra_show_scene_values(stdout);
         printf("[34;1mFinished second show_scene[m\n");
      }
   }
   else
      ra_show_no_args_message(stderr);

   return 0;
   
}


