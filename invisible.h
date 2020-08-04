typedef enum _raOpt_filter
{
   ROF_OPTIONS = 1,
   ROF_ARGS = 2,
   ROF_ALL = 3,
   ROF_VALUES = 8,
   ROF_TYPES = 16
} OptFilter;

/** invisible functions found in readargs.c */
int include_in_option_list(OptFilter set, const raOpt *ptr);
int option_labeler(const raOpt *ptr, OptFilter set, char *buffer, int len_buffer);
int get_label_length(const raOpt *ptr, OptFilter set);
int set_label_value(const raOpt *ptr, OptFilter set, char *buffer, int buffer_len);
int get_max_label_length(OptFilter set);
int get_max_argument_length(void);

/** invisible functions found in help.c */
void print_option_names(FILE *f, const raOpt *opt, int max_label, OptFilter set);
void print_usage_flag_options(FILE *f);
void print_usage_value_options(FILE *f);
void print_usage_arguments(FILE *f);
void describe_single_option(FILE *f, const raOpt *opt, int max_label, int indent);
void describe_single_argument(FILE *f, const raOpt *opt, int max_label, int indent);

/** invisible functions found in scene.c */
const raOpt *seek_raOpt_by_label(const char *str);
const raOpt *seek_raOpt_by_letter(char letter);
const raOpt *seek_next_positional_option(raTour *tour);

/** invisible functions found in agents.c  */
int str_is_number(const char *str);
int confirm_option_target(const raOpt *opt);
/* ra_flag_agent */
raStatus flag_reader(const raOpt *opt, const char *str, raTour *tour);
void flag_writer(FILE *f, const raOpt *opt);
/* ra_int_agent */
raStatus int_reader(const raOpt *opt, const char *str, raTour *tour);
void int_writer(FILE *f, const raOpt *opt);
/* ra_string_agent */
raStatus string_reader(const raOpt *opt, const char *str, raTour *tour);
void string_writer(FILE *f, const raOpt *opt);
/* ra_help_agent */
raStatus show_help_reader(const raOpt *opt, const char *str, raTour *tour);
/* ra_show_values_agent */
raStatus show_option_values_reader(const raOpt *opt, const char *value, raTour *tour);





