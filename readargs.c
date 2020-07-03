#include "readargs.h"

#include <string.h>

EXPORT const char *g_command_name = NULL;
EXPORT const raCache *g_cache = NULL;

int is_intra_argument(raCache *cache)
{
   return cache->sub_arg_ptr && cache->sub_arg_ptr > *cache->current_arg;
}

int arguments_required(const raOpt *opt)
{
   return opt && opt->agent && opt->agent->args_needed;
}

int is_named_option(const raOpt *opt) { return opt->letter > 0 || opt->label != NULL; }


const raOpt *find_raOpt_by_label(const raCache *cache, const char *str)
{
   const raOpt *ptr = cache->options;
   while (ptr < cache->options_end)
   {
      if (strcmp(ptr->label, str) == 0)
         return ptr;

      ++ptr;
   }

   return NULL;
}

const raOpt *find_raOpt_by_letter(const raCache *cache, char letter)
{
   const raOpt *ptr = cache->options;
   while (ptr < cache->options_end)
   {
      if (ptr->letter == letter)
         return ptr;

      ++ptr;
   }

   return NULL;
}

EXPORT const char *ra_cur_arg(const raCache *cache)    { return *cache->current_arg; }
EXPORT const raOpt *ra_cur_option(const raCache *cache) { return cache->current_option; }

EXPORT int ra_next_option(raCache *cache, const raOpt **option, const char **value)
{
   const char **options_str = &cache->sub_arg_ptr;
   const raOpt *opt = NULL;
   /* const char *val = NULL; */

   if (! *options_str || ! **options_str)
   {
      const char *arg = ra_next_arg(cache);

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
                  opt = find_raOpt_by_label(cache, arg);
                  if (opt)
                  {
                     *option = opt;
                     return RA_SUCCESS;
                  }
               }
               else       // options terminator, naked double-dash "--"
               {
                  ;
               }
            }
            else          // short option, single-dash + letter
               *options_str = arg;
         }
      }
      else
         return RA_END_OPTIONS;
   }                      // end of ! *options_str
      
   if (*options_str)
   {
      char letter = **options_str;
      ++*options_str;
      opt = find_raOpt_by_letter(cache, letter);
      if (opt)
      {
         *option = opt;
         if (arguments_required(opt))
         {
            if (**options_str)
            {
               *value = *options_str;

               // clear so next iteration will be new argument
               *options_str = NULL;
            }
            else
               *value = ra_next_arg(cache);
         }
         return RA_SUCCESS;
      }
      else
         return RA_UNKNOWN_OPTION;
   }

   return 0;
}

/**
 * used by ra_show_options() to align text in columns.
 */
int get_max_label_length(const raCache *cache)
{
   int len_max = 0;
   int len_label;
   const raOpt *ptr = cache->options;
   while ( ptr < cache->options_end )
   {
      if (ptr->label)
      {
         len_label = strlen(ptr->label);
         if (len_label > len_max)
            len_max = len_label;
      }
      ++ptr;
   }

   return len_max;
}

EXPORT void ra_execute_option_read(const raOpt *option, const char *str)
{
   const raAgent *agent = ((option && option->agent) ? option->agent : NULL);
   if (agent && agent->reader)
      (*agent->reader)(option, str);
}

void print_option_help(FILE *f, const raOpt *opt, int max_label)
{
   if (opt->letter > 0)
   {
      if (opt->label)
         fprintf(f, "-%c, --%-*s  ", opt->letter, max_label, opt->label);
      else
         fprintf(f, "-%c    %-*s  ", opt->letter, max_label, "");
   }
   else if (opt->label)
      fprintf(f, "    --%-*s  ", max_label, opt->label);

   fputs( opt->description, f);
   fputc('\n', f);
}

EXPORT void ra_show_options(FILE *f, const raCache *cache)
{
   // Get max width of names, then
   // pass the information to an
   // option printing function.
   int len_label = get_max_label_length(cache);


   const raOpt *ptr = cache->options;
   while ( ptr < cache->options_end )
   {
      if (is_named_option(ptr))
      {
         print_option_help(f, ptr, len_label);
      }
      ++ptr;
   }
}



#ifdef READARGS_MAIN

#include "agents.c"
#include "cache.c"

const char *path = NULL;
int        count = 0;

raOpt options[] = {
   { 'h', "help", "This help output", &ra_show_help_agent, NULL  },
   { 'v', "version", "Show version info.", NULL, NULL },
   { 'n', "number", "Set a number", &ra_int_agent, &count },
   { 'p', "path", "Set a path", &ra_string_agent, &path }
};

int main(int argc, const char **argv)
{
   raCache cache;
   ra_init_cache(&cache, argv, argc, options, OPTS_COUNT(options));

   g_command_name = ra_command_name(&cache);
   g_cache = &cache;

   const raOpt *option;
   const char *value;

   raStatus status;

   while((status = ra_next_option(&cache, &option, &value)) != RA_END_OPTIONS)
   {
      if (status == RA_SUCCESS)
         ra_execute_option_read(option, value);
      else
         printf("Unknown option.\n");
   }

   printf("\n\nResult of command line arguments:\n");
   printf("path = %s\n", path);
   printf("count = %d\n", count);

   return 0;
}
#endif


/* Local Variables: */
/* compile-command: "b=readargs; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
