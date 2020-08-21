#include <stdio.h>
#include <string.h>
#include <alloca.h>

#include "readargs.h"

// Variables to be set with readacts library:
const char *password = NULL;
int        flag = 0;
int        number = 0;
const char *input_file = NULL;
int        repetitions = 1;

raAction actions[] = {
   {'h', "help",    "This help output",     &ra_show_help_agent,  NULL,        NULL       },
   {'v', "version", "Show current version.",NULL,                 NULL,        NULL       },
   {'s', "show",    "Show action values",   &ra_show_values_agent,NULL,        NULL       },
   {'f', "flag",    "set the flag.",        &ra_flag_agent,       &flag,       NULL       },
   {'n', "number",  "Set my_number value",  &ra_int_agent,        &number,     "NUMBER"   },
   {'p', "password","Account password.",    &ra_string_agent,     &password,   "PASSWORD" },
   { -1, "*input",  "input file",           &ra_string_agent,     &input_file, "FILE"     },
   { -1, "*reps",   "repetitions",          &ra_int_agent,        &repetitions,"NUMBER"   }
};

char response_buffer[1024];
char *response_buffer_last_char = response_buffer + sizeof(response_buffer) - 1;

int get_user_response(const char *prompt)
{
   int len_str = 0;
   
   memset(response_buffer, 0, sizeof(response_buffer));
   printf("[32;1m\n%s[m ", prompt?prompt:"Enter your response");
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
   len_str = strlen(response_buffer);
   if (response_buffer[len_str-1] == '\n')
      response_buffer[--len_str] = '\0';

   // return length of string + 1 for \0
   return len_str + 1;
}

int copy_user_response(char *buffer, int buffer_len)
{
   strncpy(buffer, response_buffer, buffer_len);
   return buffer_len;
}


int main(int argc, const char **argv)
{
   // Prepare the static library environment.
   // The *scene* must be set before we can use
   // any of the library output, so we don't check
   // the argc value until the scene is set.
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));

   if (argc > 1)
   {
      // Use the simplest processing
      if (ra_process_arguments())
      {
         if (!password)
         {
            int resplen;
            char *buffer;
            const raAction *ract = ra_seek_raAction_by_letter('p');
            if (ract)
            {
               resplen = get_user_response("Please enter your password");
               if (resplen)
               {
                  buffer = (char*)alloca(resplen);
                  copy_user_response(buffer, resplen);

                  printf("You entered [32;1m%s[m.\n", buffer);
               }
            }
         }

         ra_show_scene_values(stdout);
      }
   }
   else
      ra_show_no_args_message();

   return 0;
   
}


