#ifndef __SHELL_H_
#define __SHELL_H_

/********************
 * GLOBAL VARIABLES *
 ********************/
//static char *PATH = "/bin:/usr/bin";
extern struct SLList PATH;
extern char *CURR_DIR;
//static char *CURR_DIR = "/home/vfrenkel/ACADEMIC/os/hw1/tests/dir_one";

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
static inline void init_globals() {
  CURR_DIR = "/home/vfrenkel/DATA/ACADEMIC/os/hw1/tests/dir_one";

  init_list(&PATH);
  add_back(&PATH, strdup("/bin"));
  add_back(&PATH, strdup("/usr/bin"));
}

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

/*************************
 * SPECIALIZED FUNCTIONS *
 *************************/
/*
 * Traverse the list and remove nodes
 * with matching string data.
 */
void remove_all_str(struct SLList *list, char *data);


void cmd_path(char modifier, char *path);


void cmd_cd(char *path);

#endif // __SHELL_H_
