/**
 * @file itab.c
 * @author Peter Jaeckel (jaeckel@acm.org)
 * @brief 
 * @version 0.1
 * @date 2022-08-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mem.h"
#include "itab.h"


/**
* @defgroup itab ITab
* sorted list of structures -> tables with primary index
* @{
*******************************************************************/
/**
 @brief structure of an entry in the itab.
 */
struct itab_entry {
    const char *key;            ///< key
    void *value;                ///< binary value
};
/**
 @brief structure of itab
 */
struct itab {
    unsigned total;             ///< total number of available entries
    unsigned used;              ///< actual used number of entries
    struct itab_entry *rows;    ///< array of all entries
};

/** returns the number of lines in the table 
*/
unsigned itab_lines( struct itab *itab ) {
    assert( itab != NULL );
    return itab->used;
}

/**
 @brief iterator over elements of an itab.
 */

struct itab_iter {
    struct itab *tab;           ///< table to be used
    unsigned pos;               ///< current position in the table
};

/** 
 @brief create a new itab with default parameters.
 @return reference to an itab structure.

 Detailed description follows here.
 */
struct itab *itab_new(  ) {
    struct itab *r = bc_mem_alloc( NULL, struct itab );
    r->total = 10;
    r->used = 0;
    r->rows = bc_mem_array( r, struct itab_entry, r->total );
    return r;
}

/**
@brief compares the keys of two entries
@return \arg < 0, when first key is lower
        \arg == 0, when both keys are equal
        \arg > 0, when second key is lower
*/
int itab_entry_cmp( const void *aptr, const void *bptr ) {
    const struct itab_entry *a = aptr;
    const struct itab_entry *b = bptr;
    return strcmp( a->key, b->key );
}

/**
* @brief insert a line into the table.
* @callgraph
*/
void itab_insert( struct itab *itab, const char *key, void *value ) {
    assert( itab != NULL );
    if( itab->total == itab->used ) {
        itab->total *= 2;
        itab->rows =
                bc_mem_realloc( itab, itab->rows, struct itab_entry,
                                itab->total );
    }
    struct itab_entry *row = &itab->rows[itab->used];
    row->key = bc_mem_strdup( itab, key );
    row->value = value;
    itab->used++;

    qsort( itab->rows,                           // base
           itab->used,                           // nmemb
           sizeof( struct itab_entry ),          // size
           itab_entry_cmp );
}

/**
* @brief read from an internal table by using the given key.
*
* the search internally requires an ordered list, since it is using 
* binary search to find the wanted row.
* @param itab is the table to search
* @param key is the combined key that is searched for.
* @returns the pointer to the row that has been found.
*/
void *itab_read( struct itab *itab, const char *key ) {
    assert( itab );
    assert( key );
    struct itab_entry dummy = { key, NULL };
    struct itab_entry *r = bsearch( &dummy,
                                    itab->rows,
                                    itab->used,
                                    sizeof( struct itab_entry ),
                                    itab_entry_cmp );
    if( r )
        return r->value;
    else
        return NULL;
}

/**
* @brief dumbs the content of an internal table.
*
* useful only for debugging.
*/
void itab_dump( struct itab *itab ) {
    assert( itab );
    for( int i = 0; i < itab->used; i++ ) {
        fprintf( stderr, "%s: %p\n", itab->rows[i].key, itab->rows[i].value );
    }
}

/**
*  @brief foreach implements an interator.
*
* the initialized iterator is returned.
* this iterator then is used to go to the next row in the table.
*/
struct itab_iter *itab_foreach( struct itab *tab ) {
    if( tab->used > 0 ) {
        struct itab_iter *r = bc_mem_alloc( NULL, struct itab_iter );
        r->tab = tab;
        r->pos = 0;
        return r;
    }
    else
        return NULL;
}

/**
* @brief jump to the next element in that iterator.
*/
struct itab_iter *itab_next( struct itab_iter *iter ) {
    iter->pos++;
    if( iter->tab->used > iter->pos ) {
        return iter;
    }
    else {
        bc_mem_unlink( iter );
        return NULL;
    }
}

/**
* @brief returning the value of the current row within the iterator.
*/
void *itab_value( struct itab_iter *iter ) {
    return iter->tab->rows[iter->pos].value;
}

/**
* @brief returning the key of the current row under investigation.
*/
const char *itab_key( struct itab_iter *iter ) {
    return iter->tab->rows[iter->pos].key;
}

t_itab itab_free(t_itab itab){
    return (t_itab)bc_mem_unlink(itab);
    
}


/*! @} */
