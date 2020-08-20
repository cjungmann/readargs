#include <ctype.h>
#include "readargs.h"  // for EXPORT macro

typedef int(*weemain_t)(int argc, const char **argv);

int string2args(char *str, const char **args, int arg_count)
{
   int count = 0;
   int in_margin = 0;
   int in_quote = 0;
   int is_escaped = 0;

   // Unless first character is a space, the beginning
   // of the string is the first argument.  Set variables
   // accordingly.
   if (!isspace(*str))
   {
      count = 1;
      if (args)
         *args++ = str;
   }

   int is_space;
   char *ptr = str;
   while (*ptr)
   {
      if (is_escaped)
      {
         // Ignore current character, whether it is
         // in margin or in arg.
         is_escaped = 0;
      }
      else if (*ptr == '\\')
      {
         is_escaped = 1;
      }
      else
      {
         is_space = isspace(*ptr);
         
         if (in_margin)
         {
            if (!is_space)
            {
               ++count;
               in_margin = 0;

               if (args && count <= arg_count)
                  *args++ = ptr;
            }
         }
         else if (in_quote)
         {
            if (*ptr == in_quote)
               in_quote = 0;
         }
         else if (!in_quote)
         {
            if (is_space)
            {
               in_margin = 1;

               if (args && count <= arg_count)
                  *ptr = '\0';
            }
         }
      }

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

