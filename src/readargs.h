#ifndef READARGS_H
#define READARGS_H

#define EXPORT __attribute__((visibility("default")))

#include <stdio.h>

// Forward declarations for function pointer typedefs
typedef struct _readargs_agent raAgent;
typedef struct _readargs_action raAction;
typedef struct _readargs_tour raTour;
typedef struct _readargs_scene raScene;

typedef enum {
   RA_SUCCESS = 0,
   RA_FAIL,
   // Non-error status values:
   RA_CANCEL,
   RA_END_OPTIONS,
   RA_END_ARGS,
   RA_BEFORE_ARGS,
   RA_FATAL_ERROR,
   // Programming errors:
   RA_MISSING_TARGET,
   RA_MISSING_AGENT,
   RA_MISSING_READER,
   // User-entry errors:
   RA_UNKNOWN_OPTION,
   RA_MALFORMED_OPTION,
   RA_INVALID_ARGUMENT,
   RA_MISSING_ARGUMENT,
   RA_MISSING_VALUE
} raStatus;

typedef raStatus (*raReader)(const raAction *act, const char *str, raTour *tour);
typedef void (*raWriter)(FILE *f, const raAction *act);

struct _readargs_agent
{
   int args_needed;
   raReader reader;
   raWriter writer;
};

/** Library's fundamental data structure. */
struct _readargs_action
{
   int           letter;   /* letter that invokes the action */
   const char    *label;   /* long-action name invoking action */
   const char    *comment; /* documents action for help output */
   const raAgent *agent;   /* object that manages value */
   void          *target;  /* actional data pointer */
   const char    *type;    /* actional string to describe the value */
};

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


struct _readargs_scene
{
   // Collections of CL arguments and action definitions
   const char **args;
   const char **args_end;
   const raAction *actions;
   const raAction *actions_end;
};

struct _readargs_tour
{
   const char **current_arg;
   const char *sub_arg_ptr;
   const raAction *current_action;
   const raAction *last_position_action;
};


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
#define ACTS_COUNT(a) (sizeof((a)) / sizeof(a[0]))


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
int ra_scene_argument_actions_count(void);
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
void ra_suggest_help(FILE *f, int indent);

// Post-processing tools for getting missing information
int ra_count_str2args(const char *str);
int ra_str2args(char *str, const char **args, int arg_count);

int ra_collect_user_response(void);
int ra_copy_user_response(char *buffer, int buffer_len);

void ra_print_interactive_prompt(const raAction *action);
raStatus ra_process_action_with_response(const raAction *action, char *response);

typedef struct ra_xa_env {
   int buff_len;
   char *buff;
} RA_XA_ENV;

#define RA_READ_XA_RESPONSE(raenv) (raenv).buff_len = ra_collect_user_response(); \
   (raenv).buff = (char*)alloca((raenv).buff_len); \
   ra_copy_user_response((raenv).buff, (raenv).buff_len)

#define RA_PROCESS_XA_ACTION(a, env) ra_process_action_with_response(a, (env).buff)

// Error messaging

void ra_write_warning(FILE *f,
                      raStatus status,
                      const raTour *tour,
                      const raAction *action,
                      const char *value);

void ra_show_no_args_message(FILE *f);
void ra_show_unknown_option_message(FILE *f, const raTour *tour);

// Let the library do everything:
int ra_process_tour_arguments(raTour *tour,
                              int alert_no_args,
                              int alert_unknown_option);

int ra_process_arguments(void);
int ra_process_arguments_required(void);





#endif
