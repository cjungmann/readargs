typedef enum _raAction_filter
{
   ROF_ACTIONS = 1,
   ROF_ARGS = 2,
   ROF_ALL = 3,
   ROF_VALUES = 8,
   ROF_TYPES = 16
} ActFilter;

/** invisible functions found in readargs.c */
int include_in_action_list(ActFilter set, const raAction *ptr);
int action_labeler(const raAction *ptr, ActFilter set, char *buffer, int len_buffer);
int get_label_length(const raAction *ptr, ActFilter set);
int set_label_value(const raAction *ptr, ActFilter set, char *buffer, int buffer_len);
int get_max_label_length(ActFilter set);
int get_max_argument_length(void);

/** invisible functions found in help.c */
void print_action_names(FILE *f, const raAction *opt, int max_label, ActFilter set);
void print_usage_flag_actions(FILE *f);
void print_usage_value_actions(FILE *f);
void print_usage_arguments(FILE *f);
void describe_single_action(FILE *f, const raAction *opt, int max_label, int indent);
void describe_single_argument(FILE *f, const raAction *act, int max_label, int indent);

/** invisible functions found in scene.c */
const raAction *seek_raOpt_by_label(const char *str);
const raAction *seek_raOpt_by_letter(char letter);
const raAction *seek_next_positional_action(raTour *tour);

/** invisible functions found in agents.c  */
int str_is_number(const char *str);
int confirm_action_target(const raAction *opt);
/* ra_flag_agent */
raStatus flag_reader(const raAction *act, const char *str, raTour *tour);
void flag_writer(FILE *f, const raAction *act);
/* ra_int_agent */
raStatus int_reader(const raAction *act, const char *str, raTour *tour);
void int_writer(FILE *f, const raAction *act);
/* ra_string_agent */
raStatus string_reader(const raAction *act, const char *str, raTour *tour);
void string_writer(FILE *f, const raAction *act);
/* ra_help_agent */
raStatus show_help_reader(const raAction *act, const char *str, raTour *tour);
/* ra_show_values_agent */
raStatus show_action_values_reader(const raAction *opt, const char *value, raTour *tour);





