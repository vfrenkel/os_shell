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

/********************
 * GLOBAL VARIABLES *
 ********************/
static char *PATH = "/bin:/usr/bin";
static char *CURR_DIR = "/home/vfrenkel/DATA/os/hw1/tests/dir_one";

/************************
 * FUNCTION DEFINITIONS *
 ************************/
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
    token->mod = NO_MODIFIER;
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

int find_cmd(char *name) {
  char full_path_curr[strlen(CURR_DIR)+1+strlen(name)];

  // check current dir for the command.
  strcpy(full_path_curr, CURR_DIR);
  strcat(full_path_curr, "/");
  strcat(full_path_curr, name);
  //if (DEBUG) printf("full_path_curr: %s\n", full_path_curr);
  if (!access(full_path_curr, X_OK)) {
    if (DEBUG) printf("found given command and it is executable.\n");
    return 1;
  } else { // search the path for the command.
    char full_path_search[strlen(PATH)+1+strlen(name)];
    char *delim = ":";
    char *path_dup = strdup(PATH);
    char *path_chunk = strtok(path_dup, delim);
    while (path_chunk != NULL) {
      strcpy(full_path_search, path_chunk);
      strcat(full_path_search, "/");
      strcat(full_path_search, name);
      //if (DEBUG) printf("full_path_search: %s\n", full_path_search);
      if (!access(full_path_search, X_OK)) {
	if (DEBUG) printf("found given command and it is executable.\n");
	return 1;
      }
      path_chunk = strtok(NULL, delim);
    }
  }

  return 0;
}

char **populate_args(struct Token *tok) {
  char **args_out = (char **)malloc((tok->args->length+1)*sizeof(char *));
  
  //pop all args and populate each char * with pointer to popped element.
  return args_out;
}

int evaluate(struct SLList *tokens) {
  char *cmd = NULL;
  char **args = NULL;

  printf("tokens length: %d\n", tokens->length);
  while (tokens->length != 0) {
    struct Token *tok = pop_front(tokens);
    
    if (find_cmd(tok->name)) {
      if (args) {
	free(args);
      }
      cmd = tok->name;
      args = populate_args(tok);
    }
    
    

    free(tok);
  }

  if (args) {
    free(args);
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
  //traverse(&tokens, (void *)evaluate);
  evaluate(&tokens);

  // any necessary clean up
  //TODO: use a helper function that actually goes through Token structs
  // and frees their memory as well.
  //while (!is_empty(&tokens)) {
  //  pop_front(&tokens);
  //}

  return 0;
}

void cmd_exit() {
  exit(0);
}


