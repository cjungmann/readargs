#include "readargs.h"

#include <string.h>

EXPORT int ra_init_cache(raCache *cache,
                         const char **start_arg,
                         int arg_count,
                         const raOpt *start_opt,
                         int opt_count)
{
   cache->args = start_arg;
   cache->args_end = start_arg + arg_count;
   cache->options = start_opt;
   cache->options_end = start_opt + opt_count;

   cache->current_arg = start_arg;
   cache->current_option = start_opt;
   cache->last_position_option = NULL;
   cache->sub_arg_ptr = NULL;

   return 1;
}

EXPORT const char *ra_command_name(const raCache *cache)
{
   const char *str = strrchr(*cache->args,'/');
   if (str)
      return str+1;
   else
      return *cache->args;
}

EXPORT const char *ra_next_arg(raCache *cache)
{
   cache->sub_arg_ptr = NULL;
   if (cache->current_arg + 1 < cache->args_end)
   {
      ++cache->current_arg;
      return *cache->current_arg;
   }
   else
      return NULL;
}


#ifdef CACHE_MAIN

raOpt options[] = {
   { 'h', "help", "This help message", NULL, NULL },
   { 'v', "version", "Version number", NULL, NULL }
};

int main(int argc, const char **argv)
{
   raCache cache;
   ra_init_cache(&cache, argv, argc, options, OPTS_COUNT(options));
   g_command_name = ra_command_name(&cache);

   printf("Processing command \e[32;1m%s\e[m:\n", ra_command_name(&cache));
   printf("Number of options defined: %ld.\n", cache.options_end - cache.options);
   
   const char *arg;
   int count = 0;
   while ((arg = ra_next_arg(&cache)))
      printf("%2d: %s\n", ++count, arg);
}

#endif



/* Local Variables: */
/* compile-command: "b=cache; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
