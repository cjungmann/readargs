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

int confirm_action_target(const raAction *act) { return act->target != NULL; }

/** ra_flag_agent function implementations */
raStatus flag_reader(const raAction *act, const char *str, raTour *tour)
{
   if (confirm_action_target(act))
   {
      *(int*)act->target = 1;
      return RA_SUCCESS;
   }
   else
      return RA_MISSING_TARGET;
}

void flag_writer(FILE *f, const raAction *act)
{
   if (confirm_action_target(act))
      fprintf(f, "%s", (*(int*)act->target ? "true": "false" ));
}

/** ra_int_agent function implementations  */
raStatus int_reader(const raAction *act, const char *str, raTour *tour)
{
   if (!str)
      return RA_MISSING_VALUE;

   if (confirm_action_target(act))
   {
      if (str_is_number(str))
      {
         *(int*)act->target = atoi(str);
         return RA_SUCCESS;
      }
      else
         return RA_INVALID_ARGUMENT;
   }
   else
      return RA_MISSING_TARGET;
}

void int_writer(FILE *f, const raAction *act)
{
   if (confirm_action_target(act))
      fprintf(f, "%d", *(int*)act->target);
}

/** ra_string_agent function implementations */
raStatus string_reader(const raAction *act, const char *str, raTour *tour)
{
   if (!str)
      return RA_MISSING_VALUE;

   if (confirm_action_target(act))
   {
      *(const char**)act->target = str;
      return RA_SUCCESS;
   }
   else
      return RA_MISSING_TARGET;
}

void string_writer(FILE *f, const raAction *act)
{
   if (confirm_action_target(act))
   {
      const char *val = *(const char**)act->target;
      if (val)
         fprintf(f, "%s", val);
      else
         fprintf(f, "(null)");
   }
}

/** default --help agent */
raStatus show_help_reader(const raAction *act, const char *str, raTour *tour)
{
   ra_show_help(stdout, 2, RAU_DEFAULT);
   return RA_CANCEL;
}

/** show action values */
raStatus show_action_values_reader(const raAction *act, const char *value, raTour *tour)
{
   fputs("\nCurrent action values:\n", stdout);
   ra_show_scene_values(stdout);
   putc('\n', stdout);
   return RA_SUCCESS;
}

EXPORT const raAgent ra_flag_agent        = { 0, flag_reader, flag_writer };
EXPORT const raAgent ra_int_agent         = { 1, int_reader, int_writer };
EXPORT const raAgent ra_string_agent      = { 1, string_reader, string_writer };
EXPORT const raAgent ra_show_help_agent   = { 0, show_help_reader, NULL };
EXPORT const raAgent ra_show_values_agent = { 0, show_action_values_reader, NULL };
