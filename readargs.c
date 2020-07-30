#include "readargs.h"
#include "invisible.h"

#include <string.h>
#include <alloca.h>

EXPORT raScene g_scene;

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

int include_in_option_list(OptFilter set, const raOpt *ptr)
{
   return set & ROF_ALL
      || (set & ROF_TYPES && ra_is_writable_option(ptr))
      || (set & ROF_OPTIONS && ra_is_named_option(ptr))
      || (set & ROF_ARGS && ra_is_positional_option(ptr));
}

/** Returns length needed to hold option identifier.  Sets string if buffer included.
 */
int option_labeler(const raOpt *ptr, OptFilter set, char *buffer, int len_buffer)
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
   else if (set & ROF_OPTIONS)
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

   if (set & ROF_VALUES && ra_is_value_option(ptr))
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

/** Calls option_labeler() to get label length.
 * Use in loop to get longest label length for columnar output.
 */
int get_label_length(const raOpt *ptr, OptFilter set)
{
   return option_labeler(ptr, set, NULL, 0);
}

/** Calls option_labeler() with string buffer into which the label will be copied.
 */
int set_label_value(const raOpt *ptr, OptFilter set, char *buffer, int buffer_len)
{
   return option_labeler(ptr, set, buffer, buffer_len);
}

/** Get width of longest label of invocable options.
 *
 * Called by ra_describe_options() for columnar alignment.
 * This does not consider the short option (dash-letter),
 * whose length is added to the column width according to 
 * align the help columns.
 */
int get_max_label_length(OptFilter set)
{
   int len_max = 0;
   int len_label;

   const raOpt *ptr = g_scene.options;
   while ( ptr < g_scene.options_end )
   {
      if (include_in_option_list(set, ptr))
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
   const raOpt *ptr = g_scene.options;
   const char *str;
   while ( ptr < g_scene.options_end )
   {
      if (ra_is_positional_option(ptr))
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

/** Utility function to display option statuses.
 *
 * For each option with a writer member, this function
 * writes out the option identifier and the output of the
 * option writer function.
 *
 * This can help a user check the options-accessible values
 * to confirm appropriate values.
 */
EXPORT void ra_show_scene_values(FILE *f)
{
  int len_max = get_max_label_length(ROF_OPTIONS);
  int len_buffer = len_max + 1;
  char *buffer = (char*)alloca(len_buffer);

  const raOpt *ptr = g_scene.options;
  while ( ptr < g_scene.options_end )
  {
     if (ra_is_writable_option(ptr))
     {
        set_label_value(ptr, ROF_OPTIONS, buffer, len_buffer);

        fprintf(f, "%*s:  ", len_max, buffer);
        ra_execute_option_write(f, ptr);
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
EXPORT int ra_process_arguments(void)
{
   raTour tour;
   ra_start_tour(&tour);

   raStatus status;
   const raOpt *option;
   const char *value;
   int retval = 0;

   while(1)
   {
      switch((status = ra_advance_option(&tour, &option, &value)))
      {
         case RA_SUCCESS:
            status = ra_execute_option_read(option, value);
            if (status==RA_CANCEL)
               goto arguments_end;
            break;

         case RA_END_ARGS:
         case RA_END_OPTIONS:
            retval = 1;
         case RA_CANCEL:
            goto arguments_end;

         default:
            ra_write_warning(stderr, status, &tour, option, value);
            break;
      }
   }

  arguments_end:

   return retval;
}


/* Local Variables: */
/* compile-command: "b=readargs; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
