#include "readargs.h"
#include "invisible.h"

#include <string.h>
#include <alloca.h>

EXPORT raScene g_scene;

/** Optional, easy-to-use function for executing the read action of an raAction. */
EXPORT raStatus ra_execute_action_read(const raAction *action, const char *str, raTour *tour)
{
   if (action && action->agent)
   {
      if (action->agent->reader)
         return (*action->agent->reader)(action, str, tour);
      else
         return RA_MISSING_READER;
   }
   else
      return RA_MISSING_AGENT;
}

/** Optional, easy-to-use function for executing the write action of an raAction. */
EXPORT void ra_execute_action_write(FILE *f, const raAction *action)
{
   if (action && action->agent && action->agent->writer)
      (*action->agent->writer)(f, action);
}

int include_in_action_list(ActFilter set, const raAction *ptr)
{
   return set & ROF_ALL
      || (set & ROF_TYPES && ra_is_writable_action(ptr))
      || (set & ROF_ACTIONS && ra_is_named_action(ptr))
      || (set & ROF_ARGS && ra_is_positional_action(ptr));
}

/** Returns length needed to hold action identifier.  Sets string if buffer included.
 */
int action_labeler(const raAction *ptr, ActFilter set, char *buffer, int len_buffer)
{
   int len_label;
   int len_value;

   const char *str = NULL;
   len_label = len_value = 0;

   if (set & ROF_TYPES)
   {
      if (!(str = ptr->type))
         str = ptr->label;
   }
   else if (set & ROF_ACTIONS)
      str = ptr->label;
   else if (set & ROF_ARGS)
   {
      if (!(str = ptr->type))
         str = ptr->label;
   }

   if (str)
   {
      if (*str == '*')
         ++str;

      len_label = strlen(str);
      if (len_label < len_buffer)
      {
         memcpy(buffer, str, len_label);
         buffer += len_label;
         len_buffer -= len_label;
         *buffer = '\0';
      }
   }

   if (set & ROF_VALUES && ra_is_value_action(ptr))
   {
      if (!(str = ptr->type))
         str = "VALUE";

      len_value = 2 + strlen(str);  // space for '=' and '\0'

      if (len_value < len_buffer)
      {
         *buffer++ = '=';
         memcpy(buffer, str, len_value);
         buffer[len_value] = '\0';
      }
   }

   return len_label + len_value;
}

/** Calls action_labeler() to get label length.
 * Use in loop to get longest label length for columnar output.
 */
int get_label_length(const raAction *ptr, ActFilter set)
{
   return action_labeler(ptr, set, NULL, 0);
}

/** Calls action_labeler() with string buffer into which the label will be copied.
 */
int set_label_value(const raAction *ptr, ActFilter set, char *buffer, int buffer_len)
{
   return action_labeler(ptr, set, buffer, buffer_len);
}

/** Get width of longest label of invocable actions.
 *
 * Called by ra_describe_actions() for columnar alignment.
 * This does not consider the short action (dash-letter),
 * whose length is added to the column width according to 
 * align the help columns.
 */
int get_max_label_length(ActFilter set)
{
   int len_max = 0;
   int len_label;

   const raAction *ptr = g_scene.actions;
   while ( ptr < g_scene.actions_end )
   {
      if (include_in_action_list(set, ptr))
      {
         len_label = get_label_length(ptr, set);

         if (len_label > len_max)
            len_max = len_label;
      }
      ++ptr;
   }

   return len_max;
}

/** Get width of longest label of the positional arguments
 * Called by ra_describe_arguments() for columnar alignment.
 */
int get_max_argument_length(void)
{
   int len_max = 0;
   int len_label;
   const raAction *ptr = g_scene.actions;
   const char *str;
   while ( ptr < g_scene.actions_end )
   {
      if (ra_is_positional_action(ptr))
      {
         if (ptr->type)
            str = ptr->type;
         else if (ptr->label)
         {
            str = ptr->label;
            if (*str == '*')
               ++str;
         }
         else
            str = "";

         len_label = strlen(str);

         if (len_label > len_max)
            len_max = len_label;
      }

      ++ptr;
   }

   return len_max;
}

/** Utility function to display action statuses.
 *
 * For each action with a writer member, this function
 * writes out the action identifier and the output of the
 * action writer function.
 *
 * This can help a user check the actions-accessible values
 * to confirm appropriate values.
 */
EXPORT void ra_show_scene_values(FILE *f)
{
  int len_max = get_max_label_length(ROF_ACTIONS);
  int len_buffer = len_max + 1;
  char *buffer = (char*)alloca(len_buffer);

  const raAction *ptr = g_scene.actions;
  while ( ptr < g_scene.actions_end )
  {
     if (ra_is_writable_action(ptr))
     {
        set_label_value(ptr, ROF_ACTIONS, buffer, len_buffer);

        fprintf(f, "%*s:  ", len_max, buffer);
        ra_execute_action_write(f, ptr);
        fprintf(f, "\n");
     }
        
     ++ptr;
  }
}

EXPORT void ra_show_no_args_message(void)
{
   fprintf(stderr, "Usage: ");
   ra_describe_usage(stderr, 0, RAU_DEFAULT);
   fprintf(stderr, "Try '%s --help' for more information.\n", ra_command_name());
}

/** Loop through arguments with builtin processing
 *
 * Errors are reported through stderr
 */
EXPORT int ra_process_tour_arguments(raTour *tour,
                                     int alert_no_args,
                                     int alert_unknown_option)
{
   const raAction *action;
   const char     *value;
   raStatus       status;

   if (alert_no_args && ra_scene_arguments_count() < 2)
   {
      FILE *pout = stderr;
      fputs("Usage: ", pout);
      ra_describe_usage(pout, 0, RAU_DEFAULT);
      ra_suggest_help(pout, 0);
      return 0;
   }

   while(1)
   {
      status = ra_advance_action(tour, &action, &value);

      if (status == RA_SUCCESS)
         status = ra_execute_action_read(action, value, tour);

      if (status == RA_CANCEL)              // cancel execution at action's direction
         return 0;                          // direct calling function to terminate program
      else if (status == RA_END_ARGS        // no more arguments to read
               || status == RA_END_OPTIONS) // encountered '--' argument
         return 1;                          // indicate successful tour of arguments
      else if (status != RA_SUCCESS)
      {
         ra_write_warning(stderr, status, tour, action, value);
         if (status >= RA_FATAL_ERROR)
            return 0;
         else
            return 1;
      }
   }
}

/** Make raTour instance from g_scene and send out for processing. */
EXPORT int ra_process_arguments(void)
{
   raTour tour;
   ra_start_tour(&tour);
   return ra_process_tour_arguments(&tour, 1, 1);
}

/* Local Variables: */
/* compile-command: "b=readargs; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
