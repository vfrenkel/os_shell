#ifndef __SHELL_H_
#define __SHELL_H_

/************************
 * DATA STRUCTS & ENUMS *
 ************************/
typedef enum CMD_IO_MODIFIER {
  NO_MODIFIER,
  INPUT_REDIR,
  OUTPUT_REDIR,
  ERR_OUTPUT_REDIR,
  PIPE
} IOModifier;

/*
 * Describes a single command or file,
 * along with any arguments (if a command).
 */
struct Token {
  char *name;
  struct SLList *args;
  IOModifier mod;
};

struct ExecutableCmd {
  char *full_path;
  char **args;

  char *input_redir_from;
  char *output_redir_to;
  char *err_output_redir_to;
};

/*************************
 * FUNCTION DECLARATIONS *
 *************************/
/*
 * Initializes a token.
 */
static inline void init_token(struct Token *token) {
  token->name = NULL;
  token->args = NULL;
  token->mod = NO_MODIFIER;
}

/*
 * Initializes an ExecutableCmd.
 */
static inline void init_executable_cmd(struct ExecutableCmd *exe_cmd) {
  exe_cmd->full_path = NULL;
  exe_cmd->args = NULL;

  exe_cmd->input_redir_from = NULL;
  exe_cmd->output_redir_to = NULL;
  exe_cmd->err_output_redir_to = NULL;
}

/*
 * Prepares non-built-in commands for
 * evaluation.
 */
int process_input(char *input);

/*
 * Uses the parsed tokens to evaluate the 
 * user input to the shell.
 */
int evaluate(struct SLList *tokens);

/*
 * The built in exit command action.
 */
void cmd_exit();



#endif // __SHELL_H_
