#include <string.h>
#include <alloca.h>
#include "readargs.h"

char response_buffer[1024];
char *response_buffer_last_char = response_buffer + sizeof(response_buffer) - 1;

EXPORT int ra_collect_user_response(void)
{
   fgets(response_buffer, sizeof(response_buffer), stdin);

   if (*response_buffer_last_char)
   {
      if (*response_buffer_last_char == '\n')
         *response_buffer_last_char = '\0';
      else
      {
         printf("buffer overflow.\n");
         return 0;
      }
   }

   // Truncate string at the newline
   int len_str = strlen(response_buffer);
   if (response_buffer[len_str-1] == '\n')
      response_buffer[--len_str] = '\0';

   // return length of string + 1 for \0
   return len_str + 1;
}

EXPORT int ra_copy_user_response(char *buffer, int buffer_len)
{
   strncpy(buffer, response_buffer, buffer_len);
   return buffer_len;
}

EXPORT void ra_print_interactive_prompt(const raAction *action)
{
   const char *dasher = NULL;
   const char *name = NULL;
   char letter_buffer[2] = {0,0};

   if (action->label)
   {
      if (*action->label == '*')
      {
         dasher = "";
         name = &action->label[1];
      }
      else
      {
         dasher = "--";
         name = action->label;
      }
   }
   else if (action->letter > 0)
   {
      letter_buffer[0] = action->letter;
      dasher = "-";
      name = letter_buffer;
   }

   printf("[32;1mPlease enter required value %s%s:[m ", dasher, name);
}

/** Create temporary tour and process response to set raAction target.  */
EXPORT raStatus ra_process_action_with_response(const raAction *action, char *response)
{
   int count_args = ra_count_str2args(response);
   const char **args = (const char **)alloca(count_args * sizeof(char*));
   ra_str2args(response, args, count_args);

   const raAction *ppact = action;

   raTour tour = { args, *args+count_args, ppact, ppact+1 };
   const char *value = ra_current_arg(&tour);

   return ra_execute_action_read(action, value, &tour);
}

