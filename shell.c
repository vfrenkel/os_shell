/* 
 * Operating Systems - Homework One
 * Victor Frenkel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sllist.h"
#include "shell.h"

#define DEBUG 1

typedef enum CMD_IO_MODIFIER {
  INPUT_REDIR,
  OUTPUT_REDIR,
  PIPE
} IOModifier;

struct cmd {
  struct SLList args;
  IOModifier mod;
};

/*
 * Parses and keeps a list of commands with pointers to their argument
 * lists (which can be empty), then carries out the requested actions.
 * Returns 0 on success.
 */
int process_input(char *input) {
  struct SLList commands;
  init_list(&commands);

  char *input_current_start = input;

  const char special_delims[] = "<>|";

  // break up commands into the list and collect argument lists for them.
  do {
    int cmd_str_length = strcspn(input_current_start, special_delims);
    char *tmp_cmd_string = (char *)malloc(cmd_str_length+1);

    if (!tmp_cmd_string) {
      printf("error: could not allocate memory for input processing.");
    }

    strncpy(tmp_cmd_string, input_current_start, cmd_str_length);
    tmp_cmd_string[cmd_str_length] = '\n';
    char *cmd_string = strdup(tmp_cmd_string);

    // create a cmd struct and add it to the commands list.
    add_back(&commands, cmd_string);

    char modifier = input_current_start[cmd_str_length];
    input_current_start += cmd_str_length+1;

    free(tmp_cmd_string);

  } while (*(input_current_start-1) != '\000');

  /* if (DEBUG) { */
  /*   traverse(&commands, (void*)printf); */
  /* } */

  // clean up. make sure to free argument lists, then command list.
  //destroy_list(&commands); // TODO: make this a list command
  while (!is_empty(&commands)) {
    printf((char*)pop_front(&commands));
  }

  return 0;
}
