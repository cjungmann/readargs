#include "readargs.h"
#include "invisible.h"

#include <string.h>

/** Prints available action IDs, either or both the letter and label
 * Called by describe_single_action().
 */
void print_action_names(FILE *f, const raAction *act, int max_label, ActFilter set)
{
   int len_label = get_label_length(act, set);
   char str[len_label+1];
   set_label_value(act, set, str, len_label+1);

   if (act->letter > 0)
   {
      if (*str)
         fprintf(f, "-%c, --%-*s  ", act->letter, max_label, str);
      else
         fprintf(f, "-%c    %-*s  ", act->letter, max_label, "");
   }
   else if (*str)
      fprintf(f, "    --%-*s  ", max_label, str);
}

/** Write out list of flag actions in form "[-abcde]"
 * Used by ra_describe_usage()
 *
 * Only creates output if there is at least one flag action.
 */
void print_usage_flag_actions(FILE *f)
{
   int count = 0;
   const raAction *ptr;

   ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (ra_is_flag_action(ptr) && ptr->letter > 0)
         ++count;

      ++ptr;
   }

   if (count)
   {
      fprintf(f, " [-");
      ptr = g_scene.actions;
      while (ptr < g_scene.actions_end)
      {
         if (ra_is_flag_action(ptr) && ptr->letter > 0)
            fputc(ptr->letter, f);

         ++ptr;
      }
      fputc(']', f);
   }
}

/** Prints any value actions found for ra_describe_usage()
 * with RAU_LONG usage.
 * 
 * This is the part "[ -a age ]" or "[ --age=NUMBER ]" of a
 * usage display
 */
void print_usage_value_actions(FILE *f)
{
   const raAction *ptr;
   const char *val = NULL;

   ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (ra_is_value_action(ptr))
      {
         fprintf(f, " [");
         if (ptr->letter)
            fprintf(f, "-%c ", ptr->letter);
         else if (ptr->label)
            fprintf(f, "--%s=", ptr->label);

         if (ptr->letter && ptr->label)
            val = ptr->label;
         else if (ptr->type)
            val = ptr->type;
         else
            val = "VALUE";

         fprintf(f, "%s]", val);
      }

      ++ptr;
   }
}

/**
 * Print the positional arguments for the usage display
 */
void print_usage_arguments(FILE *f)
{
   const raAction *ptr;
   const char *val = NULL;

   ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (ra_is_positional_action(ptr))
      {
         if (ptr->label && *ptr->label == '*')
            val = ptr->label + 1;
         else if (ptr->type)
            val = ptr->type;

         if (val)
            fprintf(f, " %s", val);
      }
      ++ptr;
   }
}


/** Per-line function used by ra_describe_actions() */
void describe_single_action(FILE *f, const raAction *act, int max_label, int indent)
{
   if (indent>0)
      fprintf(f, "%*s", indent, "");

   print_action_names(f, act, max_label, ROF_ACTIONS | ROF_VALUES);
   fputs( act->comment, f);
   fputc('\n', f);
}

/** Per-line function used by ra_describe_arguments() */
void describe_single_argument(FILE *f, const raAction *act, int max_label, int indent)
{
   const char *left = NULL;

   if (indent>0)
      fprintf(f, "%*s", indent, "");

   if (act->type)
      left = act->type;
   else if (act->label)
      left = act->label;

   fprintf(f, "%-*s  %s\n", max_label, left, act->comment);
}

/** Write out list of recognized arguments and their comments.
 *  Called by ra_show_help()
 */
EXPORT void ra_describe_arguments(FILE *f, int indent)
{
   // Get max width of names, then
   // pass the information to an
   // action printing function.
   int len_label = get_max_argument_length();

   const raAction *ptr = g_scene.actions;
   while ( ptr < g_scene.actions_end )
   {
      if (ra_is_positional_action(ptr))
         describe_single_argument(f, ptr, len_label, indent);

      ++ptr;
   }
}

/** Write out list of available command line actions with comments.
 *  Called by ra_show_help()
 */
EXPORT void ra_describe_actions(FILE *f, int indent)
{
   // Get max width of names, then
   // pass the information to an
   // action printing function.
   int len_label = get_max_label_length(ROF_ACTIONS | ROF_VALUES);

   const raAction *ptr = g_scene.actions;
   while ( ptr < g_scene.actions_end )
   {
      if (ra_is_named_action(ptr))
         describe_single_action(f, ptr, len_label, indent);

      ++ptr;
   }
}

/** Write out a usage example.  Called by ra_show_help() */
EXPORT void ra_describe_usage(FILE *f, int indent, raUsage usage)
{
   int actions_count = ra_scene_actions_count();

   if (usage == RAU_DEFAULT)
   {
      if (actions_count > 8)
         usage = RAU_SHORT;
      else
         usage = RAU_LONG;
   }

   fprintf(f, "%*s%s", indent, "", ra_command_name());

   if (usage == RAU_SHORT)
      fprintf(f, " [ACTIONS]");
   else
   {
      print_usage_flag_actions(f);
      print_usage_value_actions(f);
      print_usage_arguments(f);
   }

   fputc('\n', f);
}

/** Write out minimal help screen from raActions array
 *  This canned help screen may be sufficient for most uses.
 */
EXPORT void ra_show_help(FILE *f, int indent, raUsage usage)
{
   int count_actions = ra_scene_actions_count();
   int count_args = ra_scene_argument_actions_count();

   fputs("Usage:\n", f);
   ra_describe_usage(f, indent, usage);

   if (count_actions > 0)
   {
      fputs("\nOptions\n", f);
      ra_describe_actions(f, indent);
   }

   if (count_args > 0)
   {
      fputs("\nArguments\n", f);
      ra_describe_arguments(f, indent);
   }
}

EXPORT void ra_suggest_help(FILE *f, int indent)
{
   const raAction *ptr = g_scene.actions;

   while (ptr < g_scene.actions_end)
   {
      if (ptr->agent == &ra_show_help_agent)
      {
         fprintf(f, "%*sTry '%s ", indent, "", ra_command_name());

         if (ptr->label)
            fprintf(f, "--%s", ptr->label);
         else if (ptr->letter)
            fprintf(f, "-%c", ptr->letter);

         fprintf(f, "' for more information.\n");
         break;
      }
      ++ptr;
   }
}
