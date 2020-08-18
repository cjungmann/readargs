#ifndef READARGS_H
#define READARGS_H

#define EXPORT __attribute__((visibility("default")))

#include <stdio.h>

// Forward declarations for function pointer typedefs
typedef struct _readargs_action raAction;
typedef struct _readargs_tour raTour;

typedef enum {
   RA_SUCCESS = 0,
   RA_FAIL,
   RA_CANCEL,
   RA_END_OPTIONS,
   RA_END_ARGS,
   RA_BEFORE_ARGS,
   RA_UNKNOWN_OPTION,
   RA_MALFORMED_ACTION,
   RA_INVALID_ARGUMENT,
   RA_MISSING_TARGET,
   RA_MISSING_AGENT,
   RA_MISSING_READER,
   RA_MISSING_VALUE
} raStatus;

typedef raStatus (*raReader)(const raAction *act, const char *str, raTour *tour);
typedef void (*raWriter)(FILE *f, const raAction *act);

typedef struct _readargs_agent
{
   int args_needed;
   raReader reader;
   raWriter writer;
} raAgent;

/** Library's fundamental data structure. */
typedef struct _readargs_action
{
   int           letter;   /* letter that invokes the action */
   const char    *label;   /* long-action name invoking action */
   const char    *comment; /* documents action for help output */
   const raAgent *agent;   /* object that manages value */
   void          *target;  /* actional data pointer */
   const char    *type;    /* actional string to describe the value */
} raAction;

/**
 * Special action types:
 * - A flag action sets an integer value to 1.
 *     Identified by the raAgent that has set
 *     args_needed == 0;
 *
 * - A positional argument is invoked when an
 *     argument is not preceeded by a short or
 *     long action name.
 *     This action type is indicated by two
 *     raAction settings: raOpt::letter < 0
 *     and raAction::label is prefixed with an
 *     asterisk
 */


typedef struct _readargs_scene
{
   // Collections of CL arguments and action definitions
   const char **args;
   const char **args_end;
   const raAction *actions;
   const raAction *actions_end;
} raScene;

typedef struct _readargs_tour
{
   const char **current_arg;
   const char *sub_arg_ptr;
   const raAction *current_action;
   const raAction *last_position_action;
} raTour;


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

/* Use for fifth (act_count) parameter of ra_init_cache(): */
#define ACTS_COUNT(a) (sizeof((a)) / sizeof(raAction))


const char *ra_command_name(void);
void ra_set_scene(const char **start_arg,
                  int arg_count,
                  const raAction *start_action,
                  int action_count);
raTour *ra_start_tour(raTour *tour);

const char* ra_advance_arg(raTour *tour);
raStatus ra_retreat_arg(raTour *tour);
raStatus ra_advance_action(raTour *tour, const raAction **action, const char **value);

const char* ra_current_arg(const raTour *tour);
const raAction *ra_current_action(const raTour *tour);

const raAction *ra_seek_raAction_by_label(const char *str);
const raAction *ra_seek_raAction_by_letter(char letter);
const raAction *ra_seek_raAction(const char *str, const raTour *tour);

// Action characteristic test functions:
int ra_is_positional_action(const raAction* opt);
int ra_is_named_action(const raAction* opt);
int ra_is_flag_action(const raAction* opt);
int ra_is_value_action(const raAction* opt);
int ra_is_writable_action(const raAction* opt);

int ra_scene_actions_count(void);
int ra_scene_arguments_count(void);

raStatus ra_execute_action_read(const raAction *action, const char *str, raTour *tour);
void ra_execute_action_write(FILE *f, const raAction *action);

void ra_show_scene_values(FILE *f);

// functions that help build a --help display
typedef enum _readargs_usage_format
{
   RAU_DEFAULT=0,
   RAU_SHORT,
   RAU_LONG
} raUsage;

void ra_describe_arguments(FILE *f, int indent);
void ra_describe_actions(FILE *f, int indent);
void ra_describe_usage(FILE *f, int indent, raUsage usage);
void ra_show_help(FILE *f, int indent, raUsage usage);

// Error messaging

void ra_write_warning(FILE *f,
                      raStatus status,
                      const raTour *tour,
                      const raAction *action,
                      const char *value);

int arguments_required(const raAction *act);

// Let the library do everything:
int ra_process_arguments(void);
void ra_show_no_args_message(void);




#endif
