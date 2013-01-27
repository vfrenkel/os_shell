#include "CuTest.h"
#include <stdio.h>


CuSuite *collect_sllist_test_suite();

void run_all_tests(void) {
  CuString *output = CuStringNew();
  CuSuite *suite = CuSuiteNew();

  CuSuiteAddSuite(suite, collect_sllist_test_suite());

  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
}

int main(void) {
  run_all_tests();
  return 0;
}
