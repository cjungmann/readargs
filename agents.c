#include "readargs.h"

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

EXPORT void ra_warn_invalid_input(FILE *f, const raOpt *opt, const char *msg)
{
   if (opt->letter > 0 || opt->label)
   {
      fprintf(f, "%s: option ", g_command_name);
      if (opt->letter > 0)
      {
         fprintf(f, "-%c", opt->letter);
         if (opt->label)
            fprintf(f, "/--%s", opt->label);
      }
      else if (opt->label)
         fprintf(f, "--%s", opt->label);

      fputc(' ', f);
      fputs(msg, f);
      fputc('\n', f);
   }
}

void warn_missing_target(const char *target_name)
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
      warn_missing_target(target_name);
      return 0;
   }
}


/*** construct ra_int_agent  */
void int_reader(const raOpt *opt, const char *str)
{
   if (confirm_option_target(opt, "int agent"))
   {
      if (str_is_number(str))
         *(int*)opt->target = atoi(str);
      else
         ra_warn_invalid_input(stderr, opt, "requires a number");
   }
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

EXPORT const raAgent ra_string_agent = { 1, string_reader, string_writer };

/** default --help agent */

void show_help_reader(const raOpt *opt, const char *str)
{
   ra_show_options(stdout, g_cache);
}

EXPORT const raAgent ra_show_help_agent = { 0, show_help_reader, NULL };
