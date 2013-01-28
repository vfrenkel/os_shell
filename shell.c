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
 */
int process_input(char *input) {
  struct SLList commands;
  init_list(&commands);

  char input_copy[strlen(input)+1];
  strcpy(input_copy, input);

  const char special_delims[] = "<>|";
  char *token = NULL;

  // break up commands into the list and collect argument lists.
  do {
    if (token == NULL) {
      token = strtok(input_copy, special_delims);
    } else {
      token = strtok(NULL, special_delims);
    }

    add_back(&commands, token);

  } while (token != NULL);

  if (DEBUG) {
    traverse(&commands, (void*)printf);
  }

  // clean up. make sure to free argument lists, then command list.
  //destroy_list(&commands); // TODO: make this a list command

  return 0;
}
