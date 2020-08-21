#include <ctype.h>
#include <string.h>    // for memmove, strlen

#include "readargs.h"  // for EXPORT macro

char* popper(char *str)
{
   int len = strlen(str);
   memmove(str, str+1, len-1);
   str[len-1] = '\0';
   return str;
}

char* skipper(char *str)
{
   return str+1;
}

typedef char*(*discard_char_t)(char *str);

int is_quote(char chr)     { return chr == '\'' || chr == '"'; }

int string2args(char *str, const char **args, int arg_count)
{
   int count = 0;
   int in_margin = 0;
   int in_quote = 0;

   discard_char_t discard_char = args ? popper : skipper;

   // bleed-off initial spaces
   while (*str && isspace(*str))
      ++str;

   if (!*str)
      return 0;
   else
   {
      count = 1;
      if (args)
         *args++ = str;
   }

   int is_space;
   char *ptr = str;
   while (*ptr)
   {
      // Quote check must precede check for end-of-argument
      // because *ptr==0 after matching end-of-argument.
      if (in_quote == *ptr)
      {
         in_quote = 0;
         ptr = (*discard_char)(ptr);
      }
      
      is_space = isspace(*ptr);

      // Check for end-of-argument, indicated by a space
      if (is_space && !in_quote && !in_margin)
      {
         in_margin = 1;
         if (args)
            *ptr = '\0';
      }
      // Check for start of next word, first character following one or more spaces
      else if (in_margin && !is_space)
      {
         in_margin = 0;
         ++count;

         if (args && count <= arg_count)
            *args++ = ptr;
      }

      // Check for start of a quote
      if (!in_quote && is_quote(*ptr))
      {
         in_quote = *ptr;
         ptr = (*discard_char)(ptr);
      }
      // If escaping character, discard backslash, then do not
      // judge if the following character for quote or space.
      else if (*ptr == '\\')
         ptr = (*discard_char)(ptr);
      
      ++ptr;
   }

   return count;
}

int ra_count_str2args(const char *str)
{
   return string2args((char*)str, 0, 0);
}

int ra_str2args(char *str, const char **args, int arg_count)
{
   return string2args(str, args, arg_count);
}


#ifdef STR2ARGS_MAIN

#include <stdio.h>
#include <alloca.h>
#include <string.h>

typedef int(*weemain_t)(int argc, const char **argv);

int test_main(int argc, const char **argv)
{
   const char **end = argv + argc;
   const char **ptr = argv;
   int count = 0;

   while (ptr < end)
   {
      printf("%d: [32;1m%s[m\n", count++, *ptr);
      ++ptr;
   }


   return 0;
}

void test_str2args(const char *str)
{
   int count = ra_count_str2args(str);
   printf("\n[32;1m%s[m has %d arguments.\n", str, count);
}

int parse_string_to_new_main(const char *command_name, const char *str, weemain_t main)
{
   int count = ra_count_str2args(str);

   if (count)
   {
      // Leave room for command line name
      ++count;
      
      char *copy_of_str = (char*)(alloca(1+strlen(str)));
      strcpy(copy_of_str, str);

      const char **argv = (const char **)alloca(count * sizeof(char*));
      argv[0] = command_name;

      string2args(copy_of_str, &argv[1], count-1);

      return (*main)(count, argv);
   }
   else
      return (*main)(1, &command_name);
}

int fake_main(int argc, const char **argv)
{
   printf("Showing fake main:\n");
   for (int i=0; i<argc; ++i, ++argv)
      printf("%s\n", *argv);

   return 0;
}

void test_lines_set(const char *command_name)
{
   const char *lines[] = {
      "-p poopie -rasdfa loser \"trump\"",
      "one two three four",
      "-p poopie -rasdfa loser\"trump\"",
      " -p poopie -rasdfa loser\"trump\""
   };

   const char **lines_end = lines + ( sizeof(lines) / sizeof(char*) );

   const char **ptr = lines;
   while (ptr < lines_end)
   {
      test_str2args(*ptr);
      parse_string_to_new_main(command_name, *ptr, test_main);
      ++ptr;
   }
}

void test_reader(void)
{
   char buffer[1024] = {0};
   int buffer_len = sizeof(buffer);
   int string_len = 0;
   printf("Enter your string: ");
   fgets(buffer, buffer_len, stdin);

   // check for overflow:
   if (buffer[buffer_len-1])
      printf("Overflowed buffer!\n");
   else
   {
      string_len = strlen(buffer);
      if (buffer[string_len-1] == '\n')
      {
         --string_len;
         buffer[string_len] = '\0';
      }

      printf("\n you entered a %d character string: [32;1m%s[m.\n", string_len, buffer);

      parse_string_to_new_main("bogus", buffer, test_main);

      
   }
}


int main(int argc, const char **argv)
{
   /* test_lines_set(*argv); */
   /* test_main(argc, argv); */

   test_reader();


   return 0;
}

#endif

/* Local Variables: */
/* compile-command: "b=str2args; \*/
/*  gcc -Wall -Werror -ggdb      \*/
/*  -D${b^^}_MAIN -o $b ${b}.c"  \*/
/* End: */

