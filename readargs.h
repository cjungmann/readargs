#ifndef READARGS_H
#define READARGS_H

#define EXPORT __attribute__((visibility("default")))

#include <stdio.h>

typedef struct _readargs_option raOpt;

typedef enum {
   RA_SUCCESS = 0,
   RA_END_OPTIONS,
   RA_END_ARGS,
   RA_UNKNOWN_OPTION,
   RA_MALFORMED_OPTION,
   RA_INVALID_ARGUMENT,
   RA_MISSING_TARGET,
   RA_MISSING_AGENT,
   RA_MISSING_READER
} raStatus;

typedef raStatus (*raReader)(const raOpt *opt, const char *str);
typedef void (*raWriter)(FILE *f, const raOpt *opt);

typedef struct _readargs_agent
{
   int args_needed;
   raReader reader;
   raWriter writer;
} raAgent;

/** Library's fundamental data structure. */
typedef struct _readargs_option
{
   int           letter;         /* letter that invokes the option */
   const char    *label;         /* long-option name invoking option */
   const char    *type;          /* optional string to describe the value */
   const char    *description;   /* documents option for help output */
   const raAgent *agent;         /* contains function pointers that process the option */
   void          *target;        /* optional pointer to data that will be modified by the agent */
} raOpt;

/**
 * Special option types:
 * - A flag option sets an integer value to 1.
 *     Identified by the raAgent that has set
 *     args_needed == 0;
 *
 * - A positional argument is invoked when an
 *     argument is not preceeded by a short or
 *     long option name.
 *     This option type is indicated by two
 *     raOpt settings: raOpt::letter < 0
 *     and raOpt::label is prefixed with an
 *     asterisk
 */


typedef struct _readargs_scene
{
   // Collections of CL arguments and option definitions
   const char **args;
   const char **args_end;
   const raOpt *options;
   const raOpt *options_end;
} raScene;

typedef struct _readargs_tour
{
   const char **current_arg;
   const char *sub_arg_ptr;
   const raOpt *current_option;
   const raOpt *last_position_option;
} raTour;


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

/**
 * Global variables.
 */
extern raScene g_scene;

// Agent implementations
extern const raAgent ra_flag_agent;
extern const raAgent ra_int_agent;
extern const raAgent ra_string_agent;
extern const raAgent ra_show_help_agent;
extern const raAgent ra_show_values_agent;

/* Use for fifth (opt_count) parameter of ra_init_cache(): */
#define OPTS_COUNT(a) (sizeof((a)) / sizeof(raOpt))


/** raCache manipulating functions */
int ra_init_cache(raCache *cache,
                  const char **start_arg,
                  int arg_count,
                  const raOpt *start_opt,
                  int opt_count);

EXPORT const char *ra_command_name(void);
EXPORT void ra_set_scene(const char **start_arg,
                         int arg_count,
                         const raOpt *start_opt,
                         int opt_count);
EXPORT raTour *ra_start_tour(raTour *tour);

EXPORT const char* ra_advance_arg(raTour *tour);
EXPORT raStatus ra_advance_option(raTour *tour, const raOpt **option, const char **value);

// Option characteristic test functions:
int ra_is_positional_option(const raOpt* opt);
int ra_is_named_option(const raOpt* opt);
int ra_is_flag_option(const raOpt* opt);
int ra_is_value_option(const raOpt* opt);
int ra_is_writable_option(const raOpt* opt);


const char *ra_next_arg(raCache *cache);
const char *ra_cur_arg(const raCache *cache);
const raOpt *ra_cur_option(const raCache *cache);
int ra_next_option(raCache *cache, const raOpt **option, const char **value);

int ra_scene_options_count(void);
int ra_scene_arguments_count(void);

raStatus ra_execute_option_read(const raOpt *option, const char *str);
void ra_execute_option_write(FILE *f, const raOpt *option);

void ra_show_scene_values(FILE *f);

// functions that help build a --help display
typedef enum _readargs_usage_format
{
   RAU_DEFAULT=0,
   RAU_SHORT,
   RAU_LONG
} raUsage;

void ra_describe_options(FILE *f, int indent);
void ra_describe_arguments(FILE *f, int indent);
void ra_describe_usage(FILE *f, int indent, raUsage usage);
void ra_show_help(FILE *f, int indent, raUsage usage);

// Error messaging

void ra_write_warning(FILE *f,
                      raStatus status,
                      const raTour *tour,
                      const raOpt *option,
                      const char *value);

int arguments_required(const raOpt *opt);



#endif
