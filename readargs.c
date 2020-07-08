#include "readargs.h"

#include <string.h>

extern raScene g_scene;

/** Optional, easy-to-use function for executing the read option of an raOpt. */
EXPORT raStatus ra_execute_option_read(const raOpt *option, const char *str)
{
   if (option && option->agent)
   {
      if (option->agent->reader)
         return (*option->agent->reader)(option, str);
      else
         return RA_MISSING_READER;
   }
   else
      return RA_MISSING_AGENT;
}

/** Optional, easy-to-use function for executing the write option of an raOpt. */
EXPORT void ra_execute_option_write(FILE *f, const raOpt *option)
{
   if (option && option->agent && option->agent->writer)
      (*option->agent->writer)(f, option);
}

typedef enum _raOpt_filter
{
   ROF_ALL = 0,
   ROF_OPTIONS,
   ROF_ARGS
} OptFilter;

int include_in_option_list(OptFilter set, const raOpt *ptr)
{
   return set == ROF_ALL
      || (set==ROF_OPTIONS && ra_is_named_option(ptr))
      || (set==ROF_ARGS && ra_is_positional_option(ptr));
}

/**
 * used by ra_show_options() to align text in columns.
 */
int get_max_label_length(OptFilter set)
{
   int len_max = 0;
   int len_label;
   const raOpt *ptr = g_scene.options;
   const char *str;
   while ( ptr < g_scene.options_end )
   {
      if (include_in_option_list(set, ptr))
      {
         if ((str = ptr->label))
         {
            // Exclude argument prefix flag if found:
            if (*str == '*')
               ++str;

            len_label = strlen(str);
            if (len_label > len_max)
               len_max = len_label;
         }
      }
      ++ptr;
   }

   return len_max;
}

void print_option_names(FILE *f, const raOpt *opt, int max_label)
{
   const char *str;
   if (opt->letter > 0)
   {
      if (opt->label)
         fprintf(f, "-%c, --%-*s  ", opt->letter, max_label, opt->label);
      else
         fprintf(f, "-%c    %-*s  ", opt->letter, max_label, "");
   }
   else if ((str = opt->label))
   {
      if (*str == '*')
         ++str;

      fprintf(f, "    --%-*s  ", max_label, opt->label);
   }
}

void print_option_help(FILE *f, const raOpt *opt, int max_label, int indent)
{
   if (indent>0)
      fprintf(f, "%*s", indent, "");

   print_option_names(f, opt, max_label);
   fputs( opt->description, f);
   fputc('\n', f);
}

int option_value_is_showable(const raOpt* option)
{
   return option && option->agent && option->agent->writer;
}

EXPORT void ra_show_scene_values(FILE *f)
{
  int len_label = get_max_label_length(ROF_ALL);
  const raOpt *ptr = g_scene.options;
  while ( ptr < g_scene.options_end )
  {
     if (option_value_is_showable(ptr))
     {
        print_option_names(f, ptr, len_label);
        ra_execute_option_write(f, ptr);
        fprintf(f, "\n");
     }
        
     ++ptr;
  }
}

/**
 * Counts flag options, and prints a flags usage string if any found.
 * For ra_describe_usage() with RAU_LONG usage.
 */
void print_usage_flag_options(FILE *f)
{
   int count = 0;
   const raOpt *ptr;

   ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (ra_is_flag_option(ptr) && ptr->letter > 0)
         ++count;

      ++ptr;
   }

   if (count)
   {
      fprintf(f, " [-");
      ptr = g_scene.options;
      while (ptr < g_scene.options_end)
      {
         if (ra_is_flag_option(ptr) && ptr->letter > 0)
            fputc(ptr->letter, f);

         ++ptr;
      }
      fputc(']', f);
   }
}

/**
 * Prints any value options found for ra_describe_usage()
 * with RAU_LONG usage.
 */
void print_usage_value_options(FILE *f)
{
   const raOpt *ptr;

   ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (ra_is_value_option(ptr))
      {
         fprintf(f, " [");
         if (ptr->letter)
            fprintf(f, "-%c ", ptr->letter);
         else if (ptr->label)
            fprintf(f, "--%s=", ptr->label);

         if (ptr->type)
            fprintf(f, "%s]", ptr->type);
         else
            fprintf(f, "%s]", "VALUE");
      }

      ++ptr;
   }
}

void print_usage_arguments(FILE *f)
{
   const raOpt *ptr;

   ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (ra_is_positional_option(ptr))
      {
         if (ptr->type)
            fprintf(f, " %s", ptr->type);
      }
      ++ptr;
   }
}

EXPORT void ra_describe_usage(FILE *f, int indent, raUsage usage)
{
   int options_count = ra_scene_options_count();

   if (usage == RAU_DEFAULT)
   {
      if (options_count > 8)
         usage = RAU_SHORT;
      else
         usage = RAU_LONG;
   }

   fprintf(f, "%*s%s", indent, "", ra_command_name());

   if (usage == RAU_SHORT)
      fprintf(f, " [OPTIONS]");
   else
   {
      print_usage_flag_options(f);
      print_usage_value_options(f);
      print_usage_arguments(f);
   }

   fputc('\n', f);
}

EXPORT void ra_describe_options(FILE *f, int indent)
{
   // Get max width of names, then
   // pass the information to an
   // option printing function.
   int len_label = get_max_label_length(ROF_OPTIONS);

   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (ra_is_named_option(ptr))
      {
         print_option_help(f, ptr, len_label, indent);
      }
      ++ptr;
   }
}

EXPORT void ra_describe_arguments(FILE *f, int indent)
{
   // Get max width of names, then
   // pass the information to an
   // option printing function.
   int len_label = get_max_label_length(ROF_ARGS);

   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (ra_is_positional_option(ptr))
      {
         print_option_help(f, ptr, len_label, indent);
      }
      ++ptr;
   }
}

EXPORT void ra_show_help(FILE *f, int indent, raUsage usage)
{
   int count_options = ra_scene_options_count();
   int count_args = ra_scene_arguments_count();

   fputs("Usage:\n", f);
   ra_describe_usage(f, indent, usage);

   if (count_options > 0)
   {
      fputs("\nOptions\n", f);
      ra_describe_options(f, indent);
   }

   if (count_args > 0)
   {
      fputs("\nArguments\n", f);
      ra_describe_arguments(f, indent);
   }

   
}


/* Local Variables: */
/* compile-command: "b=readargs; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
