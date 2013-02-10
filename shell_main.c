/* 
 * Operating Systems - Homework One
 * Victor Frenkel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*
 * The meat of the shell is separate from the main function 
 * to allow for easy compilation and running of the test suite.
 */
#include "sllist.h"
#include "shell.h"

int main() {
  char *input = NULL;
  size_t len = 0;
  ssize_t input_status;
  
  printf("$ ");

 INPUT_LOOP:
  while ( (input_status = getline(&input, &len, stdin)) != -1) {

    // TODO: call func that checks for special command and executes
    // appropriate subroutine.
    if (strstr(input, "exit")) {
      cmd_exit();
    } else if (strstr(input, "path")) {
      char *mod_ptr = NULL;
      if ( (mod_ptr = strstr(input, "+")) ) {
	cmd_path('+', mod_ptr+2);
      } else if ( (mod_ptr = strstr(input, "-")) ) {
	cmd_path('-', mod_ptr+2);
      } else {
	cmd_path('\0', NULL);
      }
    } else if (input[0] != '\n') {
      if (process_input(input)) {
	input_status = -1;
	break;
      }
    }
    
    printf("$ ");
  }

  // check if something went wrong and let the user try to input again.
  if (input_status == -1) {
    printf("error getting or processing the input line: %s\n", strerror(errno));
    exit(-1);
    goto INPUT_LOOP;
  } else {
    // normal exit condition reached, so let's clean up.
    if (input) {
      free (input);
    }
  }

  return 0;
}
