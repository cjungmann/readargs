#include <stdio.h>

#include "readargs.h"

const char *filepath = NULL;
int        flag = 0;
int        number = 0;
const char *input_file = NULL;
int        repetitions = 1;

raOpt options[] = {
   {'h', "help",    NULL,    "This help output",     &ra_show_help_agent,  NULL },
   {'v', "version", NULL,    "Show current version.",NULL,                 NULL },
   {'f', "flag",    NULL,    "set the flag.",        &ra_flag_agent,       &flag },
   {'n', "number",  "NUMBER","Set my_number value",  &ra_int_agent,        &number },
   {'p', "filepath","FILEPATH","Set file path.",     &ra_string_agent,     &filepath },
   {'s', "show",    NULL,    "Show option values",   &ra_show_values_agent,NULL },
   { -1, "*input",  "FILE",  "input file",           &ra_string_agent,     &input_file },
   { -1, "*reps",   "NUMBER","repetitions",          &ra_int_agent,        &repetitions }
};

int main(int argc, const char **argv)
{
   ra_set_scene(argv, argc, options, OPTS_COUNT(options));

   raTour tour;
   ra_start_tour(&tour);

   const raOpt *option;
   const char *value;

   raStatus status;

   while(1)
   {
      switch((status = ra_advance_option(&tour, &option, &value)))
      {
         case RA_SUCCESS:
            status = ra_execute_option_read(option, value);
            break;
         case RA_END_ARGS:
         case RA_END_OPTIONS:
            goto arguments_end;
         default:
            ra_write_warning(stderr, status, &tour, option, value);
            break;
      }
   }

  arguments_end:

   return 0;
   
}


