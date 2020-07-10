#include "readargs.h"
#include "invisible.h"

#include <string.h>
#include <alloca.h>

EXPORT int ra_is_positional_option(const raOpt* opt)
{
   return opt->letter<1
      && (opt->label && *opt->label == '*');
}

EXPORT int ra_is_named_option(const raOpt *opt)
{
   return opt->letter > 0
      || (opt->label && *opt->label != '*');
}

EXPORT int ra_is_flag_option(const raOpt *opt)
{
   return opt->agent && opt->agent->args_needed==0;
}

EXPORT int ra_is_value_option(const raOpt *opt)
{
   return opt
      && !(opt->label && *opt->label=='*')
      && opt->agent
      && opt->agent->args_needed;
}

EXPORT int ra_is_writable_option(const raOpt *opt)
{
   return opt && opt->agent && opt->agent->writer;
}

const raOpt *seek_raOpt_by_label(const char *str)
{
   char *lstr = strchr(str,'=');

   // We can't change terminate str at '=' because it's constant.
   // So we make a copy stopping before '=' and pretend it's str.
   if (lstr)
   {
      int len = lstr - str;
      lstr = (char*)alloca(len+1);
      memcpy(lstr, str, len);
      lstr[len] = '\0';
      str = lstr;
   }

   const raOpt *ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (strcmp(ptr->label, str) == 0)
         return ptr;

      ++ptr;
   }

   return NULL;
}

const raOpt *seek_raOpt_by_letter(char letter)
{
   const raOpt *ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (ptr->letter == letter)
         return ptr;

      ++ptr;
   }

   return NULL;
}

/** Marker to indicate completion of positional options.
 * This value is written to raTour::last_position_option
 * when there are no more positional argument.
 */
static const raOpt *position_options_done = (const raOpt*)(-1);

/** Iteratively returns positional arguments.
 * This function is called to determine which option
 * to apply to command line arguments that are not 
 * labeled options.
 */
const raOpt *seek_next_positional_option(raTour *tour)
{
   const raOpt *opt = tour->last_position_option;
   if (opt == position_options_done)
      return NULL;
   else if (opt == NULL)
      opt = g_scene.options - 1;

   while (++opt < g_scene.options_end)
      if (ra_is_positional_option(opt))
      {
         tour->last_position_option = opt;
         return opt;
      }

   tour->last_position_option = position_options_done;
   return NULL;
}

/**
 * Count raOpts that are NOT positional.
 *
 * Use this function while displaying help to determine
 * if an options section is required.
 */
EXPORT int ra_scene_options_count(void)
{
   int count = 0;
   const raOpt *ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (!ra_is_positional_option(ptr))
         ++count;
      
      ++ptr;
   }

   return count;
}

/**
 * Count raOpts that ARE positional.
 *
 * Use this function while displaying help to determine
 * if an arguments section is required.
 */
EXPORT int ra_scene_arguments_count(void)
{
   int count = 0;
   const raOpt *ptr = g_scene.options;
   while (ptr < g_scene.options_end)
   {
      if (ra_is_positional_option(ptr))
         ++count;
      
      ++ptr;
   }

   return count;
}

/**
 * Reads the first command line argument and returns
 * the portion that follows that last '/', or the entire
 * argument if no '/' characters are found.
 */
EXPORT const char *ra_command_name(void)
{
   const char *str = strrchr(*g_scene.args,'/');
   if (str)
      return str+1;
   else
      return *g_scene.args;
}

/**
 * Initialize the readopts environment.
 */
EXPORT void ra_set_scene(const char **start_arg,
                         int arg_count,
                         const raOpt *start_opt,
                         int opt_count)
{
   g_scene.args = start_arg;
   g_scene.args_end = start_arg + arg_count;
   g_scene.options = start_opt;
   g_scene.options_end = start_opt + opt_count;
}

/**
 * Initialize a new tour of options.
 * 
 * This initializes a new set of pointers
 * that tracks progress through argument/options
 * processing.
 */
EXPORT raTour *ra_start_tour(raTour *tour)
{
   tour->current_arg = g_scene.args;
   tour->current_option = g_scene.options;

   tour->sub_arg_ptr = NULL;
   tour->last_position_option = NULL;

   return tour;
}

/**
 * Returns the next command line argument,
 * updating the raTour pointers to keep track
 * of progress.
 */
EXPORT const char* ra_advance_arg(raTour *tour)
{
   tour->sub_arg_ptr = NULL;
   if (tour->current_arg < g_scene.args_end)
      return *++tour->current_arg;
   else
      return NULL;
}

/**
 * Get the next option, with a value if appropriate.
 * 
 * When the return value is RA_SUCCESS, the program
 * should process the option.
 *
 * Non-RA_SUCCESS values may indicate the end of the
 * list of arguments, an error, or the end of options.
 * Process these values according to the needs of your
 * program.
 */
EXPORT raStatus ra_advance_option(raTour *tour, const raOpt **option, const char **value)
{
   *option = NULL;
   *value = NULL;

   const char **options_str = &tour->sub_arg_ptr;
   const raOpt *opt = NULL;

   if (! *options_str || ! **options_str)
   {
      const char *arg = ra_advance_arg(tour);

      if (arg)
      {
         if (*arg == '-')
         {
            ++arg;
            if (*arg == '-')
            {
               ++arg;
               if (*arg)  // long option, double-dash + label
               {
                  opt = seek_raOpt_by_label(arg);
                  if (opt)
                  {
                     *option = opt;

                     if (ra_is_value_option(opt))
                     {
                        const char *tstr = strchr(arg,'=');
                        if (tstr)
                           *value = tstr+1;
                        else
                           *value = ra_advance_arg(tour);
                     }

                     return RA_SUCCESS;
                  }
               }
               else       // options terminator, naked double-dash "--"
                  return RA_END_OPTIONS;
            }
            else          // short option, single-dash + letter
               *options_str = arg;
         }
         else // not an option, read as next positional option
         {
            opt = seek_next_positional_option(tour);
            if (opt)
            {
               *option = opt;
               *value = arg;
               return RA_SUCCESS;
            }
         }
      }
   }
      
   if (*options_str)
   {
      char letter = **options_str;
      ++*options_str;
      opt = seek_raOpt_by_letter(letter);
      if (opt)
      {
         *option = opt;
         if (ra_is_value_option(opt))
         {
            if (**options_str)
            {
               *value = *options_str;

               // clear so next iteration will be new argument
               *options_str = NULL;
            }
            else
               *value = ra_advance_arg(tour);
         }
         return RA_SUCCESS;
      }
      else
         return RA_UNKNOWN_OPTION;
   }

   return RA_END_ARGS;
}

/**
 * Convenience function for writing out an error.
 *
 * This function is used by the default raTour processor,
 * but it is optional for custom implementations.
 */
EXPORT void ra_write_warning(FILE *f,
                             raStatus status,
                             const raTour *tour,
                             const raOpt *option,
                             const char *value)
{
   fprintf(f, "%s: ", ra_command_name());

   if (option)
   {
      if (option->letter)
         fprintf(f, "-%c: ", option->letter);
      else if (option->label)
         fprintf(f, "--%s: ", option->label);
   }

   switch(status)
   {
      case RA_SUCCESS:
         fprintf(f, "no error\n");
         break;
      case RA_END_OPTIONS:
         fprintf(f, "reached the end of the options\n");
         break;
      case RA_END_ARGS:
         fprintf(f, "reached the end of the arguments\n");
         break;
      case RA_UNKNOWN_OPTION:
         fprintf(f, "%s\n", *tour->current_arg);
         break;
      case RA_MALFORMED_OPTION:
         fprintf(f, "option value in bad form\n");
         break;
      case RA_INVALID_ARGUMENT:
         fprintf(f, "invalid argument\n");
         break;
      case RA_MISSING_TARGET:
         fprintf(f, "option missing target\n");
         break;
      case RA_MISSING_AGENT:
         fprintf(f, "option missing agent\n");
         break;
      case RA_MISSING_READER:
         fprintf(f, "option missing reader\n");
         break;
      default:
         fprintf(f, "unknown status number %d\n", (int)status);
         break;
   }
}

#ifdef SCENE_MAIN

#include "agents.c"

const char *path = NULL;
int        number = 0;
int        flag = 0;
const char *input_file = NULL;
int        repetitions = 0;

raOpt options[] = {
   { 'h', "help",    "This help message", NULL,             NULL },
   { 'v', "version", "Version number",    NULL,             NULL },
   { 'p', "path",    "Set file path",     &ra_string_agent, &path },
   { 'n', "number",  "Set number value",  &ra_int_agent,    &number },
   { 'f', "flag",    "Set flag to on",    &ra_flag_agent,   &flag },
   { -1,  "*input",  "input file",        &ra_string_agent, &input_file },
   { -1,  "*reps",   "repetitions",       &ra_int_agent,    &repetitions }
};

void display_option(const raOpt *option, const char *value, int wellformed)
{
   if (!wellformed)
      printf("Malformed option: ");

   if (option->letter)
      printf("-%c", option->letter);
   else if (option->label)
      printf("--%s", option->label);

   if (value)
      printf(":%s\n", value);
   else
      printf("\n");
}

int main(int argc, const char **argv)
{
   raTour tour;
   ra_set_scene(argv, argc, options, OPTS_COUNT(options));
   ra_start_tour(&tour);

   const char *arg;
   int count = 0;

   while ((arg = ra_advance_arg(&tour)))
      printf("%2d: %s\n", ++count, arg);

   const raOpt *option;
   const char *value;

   ra_start_tour(&tour);

   while (1)
   {
      switch(ra_advance_option(&tour, &option, &value))
      {
         case RA_SUCCESS:
            switch(ra_execute_option_read(option, value))
            {
               case RA_SUCCESS:
                  break;
               case RA_INVALID_ARGUMENT:
               case RA_MISSING_AGENT:
               case RA_MISSING_READER:
                  break;
               default:
                  break;
            }
            break;
         case RA_END_ARGS:
         case RA_END_OPTIONS:
            goto end_option_walk;
         case RA_UNKNOWN_OPTION:
            fprintf(stderr, "Option %s is not known.\n", *tour.current_arg);
            break;
         case RA_MALFORMED_OPTION:
            display_option(option, value, 0);
            break;
         case RA_MISSING_TARGET:
            fprintf(stderr, "%s: missing target for option.\n", ra_command_name());
            break;
         case RA_INVALID_ARGUMENT:
            fprintf(stderr, "%s: argument value is the wrong type.\n", ra_command_name());
            break;
         default:
            break;
      }
   }
end_option_walk:

   return 0;
}

#endif



/* Local Variables: */
/* compile-command: "b=scene; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
