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
#include <errno.h>

#include "sllist.h"
#include "shell.h"

#define DEBUG 0

/********************
 * GLOBAL VARIABLES *
 ********************/
struct SLList PATH;
int MAX_PATH_SIZE;

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
  
  //extract the necessary data from the command token string.
  char *working_token_string = strdup(token_string);
  char *token_part = strtok(working_token_string, " ");
  char *token_part_dyn = malloc(strlen(token_part)+1);
  strcpy(token_part_dyn, token_part);
  token->name = token_part_dyn;
  do {
    if (DEBUG) printf("adding argument: %s\n", token_part);
    add_back(args, token_part_dyn);
    token_part = strtok(NULL, " ");
    if (token_part) {
      token_part_dyn = malloc(strlen(token_part)+1);
      strcpy(token_part_dyn, token_part);
    }
  } while (token_part);

  free(working_token_string);
  token->args = args;

  //assign correct modifier indicator based on mod character.
  if (mod == '|') {
    token->mod = PIPE;
  } else if (mod == '>') {
    token->mod = OUTPUT_REDIR;
  } else if (mod == 'e') {
    token->mod = ERR_OUTPUT_REDIR;
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
    } else if (token->mod == ERR_OUTPUT_REDIR) {
      printf("2>");
    } else if (token->mod == INPUT_REDIR) {
      printf("<");
    } else {
      printf("no modifier");
    }
    printf("\n");
  }

  return token;
}

int destroy_token(struct Token *tok) {
  // no free(tok->name) because it is already first arg.
  // probably won't need to loop because populate_args pops
  // elements from list, but included here for completeness.
  char *arg;
  while ( (arg = pop_front(tok->args)) ) {
    free(arg);
  }
  free(tok->args);
  free(tok);
  return 0;
}

int destroy_exe_cmd(struct ExecutableCmd *exe) {
  free(exe->full_path);

  char **arg = exe->args;
  while (*arg) {
    free( *arg );
    arg++;
  }

  free(exe->args);
  if (exe->input_redir_from) free(exe->input_redir_from);
  if (exe->output_redir_to) free(exe->output_redir_to);
  if (exe->err_output_redir_to) free(exe->err_output_redir_to);
  free(exe);
  return 0;
}

char *find_cmd(char *name) {
  char full_path_curr[2+strlen(name)];

  // check current dir for the command.
  strcpy(full_path_curr, "./");
  strcat(full_path_curr, name);
  //if (DEBUG) printf("full_path_curr: %s\n", full_path_curr);
  if (!access(full_path_curr, X_OK)) {
    if (DEBUG) printf("found given command and it is executable.\n");
    return strdup(full_path_curr);
  } else { // search the path for the command.
    struct Node *curr_path_node = PATH.head;
    while (curr_path_node) {
      char *search_path = curr_path_node->data;
      char full_path_search[strlen(search_path)+1+strlen(name)];
      
      strcpy(full_path_search, search_path);
      strcat(full_path_search, "/");
      strcat(full_path_search, name);
      
      if (!access(full_path_search, X_OK)) {
	if (DEBUG) printf("found given command and it is executable.\n");
	return strdup(full_path_search);
      }

      curr_path_node = curr_path_node->next;
    }
  }

  return NULL;
}

char *find_file(char *name) {
  char full_path_curr[2+strlen(name)];

  strcpy(full_path_curr, "./");
  strcat(full_path_curr, name);
  if (!access(full_path_curr, F_OK)) {
    return strdup(full_path_curr);
  }

  return NULL;
}

char *make_file_path(char *name) {
  char full_path_curr[2+strlen(name)];

  strcpy(full_path_curr, "./");
  strcat(full_path_curr, name);
  return strdup(full_path_curr);
}

char **populate_args(struct Token *tok) {
  int num_args = tok->args->length;
  char **args_out = (char **)malloc((num_args+1)*sizeof(char *));
  args_out[num_args] = NULL;

  if (DEBUG) printf("number of arguments: %d\n", num_args);
  //pop all args and populate each char * with pointer to popped element.
  for (int i = 0; i < num_args; i++) {
    char *arg = pop_front(tok->args);
    char *arg_dyn = malloc(strlen(arg)+1);
    strcpy(arg_dyn, arg);
    free(arg);
    args_out[i] = arg_dyn;
  }

  return args_out;
}

int _DEBUG_list_exe_cmds(struct SLList *cmds) {
  struct Node *current_cmd_node = cmds->head;
  while (current_cmd_node != NULL) {
    struct ExecutableCmd *current_cmd = current_cmd_node->data;
    printf("command: %s\n", current_cmd->full_path);

    printf("args:\n");
    char **arg = current_cmd->args;
    while (*arg != NULL) {
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

  int num_pipes = cmds->length-1;

  int pfds[2*num_pipes];

  for (int i = 0; i < num_pipes; i++) {
    if (pipe(pfds+2*i) < 0) {
      printf("error: could not open pipe");
      return -1;
    }
  }

  int cmd_count = 0;

  struct Node *current_cmd_node = cmds->head;
  while (current_cmd_node != NULL) {
    struct ExecutableCmd *current_cmd = current_cmd_node->data;
    int in_file = 0;
    int out_file = 0;
    int err_out_file = 0;

    // fork to run the first process.
    pid_t pid = fork();
    int cmd_exit_status;
    
    // prepare child and execute it.
    if (pid == 0) {

      if (cmds->length > 1) {
	if (current_cmd_node == cmds->head) {
	  
	  if (dup2(pfds[1], STDOUT_FILENO) < 0) {
	    printf("error: could not redirect stdio/pipes.\n");
	    exit(-1);
	  }

	  for (int i = 0; i < 2*num_pipes; i++) {
	    close(pfds[i]);
	  }

	} else if (current_cmd_node == cmds->tail_node) {

	  // redirect pipe in to stdin.
	  if (dup2(pfds[2*cmd_count-2], STDIN_FILENO) < 0) {
	    printf("error: could not redirect stdio/pipes.\n");
	    exit(-1);
	  }

	  for (int i = 0; i < 2*num_pipes; i++) {
	    close(pfds[i]);
	  }

	} else {
	  if (dup2(pfds[2*cmd_count+1], STDOUT_FILENO) < 0) {
	    printf("error: could not redirect stdio/pipes.\n");
	    exit(-1);
	  }

	  if (dup2(pfds[2*cmd_count-2], STDIN_FILENO) < 0) {
	    printf("error: could not redirect stdio/pipes.\n");
	    exit(-1);
	  }

	  for (int i = 0; i < 2*num_pipes; i++) {
	    close(pfds[i]);
	  }
	}
      }

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
      for (int i = 0; i < 2*num_pipes; i++) {
	close(pfds[i]);
      }

      for (int i = 0; i < cmds->length; i++) {
	//if (waitpid(pid, &cmd_exit_status, 0) < 0) {
	if ( wait(&cmd_exit_status) < 0) {
	  printf("error: failed to reap all children.\n");
	}
      }

      if (in_file) close(in_file);
      if (out_file) close(out_file);
      if (err_out_file) close(err_out_file);
      
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

    cmd_count++;
    current_cmd_node = current_cmd_node->next;
  }

  return 0;
}

int destroy_cmd_list(struct SLList *cmds) {
  struct ExecutableCmd *curr;
  while ( (curr = pop_front(cmds)) ) {
    destroy_exe_cmd(curr);
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
    } else if (prev_mod == ERR_OUTPUT_REDIR) {
      exe->err_output_redir_to = make_file_path(tok->name);
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

    destroy_token(tok);
  }

  // Execute the commands.
  execute_cmds(&cmds);

  // TODO: destroy the cmds list.
  destroy_cmd_list(&cmds);

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

    char modifier = input_current_start[token_str_length];
    if (input_current_start[token_str_length-1] == '2'
	&& input_current_start[token_str_length] == '>') {
      modifier = 'e';
      tmp_token_string[token_str_length-1] = '\0';
    }

    add_back(&tokens, make_token(tmp_token_string, modifier));

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


void remove_all_str(struct SLList *list, char *data) {
  struct Node *curr = list->head;
  struct Node *kill_ptr = NULL;
  struct Node *prev_node = NULL;
  while (curr) {
    if (strcmp(curr->data, data) == 0) {
      free(curr->data);
      kill_ptr = curr;

      if (prev_node == NULL) {
	list->head = curr->next;
      } else if (curr == list->tail_node) {
	prev_node->next = NULL;
	list->tail_node = prev_node;
      } else {
	prev_node->next = curr->next;
      }

      list->length--;
    } else {
      prev_node = curr;
    }

    curr = curr->next;

    if (kill_ptr) {
      free(kill_ptr);
      kill_ptr = NULL;
    }
  }
}


void cmd_path(char modifier, char *path) {
  
  if (path) {
    path[strlen(path)-1] = '\0';
    if (path[strlen(path)-1] == '/') {
      path[strlen(path)-1] = '\0';
    }
  }

  if (modifier == '+') {
    add_back(&PATH, strdup(path));
  } else if (modifier == '-') {
    //remove all matches with path from list.
    remove_all_str(&PATH, path);
  } else {
    struct Node *curr = PATH.head;
    while (curr) {
      printf(curr->data);

      if (curr != PATH.tail_node) {
	printf(":");
      }

      curr = curr->next;
    }
    printf("\n");
  }
}

void cmd_cd(char *path) {

  if (path) {
    path[strlen(path)-1] = '\0';
    if (path[strlen(path)-1] == '/') {
      path[strlen(path)-1] = '\0';
    }
  }

  if (chdir(path) < 0) {
    fprintf(stderr, "error: %s\n", strerror(errno));
  }  

}
