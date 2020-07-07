#include <stdio.h>

#include "readargs.h"

int my_number = 0;

raOpt options[] = {
   {'h', "help", "This help output", &ra_show_help_agent, NULL},
   {'v', "version", "Show current version.", NULL, NULL},
   {'n', "number", "Set my_number value", &ra_int_agent, &my_number}
};

void show_variables(void)
{
   printf("my_number: %d.\n", my_number);
}

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

   show_variables();
   
   return 0;
   
}


