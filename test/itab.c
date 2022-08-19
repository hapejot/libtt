#include <check.h>
#include <stdio.h>
#include "itab.h"
#include "mem.h"

START_TEST(_demo ){
    t_itab itab = itab_new();
    ck_assert_ptr_nonnull(itab);

    itab_insert(itab, "TKL", "Tokelau" );
    itab_insert(itab, "SMR", "San Marino" );
    itab_insert(itab, "SWE", "Sweden" );
    itab_insert(itab, "SLV", "El Salvador" );
    itab_insert(itab, "THA", "Thailand" );
    itab_insert(itab, "TGO", "Togo" );
    itab_insert(itab, "SVN", "Slovenia" );
    itab_insert(itab, "SOM", "Somalia" );
    itab_insert(itab, "SPM", "Saint Pierre and Miquelon" );
    itab_insert(itab, "TCD", "Chad" );
    itab_insert(itab, "SWZ", "Eswatini" );
    itab_insert(itab, "TON", "Tonga" );
    itab_insert(itab, "SLE", "Sierra Leone" );
    itab_insert(itab, "SSD", "South Sudan" );
    itab_insert(itab, "SXM", "Sint Maarten (Dutch part)" );
    itab_insert(itab, "STP", "Sao Tome and Principe" );
    itab_insert(itab, "TKM", "Turkmenistan" );
    itab_insert(itab, "TCA", "Turks and Caicos Islands (the)" );
    itab_insert(itab, "SUR", "Suriname" );
    itab_insert(itab, "TJK", "Tajikistan" );
    itab_insert(itab, "TLS", "Timor-Leste" );
    itab_insert(itab, "SYR", "Syrian Arab Republic (the)" );
    itab_insert(itab, "SRB", "Serbia" );
    itab_insert(itab, "SVK", "Slovakia" );
    itab_insert(itab, "SYC", "Seychelles" );

    for( t_itab_iter i = itab_foreach(itab); i; i = itab_next(i)){
        fprintf(stderr, "%s:%s\n", itab_key(i), (char*)itab_value(i));
    }
    itab = itab_free(itab);
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


TCase *test_itab(  ) {
    TCase *tcase = tcase_create( "itab" );
    tcase_add_checked_fixture( tcase, setup, teardown );
    tcase_add_test(tcase, _demo);
    return tcase;
}
