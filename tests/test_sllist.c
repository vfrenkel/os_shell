#include <stdio.h>
#include "CuTest.h"

#include "../sllist.h"

void test_example(CuTest *tc) {
  char *expected = "hello world";
  char *actual = "hello world";
  CuAssertStrEquals(tc, expected, actual);
}

void test_is_empty(CuTest *tc) {
  struct SLList list;
  init_list(&list);
  int sample_data = 42;
  add_back(&list, &sample_data);

  CuAssertTrue(tc, is_empty(&list));
}


CuSuite *collect_sllist_test_suite() {
  CuSuite *suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test_example);
  SUITE_ADD_TEST(suite, test_is_empty);
  return suite;
}


