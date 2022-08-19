#include <stdio.h>
#include <check.h>
#include <stdarg.h>
#include <stdbool.h>
#include "mem.h"


//  Suite
Suite *test_suite( void ) {
    extern TCase *test_mem(void);
    extern TCase *test_itab(void);
    Suite *s;
    s = suite_create( "Memory" );
    suite_add_tcase( s, test_mem(  ) );
    suite_add_tcase( s, test_itab(  ) );
    return s;
}


int main( void ) {
    int number_failed;
    SRunner *sr;

    sr = srunner_create( test_suite(  ) );
    srunner_set_log( sr, "test.log" );
    srunner_run_all( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed( sr );
    srunner_free( sr );
    return ( number_failed == 0 ) ? 0 : 1;
}
