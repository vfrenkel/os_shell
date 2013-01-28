#include <stdio.h>
#include <stdlib.h>
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

  CuAssertTrue(tc, is_empty(&list));
}

void test_add_back(CuTest *tc) {
  struct SLList list;
  init_list(&list);
  
  add_back(&list, "mary had a ");
  add_back(&list, " wonderful little command-line");
  add_back(&list, " argument, with no shoes \n");

  traverse(&list, (void *)printf);

  printf("reference: mary had a  wonderful little command-line argument, with no shoes \n");
}



CuSuite *collect_sllist_test_suite() {
  CuSuite *suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test_example);
  SUITE_ADD_TEST(suite, test_is_empty);
  SUITE_ADD_TEST(suite, test_add_back);
  return suite;
}


