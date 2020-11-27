// Build with "cc -o hello hello.c -lreadargs
#include <stdio.h>
#include <readargs.h>

// Argument variables
const char *name = "World";

// Action map
raAction actions[] = {
   { 'n', "name", "Set name",     &ra_string_agent, &name, "STRING" },
   { 'h', "help", "This display", &ra_show_help_agent   }
};

int main(int argc, const char **argv)
{
   // Library initialization
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));

   if (ra_process_arguments())
   {
      printf("Hello, %s.\n", name);
   }
   return 0;
}
