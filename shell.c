/* 
 * Operating Systems - Homework One
 * Victor Frenkel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "sllist.h"
#include "shell.h"

#define DEBUG 0

/********************
 * GLOBAL VARIABLES *
 ********************/
static char *PATH = "/bin:/usr/bin";
//static char *CURR_DIR = "/home/vfrenkel/DATA/ACADEMIC/os/hw1/tests/dir_one";
static char *CURR_DIR = "/home/vfrenkel/ACADEMIC/os/hw1/tests/dir_one";

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
  do {
    if (DEBUG) printf("adding argument: %s\n", token_part);
    add_back(args, token_part);
    token_part = strtok(NULL, " ");
  } while (token_part != NULL);

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

//TODO clean up malloced stuff right before returning a result.
char *find_cmd(char *name) {
  char full_path_curr[strlen(CURR_DIR)+1+strlen(name)];

  // check current dir for the command.
  strcpy(full_path_curr, CURR_DIR);
  strcat(full_path_curr, "/");
  strcat(full_path_curr, name);
  //if (DEBUG) printf("full_path_curr: %s\n", full_path_curr);
  if (!access(full_path_curr, X_OK)) {
    if (DEBUG) printf("found given command and it is executable.\n");
    return strdup(full_path_curr);
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
	return strdup(full_path_search);
      }
      path_chunk = strtok(NULL, delim);
    }
  }

  return NULL;
}

char *find_file(char *name) {
  char full_path_curr[strlen(CURR_DIR)+1+strlen(name)];

  strcpy(full_path_curr, CURR_DIR);
  strcat(full_path_curr, "/");
  strcat(full_path_curr, name);
  if (!access(full_path_curr, F_OK)) {
    return strdup(full_path_curr);
  }

  return NULL;
}

char *make_file_path(char *name) {
  char full_path_curr[strlen(CURR_DIR)+1+strlen(name)];

  strcpy(full_path_curr, CURR_DIR);
  strcat(full_path_curr, "/");
  strcat(full_path_curr, name);
  return strdup(full_path_curr);
}

char **populate_args(struct Token *tok) {
  int num_args = tok->args->length;
  char **args_out = (char **)malloc((num_args+1)*sizeof(char *));
  args_out[num_args] = '\0';

  if (DEBUG) printf("number of arguments: %d\n", num_args);
  //pop all args and populate each char * with pointer to popped element.
  for (int i = 0; i < num_args; i++) {
    args_out[i] = pop_front(tok->args);
  }

  return args_out;
}

// deprecated: consider removal.
int execute_single_cmd(char *cmd, char **args) {
  pid_t pid = fork();
  int cmd_exit_status;
  
  if (pid == 0) {
    if (execv(cmd, args) < 0) {
      printf("error: could not execute the command.");
      // child is useless now, kill it.
      exit(-1);
    }
  } else if (pid < 0) {
    printf("error: could not fork.");
    return -1;
  } else {
    if (waitpid(pid, &cmd_exit_status, 0) < 0) {
      printf("error: failed to wait for child.");
    }
  }

  return 0;
}

int _DEBUG_list_exe_cmds(struct SLList *cmds) {
  struct Node *current_cmd_node = cmds->head;
  while (current_cmd_node != NULL) {
    struct ExecutableCmd *current_cmd = current_cmd_node->data;
    printf("command: %s\n", current_cmd->full_path);

    printf("args:\n");
    char **arg = current_cmd->args;
    while (*arg != '\0') {
      printf("\t %s\n", *arg);
      arg++;
    }

    printf("input_redir_from: %s\n", current_cmd->input_redir_from);
    printf("output_redir_to: %s\n", current_cmd->output_redir_to);
    printf("err_output_redir_to: %s\n", current_cmd->err_output_redir_to);

    current_cmd_node = current_cmd_node->next;
  }

  return 0;
}

/* int execute_command(struct ExecutableCmd *exe) { */
/*   pid_t pid = fork(); */
/*   int cmd_exit_status; */

/*   if (pid == 0) { // child (for cmd) code. */
/*     if (execv(exe->full_path, exe->args) < 0) { */
/*       printf("error: could not execute the command.\n"); */
/*       // child is useless now, have it kill itself. */
/*       exit(-1); */
/*     } */
/*   } else if (pid < 0) { */
/*     printf("error: could not fork.\n"); */
/*     return -1; */
/*   } else { // parent (for shell) code. */
    
/*   } */

/*   return 0; */
/* } */

int execute_cmds(struct SLList *cmds) {
  int stdout_fd = dup(STDOUT_FILENO);
  int stdin_fd = dup(STDIN_FILENO);
  int stderr_fd = dup(STDERR_FILENO);

  // TODO: make array that is size (cmds->length-1)*2
  int pfds[2];

  if (pipe(pfds) < 0) {
    printf("error: could not open pipe");
    return -1;
  }

  struct Node *current_cmd_node = cmds->head;
  while (current_cmd_node != NULL) {
    struct ExecutableCmd *current_cmd = current_cmd_node->data;
    int in_file = 0;
    int out_file = 0;
    int err_out_file = 0;

    current_cmd->pipe_in_fd = pfds[1];
    current_cmd->pipe_out_fd = pfds[0];

    // fork to run the first process.
    pid_t pid = fork();
    int cmd_exit_status;
    
    // child has been prepared, time to execute it.
    if (pid == 0) {
      // make aliases with dup2 and kick off the command.
      if (cmds->length > 1) {
	if (current_cmd_node == cmds->head) {
	  
	  if (dup2(pfds[1], STDOUT_FILENO) < 0) {
	    printf("error: could not redirect stdio/pipes.\n");
	    exit(-1);
	  }

	  close(pfds[0]);
	  close(pfds[1]);

	} else if (current_cmd_node == cmds->tail_node) {

	  // redirect pipe in to stdin.
	  if (dup2(pfds[0], STDIN_FILENO) < 0) {
	    printf("error: could not redirect stdio/pipes.\n");
	    exit(-1);
	  }

	  close(pfds[0]);
	  close(pfds[1]);

	} else {
	  /* if (dup2(current_cmd->pipe_in_fd, last_pipe_out) < 0) { */
	  /*   printf("error: could not redirect stdio/pips.\n"); */
	  /*   exit(-1); */
	  /* } */

	  /* if (dup2(current_cmd->pipe_out_fd, STDOUT_FILENO) < 0) { */
	  /*   printf("error: could not redirect stdio/pips.\n"); */
	  /*   exit(-1); */
	  /* } */

	  //last_pipe_out = current_cmd->pipe_out_fd;
	}
      }


      //TODO: maybe it will help if you encapsulate this in a function...
      // perform any necessary IO redirections to/from files.
      if (current_cmd->input_redir_from) {
	in_file = open(current_cmd->input_redir_from, O_RDONLY);

	if ( dup2(in_file, STDIN_FILENO) < 0 ) {
	  printf("error: could not redirect stdin to the output file.\n");
	  exit(-1);
	}
      }

      if (current_cmd->output_redir_to) {
	out_file = open(current_cmd->output_redir_to,
			O_TRUNC | O_WRONLY | O_CREAT,
			S_IRUSR | S_IWUSR |
			S_IRGRP | S_IWGRP |
			S_IROTH | S_IWOTH);

	if ( dup2(out_file, STDOUT_FILENO) < 0 ) {
	  printf("error: could not redirect stdout to the output file.\n");
	  exit(-1);
	}
      }

      if (current_cmd->err_output_redir_to) {
	err_out_file = open(current_cmd->err_output_redir_to,
			    O_TRUNC | O_WRONLY | O_CREAT,
			    S_IRUSR | S_IWUSR |
			    S_IRGRP | S_IWGRP |
			    S_IROTH | S_IWOTH);

	if ( dup2(err_out_file, STDERR_FILENO) < 0 ) {
	  printf("error: could not redirect stderr to the output file.\n");
	  exit(-1);
	}
      }

      // execute the command.
      if (execv(current_cmd->full_path, current_cmd->args) < 0) {
	printf("error: could not execute the command.\n");
	// child is useless now, have it kill itself.
	exit(-1);
      }
    } else if (pid < 0) {
      printf("error: could not fork.\n");
      return -1;
    }

    // last command reached.
    if (current_cmd_node == cmds->tail_node) {
      close(pfds[0]);
      close(pfds[1]);

      // TODO: change this to loop through number of children and call that many waits.
      //wait for last pid in the chain to finish.
      if (waitpid(pid, &cmd_exit_status, 0) < 0) {
	printf("error: failed to reap all children.\n");
      }

      if (!in_file) close(in_file);
      if (!out_file) close(out_file);
      if (!err_out_file) close(err_out_file);
      
      // put the std fds back.
      if (dup2(stdin_fd, STDIN_FILENO) < 0) {
	printf("error: could not redirect stdout back to fd 0.\n");
      }

      if (dup2(stdout_fd, STDOUT_FILENO) < 0 ) {
      	printf("error: could not redirect stdout back to fd 1.\n");
      }

      if (dup2(stderr_fd, STDERR_FILENO) < 0) {
	printf("error: could not redirect stdout back to fd 2.\n");
      }
      
    }

    current_cmd_node = current_cmd_node->next;
  }

  return 0;
}

int evaluate(struct SLList *tokens) {
  struct SLList cmds;
  init_list(&cmds);
  
  IOModifier prev_mod = NO_MODIFIER;

  struct ExecutableCmd *exe = (struct ExecutableCmd *)malloc(sizeof(struct ExecutableCmd));
  init_executable_cmd(exe);

  while (tokens->length != 0) {
    struct Token *tok = pop_front(tokens);
    
    if ( prev_mod == PIPE ) {
      exe->has_pipe = 1;
      add_back(&cmds, exe);
      exe = (struct ExecutableCmd *)malloc(sizeof(struct ExecutableCmd));
      init_executable_cmd(exe);
      prev_mod = NO_MODIFIER;
    }

    // use previous token's mod, if mod exists,
    // and this token's info to populate redirection of exe.
    if (prev_mod == INPUT_REDIR) {
      exe->input_redir_from = make_file_path(tok->name);
    } else if (prev_mod == OUTPUT_REDIR) {
      exe->output_redir_to = make_file_path(tok->name);
    }

    if ( prev_mod == NO_MODIFIER ) {
      if ( (exe->full_path = find_cmd(tok->name)) != NULL ) {
	exe->args = populate_args(tok);
	if (tok->mod == NO_MODIFIER) {
	  add_back(&cmds, exe);
	}
      } else {
	printf("error: not a valid command.\n");
	return -1;
      }
    } else if (tok->mod == NO_MODIFIER) {
      add_back(&cmds, exe);
    }

    prev_mod = tok->mod;

    free(tok);
  }

  // Execute the commands.
  execute_cmds(&cmds);

  // TODO: destroy the cmds list.


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
      printf("error: could not allocate memory in input processing for token.\n");
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

  // TODO: add any necessary clean up.
  

  return 0;
}

void cmd_exit() {
  exit(0);
}


