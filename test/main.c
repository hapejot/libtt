#include <check.h>





START_TEST(_cnt_colname ){
    char* name = "name";
    ck_assert_str_eq(name, "name");
}
END_TEST

////////////////////////////////////////////////////////////////////////////////
//
// SETUP
//
void setup(  )
{
}

//
// TEARDOWN
//
void teardown(  )
{
}


TCase *test_cnt_page(  ) {
    TCase *tcase = tcase_create( "dummy" );
    tcase_add_checked_fixture( tcase, setup, teardown );
    tcase_add_test( tcase, _cnt_colname );
    return tcase;
}

//  Suite
Suite *test_suite( void ) {
    Suite *s;
    s = suite_create( "Values" );
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
