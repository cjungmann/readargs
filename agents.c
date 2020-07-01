#include "readargs.h"

#include <stdlib.h>   // for atoi

void ra_error_missing_target(const char *target_name)
{
   fprintf(stderr, "%s: agent requires cached target", g_command_name);
   if (target_name)
      fprintf(stderr, " (%s)", target_name);
   fprintf(stderr, "\n");
}

int confirm_option_target(const raOpt *opt, const char *target_name)
{
   if (opt->target)
      return 1;
   else
   {
      ra_error_missing_target(target_name);
      return 0;
   }
}


/*** construct ra_int_agent  */
void int_reader(const raOpt *opt, const char *str)
{
   if (confirm_option_target(opt, "int agent"))
      *(int*)opt->target = atoi(str);
}

void int_writer(FILE *f, const raOpt *opt)
{
   if (confirm_option_target(opt, "int agent"))
      fprintf(f, "%d", *(int*)opt->target);
}

EXPORT const raAgent ra_int_agent = { 1, int_reader, int_writer };


/** construct ra_string_agent */

void string_reader(const raOpt *opt, const char *str)
{
   if (confirm_option_target(opt, "string agent"))
      *(const char**)opt->target = str;
}

void string_writer(FILE *f, const raOpt *opt)
{
   if (confirm_option_target(opt, "string agent"))
      fprintf(f, "%s", *(const char**)opt->target);
}

EXPORT raAgent const ra_string_agent = { 1, string_reader, string_writer };

/** default --help agent */

void show_help_reader(const raOpt *opt, const char *str)
{
   ra_show_options(stdout, g_cache);
}

EXPORT raAgent const ra_show_help_agent = { 0, show_help_reader, NULL };
