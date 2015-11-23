#include <stdlib.h>
#include <check.h>

#include "test.h"

/*
START_TEST(test_ulist)
{
    char msg_pop[256] = "List pop returned bad item";
    char msg_get[256] = "List get returned bad item";
    ulist_t* test_ulist = _ulist_init(false);
    ulist_set(test_ulist, 1, 10);
    ulist_set(test_ulist, 2, 20);
    ulist_set(test_ulist, 3, 30);
    ulist_set(test_ulist, 4, 40);
    ulist_set(test_ulist, 1, 50);
    ck_assert_msg(ulist_get(test_ulist, 1) == (void*)50, msg_get);
    ck_assert_msg(ulist_get(test_ulist, 2) == (void*)20, msg_get);
    ck_assert_msg(ulist_get(test_ulist, 3) == (void*)30, msg_get);
    ck_assert_msg(ulist_get(test_ulist, 4) == (void*)40, msg_get);
    ck_assert_msg(ulist_pop(test_ulist, 1) == (void*)50, msg_pop);
    ck_assert_msg(ulist_pop(test_ulist, 2) == (void*)20, msg_pop);
    ck_assert_msg(ulist_pop(test_ulist, 3) == (void*)30, msg_pop);
    ck_assert_msg(ulist_pop(test_ulist, 4) == (void*)40, msg_pop);
    _ulist_free(test_ulist);
}
END_TEST
*/

symbol_t create_sym(str_t* name) {
    symbol_t res;
    res.name = name;
    return res;
}

START_TEST(test_memory)
{
    ifj15_memory_init();
    ptable_t* test_ptable = ptable_init();
    htable_t* test_htable = htable_init();
    vector_t* test_vector = vector_init();
    ifj15_free(test_ptable);
    ifj15_free(test_htable);
    ifj15_free(test_vector);
    ifj15_free_all();
}
END_TEST

START_TEST(test_vector)
{
    char msg_at[256] = "Vector at returned bad item";
    ifj15_memory_init();
    vector_t* test_vector = vector_init();
    for (int i = 0; i < 1000; ++i) {
        vector_push_front(test_vector, i);
    }
    for(int i = 0; i < 1000; ++i) {
        vector_push_back(test_vector, i);
    }
    for (int i = 999; i >=0; --i) {
        ck_assert_int_eq(vector_at(test_vector, 999-i), i);
    }
    for (int i = 1000; i <2000; ++i) {
        ck_assert_int_eq(vector_at(test_vector, i), i-1000);
    }
    for (int i = 0; i < 1000; ++i) {
        ck_assert_int_eq(vector_pop_front(test_vector), 999-i);
    }
    ifj15_free_all();
}
END_TEST

START_TEST(test_htable)
{
    char msg_pop[256] = "Htable pop returned bad item";
    char msg_get[256] = "Htable get returned bad item";
    ifj15_memory_init();

    str_t* str1 = str_init();
    str_t* str2 = str_init();
    str_t* str3 = str_init();
    str_t* str4 = str_init();
    str_copy(str1, "str1");
    str_copy(str2, "str2");
    str_copy(str3, "str3");
    str_copy(str4, "str4");
    symbol_t sym1 = create_sym(str1);
    symbol_t sym2 = create_sym(str2);
    symbol_t sym3 = create_sym(str3);
    symbol_t sym4 = create_sym(str4);

    htable_t* test_htable = htable_init();
    htable_set(test_htable, str1, sym1);
    htable_set(test_htable, str2, sym2);
    htable_set(test_htable, str3, sym3);
    htable_set(test_htable, str4, sym4);

    symbol_t* cmpsym1 = htable_get(test_htable, str1);
    symbol_t* cmpsym2 = htable_get(test_htable, str2);
    symbol_t* cmpsym3 = htable_get(test_htable, str3);
    symbol_t* cmpsym4 = htable_get(test_htable, str4);
    ck_assert_msg(str_equal(str1, cmpsym1->name), msg_get);
    ck_assert_msg(str_equal(str2, cmpsym2->name), msg_get);
    ck_assert_msg(str_equal(str3, cmpsym3->name), msg_get);
    ck_assert_msg(str_equal(str4, cmpsym4->name), msg_get);
    symbol_t cmpsym5 = htable_pop(test_htable, str1);
    symbol_t cmpsym6 = htable_pop(test_htable, str2);
    symbol_t cmpsym7 = htable_pop(test_htable, str3);
    symbol_t cmpsym8 = htable_pop(test_htable, str4);
    ck_assert_msg(str_equal(str1, cmpsym5.name), msg_pop);
    ck_assert_msg(str_equal(str2, cmpsym6.name), msg_pop);
    ck_assert_msg(str_equal(str3, cmpsym7.name), msg_pop);
    ck_assert_msg(str_equal(str4, cmpsym8.name), msg_pop);
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

START_TEST(test_str)
{
    char msg_str_equal[256] = "Str equals returned bad result";
    char msg_str_to_double[256] = "Str to double returned bad result";
    ifj15_memory_init();
    str_t * s = str_init();
    str_append_char(s, 'a');
    str_append_char(s, 'b');
    str_append_char(s, 'c');
    ck_assert_msg(str_equal(s, "abc"), msg_str_equal);
    str_clear(s);

    str_append_char(s, '3');
    str_append_char(s, '.');
    str_append_char(s, '1');
    str_append_char(s, '4');
    ck_assert_msg(str_to_double(s) - 3.14 < 0.001, msg_str_to_double);
    str_clear(s);
    ifj15_free_all();
}
END_TEST

bool test_cmp_true(char c){ return true; }
bool test_cmp_false(char c){ return false; }
bool test_cmp(char c){ return c < 'm'; }

START_TEST(test_stack)
{

    char msg_pop[256] = "Stack pop returned bad item";
    char msg_get[256] = "Stack get returned bad item";
    char msg_fnd[256] = "Stack find returned bad item";
    char msg_ins[256] = "Stack insert returned bad item";
    ifj15_memory_init();
    stack_char_t* test_stack = stack_init(SI_CHAR);
    stack_push(test_stack, 'a');
    ck_assert_msg(stack_pop(test_stack) == 'a', msg_get);

    for (char c = 'a'; c <= 'z'; ++c)
        stack_push(test_stack, c);
    ck_assert_msg(stack_top(test_stack) == 'z', msg_get);
    for (char c = 'z'; c >= 'a'; --c)
        ck_assert_int_eq(stack_pop(test_stack), c);
    //ck_assert_msg(stack_top(test_stack) == -1, msg_get);

    for (char c = 'a'; c <= 'z'; ++c)
        stack_push(test_stack, c);
    ck_assert_msg(stack_find(test_stack, test_cmp_false) == -1, msg_fnd);
    ck_assert_msg(stack_find(test_stack, test_cmp_true) == stack_top(test_stack), msg_fnd);
    ck_assert_msg(stack_find(test_stack, test_cmp) == 'l', msg_fnd);

    stack_insert_after(test_stack, 'z', test_cmp_true);
    ck_assert_msg(stack_pop(test_stack) == 'z', msg_pop);
    ck_assert_msg(stack_top(test_stack) == 'z', msg_get);
    stack_insert_after(test_stack, 'a', test_cmp_false);
    stack_insert_after(test_stack, 'a', test_cmp);
    for (char c = 'z'; c >= 'm'; --c)
        ck_assert_int_eq(stack_pop(test_stack), c);
    ck_assert_msg(stack_pop(test_stack) == 'a', msg_pop);
    for (char c = 'l'; c >= 'a'; --c)
        ck_assert_int_eq(stack_pop(test_stack), c);
    //ck_assert_msg(stack_pop(test_stack) == -1, msg_pop);

    ifj15_free_all();
}
END_TEST

Suite* core_suite(void)
{
    Suite* s;
    TCase *tc_core;

    s = suite_create("Core Test Suite");

    tc_core = tcase_create("Core");

    //tcase_add_test(tc_core, test_ulist);
    tcase_add_test(tc_core, test_memory);
    tcase_add_test(tc_core, test_htable);
    tcase_add_test(tc_core, test_ptable);
    tcase_add_test(tc_core, test_vector);
    tcase_add_test(tc_core, test_str);
    tcase_add_test(tc_core, test_stack);
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
    srunner_set_fork_status(sr, CK_NOFORK);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
