#include "readargs.h"
#include "invisible.h"

#include <string.h>
#include <alloca.h>

/** Prints available option IDs, either or both the letter and label
 * Called by describe_single_option().
 */
void print_option_names(FILE *f, const raOpt *opt, int max_label, OptFilter set)
{
   int len_label = get_label_length(opt, set);
   char *str = (char*)alloca(len_label+1);
   set_label_value(opt, set, str, len_label+1);

   if (opt->letter > 0)
   {
      if (str)
         fprintf(f, "-%c, --%-*s  ", opt->letter, max_label, str);
      else
         fprintf(f, "-%c    %-*s  ", opt->letter, max_label, "");
   }
   else if (str)
      fprintf(f, "    --%-*s  ", max_label, str);
}

/** Write out list of flag options in form "[-abcde]"
 * Used by ra_describe_usage()
 *
 * Only creates output if there is at least flag option.
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

/** Prints any value options found for ra_describe_usage()
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



/** Per-line function used by ra_describe_options() */
void describe_single_option(FILE *f, const raOpt *opt, int max_label, int indent)
{
   if (indent>0)
      fprintf(f, "%*s", indent, "");

   print_option_names(f, opt, max_label, ROF_OPTIONS | ROF_VALUES);
   fputs( opt->comment, f);
   fputc('\n', f);
}

/** Per-line function used by ra_describe_arguments() */
void describe_single_argument(FILE *f, const raOpt *opt, int max_label, int indent)
{
   const char *left = NULL;

   if (indent>0)
      fprintf(f, "%*s", indent, "");

   if (opt->type)
      left = opt->type;
   else if (opt->label)
      left = opt->label;

   fprintf(f, "%-*s  %s\n", max_label, left, opt->comment);
}

/** Write out list of recognized arguments and their comments.
 *  Called by ra_show_help()
 */
EXPORT void ra_describe_arguments(FILE *f, int indent)
{
   // Get max width of names, then
   // pass the information to an
   // option printing function.
   int len_label = get_max_argument_length();

   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (ra_is_positional_option(ptr))
         describe_single_argument(f, ptr, len_label, indent);

      ++ptr;
   }
}

/** Write out list of available command line options with comments.
 *  Called by ra_show_help()
 */
EXPORT void ra_describe_options(FILE *f, int indent)
{
   // Get max width of names, then
   // pass the information to an
   // option printing function.
   int len_label = get_max_label_length(ROF_OPTIONS | ROF_VALUES);

   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (ra_is_named_option(ptr))
         describe_single_option(f, ptr, len_label, indent);

      ++ptr;
   }
}

/** Write out a usage example.  Called by ra_show_help() */
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

/** Write out minimal help screen from raOpts array
 *  This canned help screen may be sufficient for most uses.
 */
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

