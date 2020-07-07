#include "readargs.h"

#include <string.h>

extern raScene g_scene;

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

EXPORT void ra_execute_option_write(FILE *f, const raOpt *option)
{
   if (option && option->agent && option->agent->writer)
      (*option->agent->writer)(f, option);
}


int is_named_option(const raOpt *opt) { return opt->letter > 0 || opt->label != NULL; }

/**
 * used by ra_show_options() to align text in columns.
 */
int get_max_label_length(void)
{
   int len_max = 0;
   int len_label;
   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (ptr->label)
      {
         len_label = strlen(ptr->label);
         if (len_label > len_max)
            len_max = len_label;
      }
      ++ptr;
   }

   return len_max;
}

void print_option_names(FILE *f, const raOpt *opt, int max_label)
{
   if (opt->letter > 0)
   {
      if (opt->label)
         fprintf(f, "-%c, --%-*s  ", opt->letter, max_label, opt->label);
      else
         fprintf(f, "-%c    %-*s  ", opt->letter, max_label, "");
   }
   else if (opt->label)
      fprintf(f, "    --%-*s  ", max_label, opt->label);
}

void print_option_help(FILE *f, const raOpt *opt, int max_label)
{
   print_option_names(f, opt, max_label);
   fputs( opt->description, f);
   fputc('\n', f);
}

EXPORT void ra_show_options(FILE *f)
{
   // Get max width of names, then
   // pass the information to an
   // option printing function.
   int len_label = get_max_label_length();

   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (is_named_option(ptr))
      {
         print_option_help(f, ptr, len_label);
      }
      ++ptr;
   }
}

int option_value_is_showable(const raOpt* option)
{
   return option && option->agent && option->agent->writer;
}

EXPORT void ra_show_option_values(FILE *f)
{
  int len_label = get_max_label_length();
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


/* Local Variables: */
/* compile-command: "b=readargs; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
