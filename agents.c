#include "readargs.h"
#include "invisible.h"

#include <stdlib.h>   // for atoi()
#include <string.h>   // for strchr()

int str_is_number(const char *str)
{
   static const char numlist[] = "-0123456789";
   const char *ptr = strchr(numlist, *str);
   if (ptr && *ptr == '-')
      ptr = strchr(&numlist[1], *(str+1));

   return ptr != NULL;
}

int confirm_option_target(const raOpt *opt) { return opt->target != NULL; }

/** ra_flag_agent function implementations */
raStatus flag_reader(const raOpt *opt, const char *str)
{
   if (confirm_option_target(opt))
   {
      *(int*)opt->target = 1;
      return RA_SUCCESS;
   }
   else
      return RA_MISSING_TARGET;
}

void flag_writer(FILE *f, const raOpt *opt)
{
   if (confirm_option_target(opt))
      fprintf(f, "%s", (*(int*)opt->target ? "true": "false" ));
}

/** ra_int_agent function implementations  */
raStatus int_reader(const raOpt *opt, const char *str)
{
   if (confirm_option_target(opt))
   {
      if (str_is_number(str))
      {
         *(int*)opt->target = atoi(str);
         return RA_SUCCESS;
      }
      else
         return RA_INVALID_ARGUMENT;
   }
   else
      return RA_MISSING_TARGET;
}

void int_writer(FILE *f, const raOpt *opt)
{
   if (confirm_option_target(opt))
      fprintf(f, "%d", *(int*)opt->target);
}

/** ra_string_agent function implementations */
raStatus string_reader(const raOpt *opt, const char *str)
{
   if (confirm_option_target(opt))
   {
      *(const char**)opt->target = str;
      return RA_SUCCESS;
   }
   else
      return RA_MISSING_TARGET;
}

void string_writer(FILE *f, const raOpt *opt)
{
   if (confirm_option_target(opt))
   {
      const char *val = *(const char**)opt->target;
      if (val)
         fprintf(f, "%s", val);
      else
         fprintf(f, "(null)");
   }
}

/** default --help agent */
raStatus show_help_reader(const raOpt *opt, const char *str)
{
   ra_show_help(stdout, 2, RAU_DEFAULT);
   return RA_CANCEL;
}

/** show option values */
raStatus show_option_values_reader(const raOpt *opt, const char *value)
{
   fputs("\nCurrent option values:\n", stdout);
   ra_show_scene_values(stdout);
   putc('\n', stdout);
   return RA_SUCCESS;
}

EXPORT const raAgent ra_flag_agent        = { 0, flag_reader, flag_writer };
EXPORT const raAgent ra_int_agent         = { 1, int_reader, int_writer };
EXPORT const raAgent ra_string_agent      = { 1, string_reader, string_writer };
EXPORT const raAgent ra_show_help_agent   = { 0, show_help_reader, NULL };
EXPORT const raAgent ra_show_values_agent = { 0, show_option_values_reader, NULL };
