/* 
 * Operating Systems - Homework One
 * Victor Frenkel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    if (strcmp(input, "exit\n") == 0) {
      cmd_exit();
    } else if (input[0] != '\n') {
      process_input(input);
    }

    printf("$ ");
  }

  // check if something went wrong and let the user try to input again.
  if (input_status == -1) {
    printf("error: could not get line");
    goto INPUT_LOOP;
  } else {
    // normal exit condition reached, so let's clean up.
    if (input) {
      free (input);
    }
  }

  return 0;
}
