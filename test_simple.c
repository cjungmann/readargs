#include <stdio.h>

#include "readargs.h"

raStatus show_option_values_reader(const raOpt *option, const char *value)
{
   ra_show_option_values(stderr);
   return RA_SUCCESS;
}

raAgent show_value_agent = { 0, show_option_values_reader, NULL };

const char *filepath = NULL;
int        flag = 0;
int        number = 0;

raOpt options[] = {
   {'h', "help",     "This help output",      &ra_show_help_agent, NULL},
   {'v', "version",  "Show current version.", NULL,                NULL},
   {'f', "flag",     "set the flag.",         &ra_flag_agent,      &flag },
   {'n', "number",   "Set my_number value",   &ra_int_agent,       &number },
   {'p', "filepath", "Set file path.",        &ra_string_agent,    &filepath },
   {'s', "show",     "Show option values",    &show_value_agent,   NULL }
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


