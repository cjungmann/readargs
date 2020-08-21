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

/* char response_buffer[1024]; */
/* char *response_buffer_last_char = response_buffer + sizeof(response_buffer) - 1; */

/* int collect_user_response(void) */
/* { */
/*    fgets(response_buffer, sizeof(response_buffer), stdin); */

/*    if (*response_buffer_last_char) */
/*    { */
/*       if (*response_buffer_last_char == '\n') */
/*          *response_buffer_last_char = '\0'; */
/*       else */
/*       { */
/*          printf("buffer overflow.\n"); */
/*          return 0; */
/*       } */
/*    } */

/*    // Truncate string at the newline */
/*    int len_str = strlen(response_buffer); */
/*    if (response_buffer[len_str-1] == '\n') */
/*       response_buffer[--len_str] = '\0'; */

/*    // return length of string + 1 for \0 */
/*    return len_str + 1; */
/* } */

/* int copy_user_response(char *buffer, int buffer_len) */
/* { */
/*    strncpy(buffer, response_buffer, buffer_len); */
/*    return buffer_len; */
/* } */

/* int get_user_response(const char *prompt) */
/* { */
/*    memset(response_buffer, 0, sizeof(response_buffer)); */
/*    printf("[32;1m\n%s[m ", prompt?prompt:"Enter your response"); */
/*    return collect_user_response(); */
/* } */

/* void print_interactive_prompt(const raAction *action) */
/* { */
/*    const char *dasher = NULL; */
/*    const char *name = NULL; */
/*    char letter_buffer[2] = {0,0}; */

/*    if (action->label) */
/*    { */
/*       if (*action->label == '*') */
/*       { */
/*          dasher = ""; */
/*          name = &action->label[1]; */
/*       } */
/*       else */
/*       { */
/*          dasher = "--"; */
/*          name = action->label; */
/*       } */
/*    } */
/*    else if (action->letter > 0) */
/*    { */
/*       letter_buffer[0] = action->letter; */
/*       dasher = "-"; */
/*       name = letter_buffer; */
/*    } */

/*    printf("[32;1mPlease enter required value %s%s:[m ", dasher, name); */
/* } */

/* /\** Create temporary tour and process response to set raAction target.  *\/ */
/* raStatus ra_process_action_with_response(const raAction *action, char *response) */
/* { */
/*    int count_args = ra_count_str2args(response); */
/*    const char **args = (const char **)alloca(count_args * sizeof(char*)); */
/*    ra_str2args(response, args, count_args); */

/*    const raAction *ppact = action; */

/*    raTour tour = { args, *args+count_args, ppact, ppact+1 }; */
/*    const char *value = ra_current_arg(&tour); */

/*    return ra_execute_action_read(action, value, &tour); */
/* } */

/* typedef struct ra_xa_env { */
/*    int buff_len; */
/*    char *buff; */
/* } RA_XA_ENV; */

/* #define RA_READ_XA_RESPONSE(raenv) (raenv).buff_len = collect_user_response(); \ */
/*    (raenv).buff = (char*)alloca((raenv).buff_len); \ */
/*    copy_user_response((raenv).buff, (raenv).buff_len) */
/* #define RA_PROCESS_XA_ACTION(a, env) ra_process_action_with_response(a, (env).buff) */


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
         printf("[34;1mBefore getting extras.[m\n");
         ra_show_scene_values(stdout);

         printf("[34;1mGetting extras.[m\n");
         RA_XA_ENV env;
         if (!password)
         {
            const raAction *ract = ra_seek_raAction_by_letter('p');
            ra_print_interactive_prompt(ract);
            RA_READ_XA_RESPONSE(env);
            RA_PROCESS_XA_ACTION(ract, env);
         }

         printf("[34;1mJust got extras[m\n");
         ra_show_scene_values(stdout);
         printf("[34;1mFinished second show_scene[m\n");
      }
   }
   else
      ra_show_no_args_message();

   return 0;
   
}


