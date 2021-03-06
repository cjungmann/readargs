// Build with "gcc -o customs customs.c -lreadargs
#include <stdio.h>
#include <readargs.h>

#include <stdlib.h>  // for atof()
#include <string.h>  // for strcasecmp()
#include <errno.h>

/****************************************************************
 * Custom agent: read a floating value
 * Name:         float_agent
 * Reference:    info readargs "Advanced Topics" "Floating Agent"
 */

raStatus float_reader(const raAction *act, const char *str, raTour *tour)
{
   if (!str)
      return RA_MISSING_VALUE;

   float val = 0.0;
   char *endptr = NULL;

   val = strtof(str, &endptr);

   if (val == 0.0 && str == endptr)
      return RA_INVALID_ARGUMENT;

   *(float*)act->target = val;
   return RA_SUCCESS;
}

void float_writer(FILE *f, const raAction *act)
{
   float *target = (float*)act->target;
   fprintf(f, "%f", *target);
}

const raAgent float_agent = { 1, float_reader, float_writer };

/****************************************************************
 * Custom agent: read two arguments
 * Name:         var_agent
 * Reference:    info readargs "Advanced Topics" "Multi-value Agent"
 */

// Target declaration
struct VAR_PAIR { const char *name; const char *value; };

raStatus var_reader(const raAction *act, const char *str, raTour *tour)
{
   struct VAR_PAIR *pair = (struct VAR_PAIR*)act->target;

   pair->name = str;
   // Safely collect the next argument value
   pair->value = ra_advance_arg(tour);

   if (pair->value)
      return RA_SUCCESS;
   else
      return RA_CANCEL;
}

void var_writer(FILE *f, const raAction *act)
{
   struct VAR_PAIR *pair = (struct VAR_PAIR*)act->target;
   if (pair->name && pair->value)
      fprintf(f, "%s = \"%s\"", pair->name, pair->value);
   else
      fputs("n/a", f);
}

const raAgent var_agent = { 2, var_reader, var_writer };


/** color_agent support code: */

struct ColMap { const char *name; const char *console; };
const struct ColMap colmap[] = {
   { "red", "\e[31;1m" },
   { "green", "\e[32;1m" },
   { "brown", "\e[33m" },
   { "yellow", "\e[33;1m" },
   { "blue", "\e[34;1m" },
   { "magenta", "\e[35;1m" },
   { "cyan", "\e[36;1m" }
};

const char *get_console_code(const char *str)
{
   const struct ColMap* cm = colmap;
   const struct ColMap* end = colmap + (sizeof(colmap) / sizeof(colmap[0]));
   while (cm < end)
   {
      if (strcasecmp(cm->name, str)==0)
         return cm->console;
      ++cm;
   }
   return NULL;
}

/****************************************************************
 * Custom agent: optional argument, MUST be last option if argument omitted
 * Name:         color_agent
 * Reference:    info readargs "Advanced Topics" "Optional-value Agent"
 */

raStatus color_reader(const raAction *act, const char *str, raTour *tour)
{
   const char **target = (const char **)act->target;

   if (!str || !*str)
      str = "red";

   if (get_console_code(str))
   {
      *target = str;
      return RA_SUCCESS;
   }
   else
      return RA_INVALID_ARGUMENT;
}

void color_writer(FILE *f, const raAction *act)
{
   const char *target = *(const char **)act->target;
   if (target)
      fprintf(f, "%s", target);
   else
      // handle unset value
      fputs("n/a", f);
}

const raAgent color_agent = { 1, color_reader, color_writer };



/****************************************************************
 * Custom agent: optional argument, other options can follow
 * Name:         greeting_agent
 * Reference:    info readargs "Advanced Topics" "Optional-value Agent"
 */

raStatus greeting_reader(const raAction *act, const char *str, raTour *tour)
{
   const char **target = (const char**)act->target;

   *target = "Hello";

   if (str && *str)
   {
      // Only use the value if it cannot be an option call
      if (!ra_seek_raAction(str,tour))
         *target = str;
      else
         ra_retreat_arg(tour);
   }

   return RA_SUCCESS;
}

void greeting_writer(FILE *f, const raAction *act)
{
   const char *target = *(const char**)act->target;
   if (target)
      fprintf(f, "%s", target);
   else
      fputs("n/a", f);
}

const raAgent greeting_agent = { 1, greeting_reader, greeting_writer };

/****************************************************************
 * Demonstration of processing a repeating option
 * Reference: info "Advanced Topics" "Repeating Option"
 */

void display_friends(void)
{
   raTour tour;
   ra_start_tour(&tour);

   const raAction *action;
   const char     *value;
   raStatus       status;

   // First count friends to see if we need to display anything
   int friend_count = 0;

   while (1)
   {
      status = ra_advance_action(&tour, &action, &value);
      if (status == RA_SUCCESS && action->letter == 'f')
         ++friend_count;

      if (status == RA_END_ARGS || status == RA_END_OPTIONS)
         break;
   }

   if (friend_count)
   {
      printf("\nFriends list:\n");
      ra_start_tour(&tour);
      while (1)
      {
         status = ra_advance_action(&tour, &action, &value);
         if (status == RA_SUCCESS && action->letter == 'f')
            printf("  - %s\n", value);

         if (status == RA_END_ARGS || status == RA_END_OPTIONS)
            break;
      }
   }
   else
      printf("\nNo friends\n");
}


const char *name = NULL;
const char *friend = NULL;
int show = 0;

float temperature = 0.0;
const char *colstart = NULL;
const char *greeting = NULL;
struct VAR_PAIR vpair = {};

raAction actions[] = {
   { 'h', "help", "Show this display", &ra_show_help_agent },
   { 'n', "name", "Set the name", &ra_string_agent, &name },
   { 's', "show", "Show values", &ra_flag_agent, &show },

   // Repeating option
   { 'f', "friend", "Add friend.  May use multiple times.", &ra_string_agent, &friend, "NAME" },

   // Using custom agents
   { 't', "temperature", "Set temperature (float_agent, float value)", &float_agent, &temperature, "TEMP" },
   { 'c', "color", "Set output color (color_agent, optional argument, naked option must be at end)", &color_agent, &colstart, "COLOR" },
   { 'g', "greet", "Greet user (greeting_agent, optional argument, naked option need not be at end)", &greeting_agent, &greeting, "GREETING" },
   { 'v', "var", "Set variable value (var_agent, two string arguments)", &var_agent, &vpair, "NAME VALUE" }
};



int main(int argc, const char **argv)
{
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));

   if (ra_process_arguments())
   {
      if (show)
      {
         printf("Showing scene values:\n");
         ra_show_scene_values(stdout);
         putchar('\n');
      }

      // Carry out -c option
      if (colstart)
         printf("%s", get_console_code(colstart));

      // Display value from -g option
      if (greeting)
         printf("%s\n", greeting);

      // Display value from -v option
      if (vpair.name)
         printf("%s = \"%s\"\n", vpair.name, vpair.value);

      // Display value from -n option
      if (name)
         printf("The name is \"%s\"\n", name);

      // Display value from -t option
      if (temperature != 0.0)
         printf("The temperature is %f.\n", temperature);

      // Display 0 or more -f options
      display_friends();

      printf("Done.\n");

      // Undo state left by -c option
      if (colstart)
         printf("\e[m");
   }

   return 0;
}
