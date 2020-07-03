#ifndef READARGS_H
#define READARGS_H

#define EXPORT __attribute__((visibility("default")))

#include <stdio.h>

typedef struct _readargs_option raOpt;

typedef void (*readargs_reader_t)(const raOpt *opt, const char *str);
typedef void (*readargs_writer_t)(FILE *f, const raOpt *opt);

typedef struct _readargs_agent
{
   int args_needed;
   readargs_reader_t reader;
   readargs_writer_t writer;
} raAgent;

typedef struct _readargs_option
{
   int           letter;
   const char    *label;
   const char    *description;
   const raAgent *agent;
   void          *target;
} raOpt;

typedef struct _readargs_cache
{
   // Collections of CL arguments and option definitions
   const char **args;
   const char **args_end;
   const raOpt *options;
   const raOpt *options_end;

   // Progress-tracking member variables
   const char **current_arg;
   const raOpt *current_option;
   const raOpt *last_position_option;
   const char *sub_arg_ptr;
} raCache;

typedef enum {
   RA_SUCCESS = 0,
   RA_END_OPTIONS,
   RA_UNKNOWN_OPTION,
   RA_MALFORMED_OPTION
} raStatus;

/**
 * Global variables.
 */
extern const char *g_command_name;
extern const raCache *g_cache;

// Agent implementations
const raAgent ra_int_agent;
const raAgent ra_string_agent;
const raAgent ra_show_help_agent;

/* Use for fifth (opt_count) parameter of ra_init_cache(): */
#define OPTS_COUNT(a) (sizeof((a)) / sizeof(raOpt))


/** raCache manipulating functions */
int ra_init_cache(raCache *cache,
                  const char **start_arg,
                  int arg_count,
                  const raOpt *start_opt,
                  int opt_count);

const char *ra_command_name(const raCache *cache);
const char *ra_next_arg(raCache *cache);
const char *ra_cur_arg(const raCache *cache);
const raOpt *ra_cur_option(const raCache *cache);
int ra_next_option(raCache *cache, const raOpt **option, const char **value);

void ra_execute_option_read(const raOpt *option, const char *str);

void ra_show_options(FILE *f, const raCache *cache);

// Error messaging
void ra_warn_invalid_input(FILE *f, const raOpt *option, const char *message);

int arguments_required(const raOpt *opt);



#endif
