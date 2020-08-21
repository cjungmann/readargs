#include <stdio.h>
#include <unistd.h>   // for access()
#include <alloca.h>

#include "readargs.h"

/** This is a test if a filename exists and is readable.
 *
 * This is used to interpret a string that may or may not be meant
 * for an optional-valued option.
 */
int is_appropriate_input_file(const char *filepath)
{
   return !access(filepath, R_OK);
}

raStatus filepath_optional_reader(const raAction *act, const char *str, raTour *tour)
{
   // Make sure default value is valid after function returns!
   static const char *default_value = "/etc/hosts";

   if (!act->target)
      return RA_MISSING_TARGET;

   // Cast target for eventual assignment
   const char **target = (const char **)act->target;
   *target = NULL;

   if (str == NULL)                       // if no string
   {
      *target = default_value;
      return RA_SUCCESS;
   }
   else if ( (ra_seek_raAction(str, tour))       // str, but is option
             || !is_appropriate_input_file(str)) // str, but not a readable file
   {
      *target = default_value;                   // ignore str and signal success
      ra_retreat_arg(tour);
      return RA_SUCCESS;
   }
   else                                          // str is usable
   {
      *target = str;
      return RA_SUCCESS;
   }

   return RA_SUCCESS;
}

void filepath_optional_writer(FILE *f, const raAction *act)
{
   const char **target = (const char **)act->target;
   if (*target == NULL)
      fprintf(f, "(null)");
   else
      fprintf(f, "%s", *target);
}

/** raReader implementation for param_agent. 
 *
 * This uses the simplest test: both values must be available,
 * an no checking is done on the values.
 *
 * There are other options:
 * - There could be a default value for a single-valued option call.
 * - There could be an option test (see if either str or str2 could
 *   be interpreted as another option.
 * - The program could continue after an error, but that would
 *   entail deciding on how to dispose of str and str2, if found.
 */
raStatus param_reader(const raAction *act, const char *str, raTour *tour)
{
   const char **lpair = (const char**)act->target;

   // Preserve tour progress by using a copy
   raTour saved_tour = *tour;
   const char *str2 = ra_advance_arg(tour);

   if (str && str2)
   {
      lpair[0] = str;
      lpair[1] = str2;
      return RA_SUCCESS;
   }
   else
   {
      fprintf(stderr, "Not enough parameters for --param option.\n");
      ra_describe_usage(stderr, 0, RAU_DEFAULT);
      *tour = saved_tour;
      return RA_CANCEL;
   }
}

void param_writer(FILE *f, const raAction *act)
{
   const char **lpair = (const char**)act->target;

   if (lpair[1] == NULL)
   {
      if (lpair[0] == NULL)
         fprintf(f, "(null):(null)");
      else
         fprintf(f, "%s:(null)", lpair[0]);
   }
   else
      fprintf(f, "%s:%s", lpair[0], lpair[1]);
}

raAgent filepath_optional_agent = { 1, filepath_optional_reader, filepath_optional_writer };
raAgent param_agent = { 1, param_reader, param_writer };


// Variables to be set with readacts library:
const char *filepath = NULL;
const char *section = NULL;
const char *name = NULL;
const char *ppair[2] = {NULL,NULL};

raAction actions[] = {
   {'h', "help",  "This help output",   &ra_show_help_agent },
   {'s', "show",  "Show action values", &ra_show_values_agent },
   {'i', "input", "input file",         &filepath_optional_agent, &filepath, "FILEPATH" },
   {'p', "param", "Set param value",    &param_agent,             &ppair,    "NAME VALUE" }
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
         if (ppair[0] == NULL)
         {
            RA_XA_ENV env;
            const raAction *ract = ra_seek_raAction_by_letter('p');
            ra_print_interactive_prompt(ract);
            RA_READ_XA_RESPONSE(env);
            RA_PROCESS_XA_ACTION(ract, env);
         }

         ra_show_scene_values(stdout);
      }
   }
   else
      ra_show_no_args_message();

   return 0;
   
}


