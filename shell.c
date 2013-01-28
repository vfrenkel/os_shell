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

struct Cmd {
  char *name;
  struct SLList *args;
  IOModifier mod;
};

struct Cmd *make_cmd(char *cmd_string, char mod) {
  struct Cmd *cmd;
  struct SLList *args;

  if (!(cmd = (struct Cmd *)malloc(sizeof(struct Cmd)))) {
    printf("error: could not allocate memory for command");
  }
  if (!(args = (struct SLList *)malloc(sizeof(struct SLList)))) {
    printf("error: could not allocate memory for command");
  }

  init_list(args);
  
  //extract the necessary data from the command string.
  char *token = strtok(cmd_string, " ");
  char *name = token;
  while (token != NULL) {
    token = strtok(NULL, " ");
    add_back(args, token);
  }

  if (DEBUG) {
    printf("cmd name: %s\n", name);
    printf("arguments: ");
    traverse(args, (void*)printf);
  }
  

  return cmd;
}

/*
 * Parses and keeps a list of commands with pointers to their arguments
 * and then carries out the requested actions. Returns 0 on success.
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
    tmp_cmd_string[cmd_str_length] = '\0';
    char *cmd_string = strdup(tmp_cmd_string);

    char modifier = input_current_start[cmd_str_length];

    add_back(&commands, make_cmd(cmd_string, modifier));

    input_current_start += cmd_str_length+1;

    free(tmp_cmd_string);
  } while (*(input_current_start-2) != '\n');

  //clean up
  //TODO: use a helper function that actually goes through Cmd structs
  // and frees their memory as well.
  while (!is_empty(&commands)) {
    pop_front(&commands);
  }

  return 0;
}

void cmd_exit() {
  exit(0);
}


