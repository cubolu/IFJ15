#include <stdlib.h>
#include <check.h>

#include "test.h"

START_TEST(test_list)
{
    char msg_pop[256] = "List pop returned bad item";
    list_t* test_list = _list_init(false);
    list_insert(test_list, 1, 10);
    list_insert(test_list, 2, 20);
    list_insert(test_list, 3, 30);
    list_insert(test_list, 4, 40);
    ck_assert_msg(list_pop(test_list, 1) == (void*)10, msg_pop);
    ck_assert_msg(list_pop(test_list, 2) == (void*)20, msg_pop);
    ck_assert_msg(list_pop(test_list, 3) == (void*)30, msg_pop);
    ck_assert_msg(list_pop(test_list, 4) == (void*)40, msg_pop);
    _list_free(test_list);
}
END_TEST

START_TEST(test_memory)
{
    ifj15_memory_init();
    list_t* test_list = list_init();
    ptable_t* test_ptable = ptable_init();
    htable_t* test_htable = htable_init();
    ifj15_free(test_list);
    ifj15_free(test_ptable);
    ifj15_free(test_htable);
    ifj15_free_all();
}
END_TEST

START_TEST(test_htable)
{
    char msg_pop[256] = "Htable pop returned bad item";
    ifj15_memory_init();
    htable_t* test_htable = htable_init();
    htable_insert(test_htable, "1", 10);
    htable_insert(test_htable, "2", 20);
    htable_insert(test_htable, "3", 30);
    htable_insert(test_htable, "4", 40);
    ck_assert_msg(htable_pop(test_htable, "1") == (void*)10, msg_pop);
    ck_assert_msg(htable_pop(test_htable, "2") == (void*)20, msg_pop);
    ck_assert_msg(htable_pop(test_htable, "3") == (void*)30, msg_pop);
    ck_assert_msg(htable_pop(test_htable, "4") == (void*)40, msg_pop);
    ifj15_free_all();
}
END_TEST

START_TEST(test_ptable)
{
    char msg_pop[256] = "Htable pop returned bad item";
    ifj15_memory_init();
    ptable_t* test_ptable = ptable_init();
    ptable_insert(test_ptable, 1, 10);
    ptable_insert(test_ptable, 2, 20);
    ptable_insert(test_ptable, 3, 30);
    ptable_insert(test_ptable, 4, 40);
    ck_assert_msg(ptable_pop(test_ptable, 1) == 10, msg_pop);
    ck_assert_msg(ptable_pop(test_ptable, 2) == 20, msg_pop);
    ck_assert_msg(ptable_pop(test_ptable, 3) == 30, msg_pop);
    ck_assert_msg(ptable_pop(test_ptable, 4) == 40, msg_pop);
    ifj15_free_all();
}
END_TEST

Suite* core_suite(void)
{
    Suite* s;
    TCase *tc_core;

    s = suite_create("Core Test Suite");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_list);
    tcase_add_test(tc_core, test_memory);
    tcase_add_test(tc_core, test_htable);
    tcase_add_test(tc_core, test_ptable);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = core_suite();
    sr = srunner_create(s);

    // Uncomment when debugging segfaults (for DBG)
    //srunner_set_fork_status(sr, CK_NOFORK);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
