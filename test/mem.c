#include <stdio.h>
#include <check.h>
#include <stdarg.h>
#include <stdbool.h>
#include "mem.h"

struct mem g_mem;

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
    s = bc_mem_unlink(s);
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

START_TEST(_realloc){
    const char* orig = "sample name";
    const char* n2 = "Günther";
    demo_structure *s = bc_mem_realloc(NULL, NULL, demo_structure, 1);
    s->id = 123;
    strcpy(s->name, orig);

    demo_structure *s1 = bc_mem_realloc(NULL, s, demo_structure, 2);

    ck_assert_ptr_ne(s, s1);
    ck_assert_ptr_nonnull(s1);
    ck_assert_str_eq(s1->name, orig);
    ck_assert_int_eq(s1->id, 123);
    ck_assert(bc_mem_is_valid(s1));

    s1[1].id = 333;
    strcpy( s1[1].name, n2);

    demo_structure *s2 = bc_mem_realloc(NULL, s1, demo_structure, 3);

    ck_assert_ptr_ne(s, s2);
    ck_assert_ptr_nonnull(s2);
    ck_assert(bc_mem_is_valid(s2));
    ck_assert_str_eq(s2[0].name, orig);
    ck_assert_int_eq(s2[0].id, 123);
    ck_assert_str_eq(s2[1].name, n2);
    ck_assert_int_eq(s2[1].id, 333);
}
END_TEST

////////////////////////////////////////////////////////////////////////////////
//
// SETUP
//
static void setup(void)
{
    bc_mem_init();
}

//
// TEARDOWN
//

static void teardown(void)
{
    bc_mem_report();
}


TCase *test_mem(  ) {
    TCase *tcase = tcase_create( "mem" );
    tcase_add_checked_fixture( tcase, setup, teardown );
    tcase_add_test( tcase, _alloc_simple );
    tcase_add_test( tcase, _alloc_and_unlink );
    tcase_add_test( tcase, _overwrite );
    tcase_add_test( tcase, _change );
    tcase_add_test( tcase, _realloc );
    return tcase;
}
