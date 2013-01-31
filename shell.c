/* 
 * Operating Systems - Homework One
 * Victor Frenkel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sllist.h"
#include "shell.h"

#define DEBUG 1

struct Token *make_token(char *token_string, char mod) {
  struct Token *token;
  struct SLList *args;

  if (!(token = (struct Token *)malloc(sizeof(struct Token)))) {
    printf("error: could not allocate memory for command");
  }
  if (!(args = (struct SLList *)malloc(sizeof(struct SLList)))) {
    printf("error: could not allocate memory for command");
  }

  init_token(token);
  init_list(args);
  
  //extract the necessary data from the command string.
  char *token_part = strtok(token_string, " ");
  char *name = token_part;
  token->name = name;
  add_back(args, name);
  while (token_part != NULL) {
    token_part = strtok(NULL, " ");
    add_back(args, token_part);
  }

  token->args = args;

  //assign correct modifier indicator based on mod character.
  if (mod == '|') {
    token->mod = PIPE;
  } else if (mod == '>') {
    token->mod = OUTPUT_REDIR;
  } else if (mod == '<') {
    token->mod = INPUT_REDIR;
  } else {
    //printf("error: bad token: %c", mod);
  }

  if (DEBUG) {
    // verify that token has correct info.
    printf("token name: %s\n", token->name);
    printf("arguments: ");
    traverse(token->args, (void*)printf);
    printf("\n");
    printf("IO modifier: ");
    if (token->mod == PIPE) {
      printf("|");
    } else if (token->mod == OUTPUT_REDIR) {
      printf(">");
    } else if (token->mod == INPUT_REDIR) {
      printf("<");
    } else {
      printf("no modifier");
    }
    printf("\n");
  }
  

  return token;
}

int evaluate(struct Token *token) {
  // prep values for the execv call. static vars, 
  // so will persist across function invocations.
  static char *cmd = NULL;
  static char **args[token->args->length+1];

  char full_path[strlen(CURR_DIR)+strlen(token->name)];

  //check current dir and path for the command.
  strcpy(full_path, CURR_DIR);
  strcat(full_path, token->name);
  if (!access(full_path, X_OK)) {
    printf("found given command and it is executable\n");
  } else {
    printf("looked at path: %s\n", full_path);
  }

  

  // return 0 if everything went peachy.
  return 0;
}

/*
 * Parses and keeps a list of commands with pointers to their arguments
 * and then carries out the requested actions. Returns 0 on success.
 */
int process_input(char *input) {
  struct SLList tokens;
  init_list(&tokens);

  char *input_current_start = input;
  const char special_delims[] = "<>|";

  // break up tokens into the list and collect argument lists for them.
  do {
    int token_str_length = strcspn(input_current_start, special_delims);
    char *tmp_token_string = (char *)malloc(token_str_length+1);

    if (!tmp_token_string) {
      printf("error: could not allocate memory in input processing for token.");
      return -1;
    }

    strncpy(tmp_token_string, input_current_start, token_str_length);
    // if this is the last token, replace the ending '\n' before adding to list.
    if (tmp_token_string[token_str_length-1] == '\n') {
      tmp_token_string[token_str_length-1] = '\0';
    } else {
      tmp_token_string[token_str_length] = '\0';
    }

    char *token_string = strdup(tmp_token_string);
    char modifier = input_current_start[token_str_length];
    add_back(&tokens, make_token(token_string, modifier));

    input_current_start += token_str_length+1;

    free(tmp_token_string);
  } while (*(input_current_start-2) != '\n');

  // carry out the instructions formed by the tokens.
  traverse(&tokens, (void *)evaluate);

  // any necessary clean up
  //TODO: use a helper function that actually goes through Token structs
  // and frees their memory as well.
  while (!is_empty(&tokens)) {
    pop_front(&tokens);
  }

  return 0;
}

void cmd_exit() {
  exit(0);
}


