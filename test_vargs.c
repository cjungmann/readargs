#include <stdio.h>

#include "readargs.h"

/** This function signals whether a string is appropriate for the option.
 *
 * This is used to interpret a string that may or may not be meant
 * for an optional-valued option.
 */
int str_suits_positional_option(const raOpt *opt, const char *str)
{
   return 0;
}

raStatus filepath_optional_reader(const raOpt *opt, const char *str, raTour *tour)
{
   static const char *default_value = "./";
   // Cast target for eventual assignment
   const char **target = (const char **)opt->target;

   // If there is no *str, these is no mystery: use the default option value.
   if (str == NULL)
      *target = default_value;
   else
   {
      const raOpt *valueopt = ra_seek_raOpt(str, tour);

      // If there is no *valueopt, the value is *str
      if (valueopt == NULL)
         *target = str;
      else if (!ra_is_positional_option(valueopt)
               || (str_suits_positional_option(valueopt, str)))
      {
            // Reuse the str as the next options by pushing it back:
            ra_retreat_arg(tour);

            // Since we're not using the value, we fall back to the default value
            *target = default_value;
      }
      else
      {
         // even though there is a position option available,
         // the value is not appropriate for that option, so
         // allow the str to be used as the current option value.
         *target = str;
      }
   }

   return RA_SUCCESS;
}

void filepath_optional_writer(FILE *f, const raOpt *opt)
{
   const char **target = (const char **)opt->target;
   if (*target == NULL)
      fprintf(f, "(null)");
   else
      fprintf(f, "%s", *target);
}

typedef struct ppair
{
   const char *name;
   const char *value;
} PPair;

raStatus param_reader(const raOpt *opt, const char *str, raTour *tour)
{
   PPair *ppair = (PPair* )opt->target;

   // Preserve tour progress by using a copy
   raTour saved_tour = *tour;
   const char *str2 = ra_advance_arg(tour);

   if (str && str2)
   {
      ppair->name = str;
      ppair->value = str2;
      return RA_SUCCESS;
   }
   else
   {
      *tour = saved_tour;
      return RA_END_ARGS;
   }
}

void param_writer(FILE *f, const raOpt *opt)
{
   const PPair *ppair = (const PPair* )opt->target;

   if (ppair->name == NULL)
      fprintf(f, "(null)");
   else
      fprintf(f, "%s", ppair->name);

   fprintf(f, " : ");

   if (ppair->value == NULL)
      fprintf(f, "(null)");
   else
      fprintf(f, "%s", ppair->value);
}

raAgent filepath_optional_agent = { 1, filepath_optional_reader, filepath_optional_writer };
raAgent param_agent = { 1, param_reader, param_writer };


// Variables to be set with readopts library:
const char *filepath = NULL;
const char *section = NULL;
const char *name = NULL;
PPair ppair = {NULL,NULL};

raOpt options[] = {
   {'h', "help",    "This help output",     &ra_show_help_agent },
   {'s', "show",    "Show option values",   &ra_show_values_agent },
   {'f', "filepath","Set file path",        &filepath_optional_agent, &filepath,   "FILEPATH" },
   {'p', "param",   "Set param value",      &param_agent,             &ppair }
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


