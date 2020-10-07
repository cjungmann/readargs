#include "readargs.h"
#include "invisible.h"

#include <string.h>
#include <alloca.h>

EXPORT int ra_is_positional_action(const raAction* act)
{
   return act->letter < 0
      && (act->label && *act->label == '*');
}

EXPORT int ra_is_named_action(const raAction *act)
{
   return act->letter > 0
      || (act->label && *act->label != '*');
}

EXPORT int ra_is_flag_action(const raAction *act)
{
   return act->agent && act->agent->args_needed==0;
}

EXPORT int ra_is_value_action(const raAction *act)
{
   return act
      && !(act->label && *act->label=='*')
      && act->agent
      && act->agent->args_needed;
}

EXPORT int ra_is_writable_action(const raAction *act)
{
   return act && act->agent && act->agent->writer;
}

EXPORT const raAction *ra_seek_raAction_by_label(const char *str)
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

   const raAction *ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (strcmp(ptr->label, str) == 0)
         return ptr;

      ++ptr;
   }

   return NULL;
}

EXPORT const raAction *ra_seek_raAction_by_letter(char letter)
{
   const raAction *ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (ptr->letter == letter)
         return ptr;

      ++ptr;
   }

   return NULL;
}

/** Find matching action, if any.  May return a positional action if available.
 *
 * This function is meant to be used for an action with
 * an actional value to determine if the value following
 * the action invocation is another action.
 *
 * Using this function is not required, it's just offered
 * in case a project requires discretion in handling the 
 * actional value.
 */
EXPORT const raAction *ra_seek_raAction(const char *str, const raTour *tour)
{
   const raAction *act = NULL;
   
   if (*str == '-')         // is action
   {
      ++str;

      if (*str == '-')    // may be long action
      {
         ++str;

         if (!*str)         // is double-dash, cannot be an action
            return NULL;
         else
            act = ra_seek_raAction_by_label(str);
      }
      else                  // is short action
         act = ra_seek_raAction_by_letter(*str);
   }
   else                     // may be positional action
   {
      // Make tour copy to preserve constness
      // for positional action search.
      raTour ltour = *tour;

      act = seek_next_positional_action(&ltour);
   }

   return act;
}

/** Marker to indicate completion of positional actions.
 * This value is written to raTour::last_position_action
 * when there are no more positional argument.
 */
static const raAction *position_actions_done = (const raAction*)(-1);

/** Iteratively returns positional arguments.
 * This function is called to determine which action
 * to apply to command line arguments that are not 
 * labeled actions.
 */
const raAction *seek_next_positional_action(raTour *tour)
{
   const raAction *act = tour->last_position_action;
   if (act == position_actions_done)
      return NULL;
   else if (act == NULL)
      act = g_scene.actions - 1;

   while (++act < g_scene.actions_end)
      if (ra_is_positional_action(act))
      {
         tour->last_position_action = act;
         return act;
      }

   tour->last_position_action = position_actions_done;
   return NULL;
}

/**
 * Count raActions that are NOT positional.
 *
 * Use this function while displaying help to determine
 * if an actions section is required.
 */
EXPORT int ra_scene_actions_count(void)
{
   int count = 0;
   const raAction *ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (!ra_is_positional_action(ptr))
         ++count;
      
      ++ptr;
   }

   return count;
}

/**
 * Count raActions that ARE positional.
 *
 * Use this function while displaying help to determine
 * if an arguments section is required.
 */
EXPORT int ra_scene_argument_actions_count(void)
{
   int count = 0;
   const raAction *ptr = g_scene.actions;
   while (ptr < g_scene.actions_end)
   {
      if (ra_is_positional_action(ptr))
         ++count;
      
      ++ptr;
   }

   return count;
}

/**
 * Returns the number of command line arguments
 */
EXPORT int ra_scene_arguments_count(void)
{
   return g_scene.args_end - g_scene.args;
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
 * Initialize the readacts environment.
 */
EXPORT void ra_set_scene(const char **start_arg,
                         int arg_count,
                         const raAction *start_act,
                         int act_count)
{
   g_scene.args = start_arg;
   g_scene.args_end = start_arg + arg_count;
   g_scene.actions = start_act;
   g_scene.actions_end = start_act + act_count;
}

/**
 * Initialize a new tour of actions.
 * 
 * This initializes a new set of pointers
 * that tracks progress through argument/actions
 * processing.
 */
EXPORT raTour *ra_start_tour(raTour *tour)
{
   tour->current_arg = g_scene.args;
   tour->current_action = g_scene.actions;

   tour->sub_arg_ptr = NULL;
   tour->last_position_action = NULL;

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

/** Decrement current_arg so it will be considered for the next action.
 *
 * This function is used for an action that has an actional value,
 * so the agent can consider the action's value and decide whether
 * or not it is the value or it is the next action.
 */
EXPORT raStatus ra_retreat_arg(raTour *tour)
{
   if (tour->current_arg > g_scene.args)
   {
      --tour->current_arg;
      return RA_SUCCESS;
   }
   else
      return RA_BEFORE_ARGS;
}

/**
 * Get the next action, with a value if appropriate.
 * 
 * When the return value is RA_SUCCESS, the program
 * should process the action.
 *
 * Non-RA_SUCCESS values may indicate the end of the
 * list of arguments, an error, or the end of actions.
 * Process these values according to the needs of your
 * program.
 */
EXPORT raStatus ra_advance_action(raTour *tour, const raAction **action, const char **value)
{
   *action = NULL;
   *value = NULL;

   const char **actions_str = &tour->sub_arg_ptr;
   const raAction *act = NULL;

   if (! *actions_str || ! **actions_str)
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
               if (*arg)  // long action, double-dash + label
               {
                  act = ra_seek_raAction_by_label(arg);
                  if (act)
                  {
                     *action = act;

                     if (ra_is_value_action(act))
                     {
                        const char *tstr = strchr(arg,'=');
                        if (tstr)
                           *value = tstr+1;
                        else
                           *value = ra_advance_arg(tour);
                     }

                     return RA_SUCCESS;
                  }
                  else
                     return RA_UNKNOWN_OPTION;
               }
               else       // actions terminator, naked double-dash "--"
                  return RA_END_OPTIONS;
            }
            else          // short action, single-dash + letter
               *actions_str = arg;
         }
         else // not an action, read as next positional action
         {
            act = seek_next_positional_action(tour);
            if (act)
            {
               *action = act;
               *value = arg;
               return RA_SUCCESS;
            }
         }
      }
   }

   if (*actions_str)
   {
      char letter = **actions_str;
      ++*actions_str;
      act = ra_seek_raAction_by_letter(letter);
      if (act)
      {
         *action = act;
         if (ra_is_value_action(act))
         {
            if (**actions_str)
            {
               *value = *actions_str;

               // clear so next iteration will be new argument
               *actions_str = NULL;
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

EXPORT const char* ra_current_arg(const raTour *tour)
{
   return *(tour->current_arg);
}

EXPORT const raAction *ra_current_action(const raTour *tour)
{
   return tour->current_action;
}

/**
 * Convenience function for writing out an error.
 *
 * This function is used by the default raTour processor,
 * but it is actional for custom implementations.
 */
EXPORT void ra_write_warning(FILE *f,
                             raStatus status,
                             const raTour *tour,
                             const raAction *action,
                             const char *value)
{
   fprintf(f, "%s: ", ra_command_name());

   if (action)
   {
      if (action->letter)
         fprintf(f, "-%c: ", action->letter);
      else if (action->label)
         fprintf(f, "--%s: ", action->label);
   }

   switch(status)
   {
      case RA_SUCCESS:
         fprintf(f, "no error\n");
         break;
      case RA_END_OPTIONS:
         fprintf(f, "encountered argument '--'\n");
         break;
      case RA_END_ARGS:
         fprintf(f, "reached the end of the arguments\n");
         break;
      case RA_FATAL_ERROR:
         fprintf(f, "generic fatal error\n");
         break;

         /* Programming errors */
      case RA_MISSING_TARGET:
         fprintf(f, "action missing target\n");
         break;
      case RA_MISSING_AGENT:
         fprintf(f, "action missing agent\n");
         break;
      case RA_MISSING_READER:
         fprintf(f, "action missing reader\n");
         break;

         /* User-entry errors */
      case RA_UNKNOWN_OPTION:
         fprintf(f, "unknown option \"%s\"\n", *tour->current_arg);
         break;
      case RA_MALFORMED_OPTION:
         fprintf(f, "option value in bad form\n");
         break;
      case RA_INVALID_ARGUMENT:
         fprintf(f, "invalid argument\n");
         if (action)
         {
            describe_single_action(f, action, 0, 0);
            ra_suggest_help(f,0);
         }
         break;
      case RA_MISSING_ARGUMENT:
         fprintf(f, "missing argument\n");
         break;
      case RA_MISSING_VALUE:
         fprintf(f, "action missing value\n");
         if (action)
            describe_single_action(f, action, 0, 0);
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

raAction actions[] = {
   { 'h', "help",    "This help message", NULL,             NULL },
   { 'v', "version", "Version number",    NULL,             NULL },
   { 'p', "path",    "Set file path",     &ra_string_agent, &path },
   { 'n', "number",  "Set number value",  &ra_int_agent,    &number },
   { 'f', "flag",    "Set flag to on",    &ra_flag_agent,   &flag },
   { -1,  "*input",  "input file",        &ra_string_agent, &input_file },
   { -1,  "*reps",   "repetitions",       &ra_int_agent,    &repetitions }
};

void display_action(const raAction *action, const char *value, int wellformed)
{
   if (!wellformed)
      printf("Malformed action: ");

   if (action->letter)
      printf("-%c", action->letter);
   else if (action->label)
      printf("--%s", action->label);

   if (value)
      printf(":%s\n", value);
   else
      printf("\n");
}

int main(int argc, const char **argv)
{
   raTour tour;
   ra_set_scene(argv, argc, actions, ACTS_COUNT(actions));
   ra_start_tour(&tour);

   const char *arg;
   int count = 0;

   while ((arg = ra_advance_arg(&tour)))
      printf("%2d: %s\n", ++count, arg);

   const raAction *action;
   const char *value;

   ra_start_tour(&tour);

   while (1)
   {
      switch(ra_advance_action(&tour, &action, &value))
      {
         case RA_SUCCESS:
            switch(ra_execute_action_read(action, value))
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
            goto end_action_walk;
         case RA_UNKNOWN_ACTION:
            fprintf(stderr, "Action %s is not known.\n", *tour.current_arg);
            break;
         case RA_MALFORMED_ACTION:
            display_action(action, value, 0);
            break;
         case RA_MISSING_TARGET:
            fprintf(stderr, "%s: missing target for action.\n", ra_command_name());
            break;
         case RA_INVALID_ARGUMENT:
            fprintf(stderr, "%s: argument value is the wrong type.\n", ra_command_name());
            break;
         default:
            break;
      }
   }
end_action_walk:

   return 0;
}

#endif



/* Local Variables: */
/* compile-command: "b=scene; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */
