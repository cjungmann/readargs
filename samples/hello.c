// Build with "gcc -o hello hello.c -lreadargs
#include <stdio.h>
#include <readargs.h>

// *Note 1: State Variables definition.
const char *name = "World";

// *Note 2: Action Map definition.
raAction actions[] = {
   { 'n', "name", "Set name",     &ra_string_agent, &name, "STRING" },
   { 'h', "help", "This display", &ra_show_help_agent   },
};

int main(int argc, const char **argv)
{
   // *Note 3: Initialize the library.
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));

   // *Note 4: Call library to process arguments.
   if (ra_process_arguments())
   {
      if (name)
         printf("Hello, %s.\n", name);
      else
         printf("Hello, world.\n");
   }
   return 0;
}
