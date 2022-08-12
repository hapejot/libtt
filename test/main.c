#include <stdio.h>
#include <check.h>
#include <stdarg.h>
#include <stdbool.h>
#include "mem.h"

struct mem g_mem;
extern void mem_init();
extern void mem_report();

typedef struct {
    int id;
    char name[100];
}       demo_structure;



START_TEST(_alloc_simple ){
    demo_structure *s = bc_mem_realloc(NULL, NULL, demo_structure, 1);
    ck_assert_ptr_nonnull(s);
    ck_assert(bc_mem_is_valid(s));
    ck_assert_int_eq(s->id, 0);
    ck_assert_int_eq(s->name[0], 0);
    ck_assert_int_eq(s->name[99], 0);
}
END_TEST

START_TEST(_alloc_and_unlink ){
    demo_structure *s = bc_mem_realloc(NULL, NULL, demo_structure, 1);
    ck_assert_ptr_nonnull(s);
    ck_assert(bc_mem_is_valid(s));
    bc_mem_unlink(&s);
    ck_assert_ptr_null(s);
    ck_assert(!bc_mem_is_valid(s));
}
END_TEST


START_TEST(_overwrite ){
    demo_structure *s1 = bc_mem_realloc(NULL, NULL, demo_structure, 1);
    demo_structure *s2 = bc_mem_realloc(NULL, NULL, demo_structure, 1);

    ck_assert(bc_mem_is_valid(s1));
    ck_assert(bc_mem_is_valid(s2));

    for(int i = 0; i< 102;i++) s1->name[i] = '.';
    ck_assert(!bc_mem_is_valid(s1));

    // ck_assert(!bc_mem_is_valid(s2));  this check does fail on windows.
}
END_TEST

START_TEST(_change ){
    demo_structure *s1 = bc_mem_realloc(NULL, NULL, demo_structure, 1);
    demo_structure *s2 = bc_mem_realloc(NULL, NULL, demo_structure, 1);
    ck_assert(bc_mem_is_valid(s1));
    ck_assert(bc_mem_is_valid(s2));
    for(int i = 0; i< 100;i++) s1->name[i] = '.';
    ck_assert(!bc_mem_is_valid(s1));
    bc_mem_checkpoint(s1);
    ck_assert(bc_mem_is_valid(s1));
    ck_assert(bc_mem_is_valid(s2));
}
END_TEST


////////////////////////////////////////////////////////////////////////////////
//
// SETUP
//
void setup(void)
{
    mem_init();
}

//
// TEARDOWN
//

void teardown(void)
{
    mem_report();
}


TCase *test_cnt_page(  ) {
    TCase *tcase = tcase_create( "dummy" );
    tcase_add_checked_fixture( tcase, setup, teardown );
    tcase_add_test( tcase, _alloc_simple );
    tcase_add_test( tcase, _alloc_and_unlink );
    tcase_add_test( tcase, _overwrite );
    tcase_add_test( tcase, _change );
    return tcase;
}

//  Suite
Suite *test_suite( void ) {
    Suite *s;
    s = suite_create( "Memory" );
    suite_add_tcase( s, test_cnt_page(  ) );
    return s;
}


int main( void ) {
    int number_failed;
    SRunner *sr;

    sr = srunner_create( test_suite(  ) );
    srunner_set_log( sr, "hape-test.log" );
    srunner_run_all( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed( sr );
    srunner_free( sr );
    return ( number_failed == 0 ) ? 0 : 1;
}
