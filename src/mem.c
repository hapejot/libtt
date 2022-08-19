/**
 * @file mem.c
 * @author Peter Jaeckel (jaeckel@acm.org)
 * @brief 
 * @version 0.1
 * @date 2022-08-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "internal.h"
#include "mem.h"
#include <malloc.h>
#include <assert.h>

extern struct mem g_mem;
#define MAGIC_START 0x12345678

typedef struct {
    const char *file;
    int line;
} t_location;

typedef struct {
    unsigned int c;
    unsigned int sum;
} t_magic;

typedef struct mem_hd {
    t_magic magic;
    size_t size;    ///< total allocated size */
    size_t len;     ///< requested size */
    t_location allocated;
    t_location last_checked;
    t_location freed;
    struct mem_hd *next;
    struct mem_hd * next_free;
    struct mem_lst * children;    
    bool free;

    char *data[0];
} t_mem_hd;

typedef struct mem_lst {
    t_mem_hd * chunk;
    struct mem_lst * next;
} t_mem_lst;



typedef struct mem_sentinel {
    t_magic magic;
} t_mem_sentinel;


/** 
 * @brief all chunks that have been allocated.
 * 
 * This bookkeeping is done in order to do some reporting at the end.
 */
static t_mem_hd *g_chunks = NULL;

/**
 * @brief list of all free chunks
 * 
 * This list can be used to recycle freed memory chunks.
 * We search for available chunks with the exact page size. This makes things easier for now.
 * 
 */

static t_mem_hd *g_free_list = NULL;

/**
 * @brief lower limit of memory.
 * 
 * see mem_adjust_limits for further discussion.
 */
static char *mem_limit_low = NULL;
/**
 * @brief upper limit of memory.
 * 
 * see mem_adjust_limits for further discussion.
 */
static char *mem_limit_high = NULL;



static void mem_checkpoint( void *ptr, const char *file, int line );
static void* mem_unlink( void *ptr , const char *file, int line );



static void *payload_ptr( t_mem_hd * hd ) {
    return ( char * )hd + sizeof( t_mem_hd );
}

static t_mem_hd *header_ptr( void *payload ) {
    char *m = ( char * )payload;
    m -= sizeof( t_mem_hd );
    return ( void * )m;
}

static t_mem_sentinel *sentinel_ptr( t_mem_hd * hd ) {
    char *m = ( char * )hd;
    m += sizeof( *hd );
    m += hd->len;
    return ( t_mem_sentinel * ) m;
}

static t_magic calculate_magic( t_mem_hd * hd ) {
    t_magic magic;
    magic.c = MAGIC_START;
    magic.sum = hd->len;
    unsigned char *m = ( unsigned char * )payload_ptr( hd );
    for( size_t i = 0; i < hd->len; i++ ) {
        magic.sum += m[i];
    }
    return magic;
}

static void check_ptr( void *ptr, bool *range, bool *magic, bool *sum ) {
    t_mem_hd *hd = NULL;
    *range = false;
    *magic = false;
    if( sum != NULL )
        *sum = false;
    if( ( mem_limit_low != NULL ) && ( mem_limit_low <= ( char * )ptr )
        && ( ( char * )ptr <= mem_limit_high ) ) {
        *range = true;
    }
    if( *range ) {
        hd = header_ptr( ptr );
        t_mem_sentinel *sentinel = sentinel_ptr( hd );

        *magic = ( hd->magic.c == MAGIC_START )
                && ( sentinel->magic.c == MAGIC_START )
                && ( hd->magic.sum == sentinel->magic.sum );
    }
    if( *magic && sum != NULL ) {
        t_magic magic = calculate_magic( hd );
        if( magic.sum == hd->magic.sum )
            *sum = true;
    }

// if(!*range) fprintf(stderr, "range check failed %p\n", ptr);
// if(!*magic) fprintf(stderr, "magic check failed %p\n", ptr);
// if(sum != NULL && !*sum) fprintf(stderr, "checksum check failed %p\n", ptr);
}



static bool mem_is_valid( void *ptr ) {
    bool range;
    bool magic;
    bool sum;

    check_ptr( ptr, &range, &magic, &sum );

    return range && magic && sum;
}

/**
 * @brief there are two variables that track the lower and upper limit
 *          of used memory from this library. 
 * 
 *  These limits are used to check if a pointer is eventually a valid pointer
 *  for this memory manager. This is not very reliable but can keep away a couple of 
 *  *false* pointers. NULL pointers for example. This of course is not working for
 *  Implementations where the NULL pointer is not zero. And also memory addresses are required
 *  to be in a sequence and ordered in a linear order, otherwise the comparisons might fail.
 *  Good example are old MSDOS/Windows pointer models.
 * 
 * @param m   char pointer to the start of the memory chunk
 * @param total_size  size of the chunk
 */
static void mem_adjust_limits(char* m, size_t total_size){
    if( mem_limit_low == NULL || mem_limit_low > m ) {
        mem_limit_low = m;
    }
    m += total_size;
    if( mem_limit_high == NULL || mem_limit_high < m ) {
        mem_limit_high = m;
    }
}


/**
 * @brief find the next page in the list of free pages.
 * 
 * It is checked for an exact match of the size. 
 * Search is done sequentially through all available pages.
 * 
 * @param page_size     size of the page
 * @return t_mem_hd*    pointer to the header, or NULL if nothing found.
 */
static t_mem_hd * mem_find_free_chunk(size_t page_size){
    t_mem_hd *prev_hd = NULL; 
    t_mem_hd *result = NULL;
    for(t_mem_hd *hd = g_free_list; hd; hd = hd->next_free){
        if(hd->size == page_size){
            result = hd;
            if(prev_hd) prev_hd->next_free = hd->next_free;
            else g_free_list = hd->next_free;
            break;
        }
    }
    return result;
}


static void *mem_realloc( void *context, void *ptr, int size, int count,
                          const char *file, int line ) {
    size_t payload_size = size * count;
    size_t total_size =
            sizeof( t_mem_hd ) + sizeof( t_mem_sentinel ) + payload_size;
    size_t page_size = 128;
    while(total_size > page_size) page_size <<= 1;
    
    t_mem_hd *hd = mem_find_free_chunk(page_size);
    if(hd == NULL){ 
        hd = malloc( page_size );
        memset( hd, 0, page_size );
        hd->next = g_chunks;
        g_chunks = hd;    
        mem_adjust_limits((char*)hd, total_size);
        hd->size = page_size;
    }

    hd->len = payload_size;
    hd->magic = calculate_magic( hd );
    t_mem_sentinel *s = sentinel_ptr( hd );
    s->magic = hd->magic;
    hd->allocated.file = file;
    hd->allocated.line = line;
    hd->last_checked = hd->allocated;
    hd->free = false;

    if(context){
        bool range;
        bool magic;
        check_ptr(context, &range, &magic, NULL);
        assert(range && magic);
        if(range && magic) {
            t_mem_hd *  parent = header_ptr(context);
            t_mem_lst * lst = malloc(sizeof(t_mem_lst));
            lst->chunk = hd;
            lst->next = parent->children;
            parent->children = lst;
        }
    }

    if(ptr){
        t_mem_hd *oldhd = header_ptr(ptr);
        memcpy(hd->data, ptr, oldhd->len);
        mem_checkpoint(hd->data, file, line);
        mem_unlink(ptr, file, line);
    }

    return payload_ptr( hd );
}

static void mem_checkpoint( void *ptr, const char *file, int line ) {
    bool range;
    bool magic;

    check_ptr( ptr, &range, &magic, NULL );

    if( range && magic ) {
        t_mem_hd *hd = header_ptr( ptr );
        t_mem_sentinel *sentinel = sentinel_ptr( hd );
        t_magic m = calculate_magic( hd );
        hd->magic = m;
        sentinel->magic = m;
        hd->last_checked.file = file;
        hd->last_checked.line = line;
    }
}

static void* mem_unlink( void *ptr, const char *file, int line ) {
    bool range;
    bool magic;

    check_ptr( ptr, &range, &magic, NULL );
    assert(range && magic);
    if( range && magic ) {
        t_mem_hd *hd = header_ptr( ptr );
        for(t_mem_lst *child = hd->children; child; child = child->next){
            (void) mem_unlink(child->chunk->data, file, line);
        }        

        hd->free = true;
        hd->freed.file = file;
        hd->freed.line = line;
        hd->next_free = g_free_list;
        g_free_list = hd;
        return NULL;
    }
    return ptr;
}


static void mem_report(  void ) {
    char status[80];
    fprintf( stderr, "*** * memory report ***\n" );
    int no = 1;
    for( t_mem_hd *hd = g_chunks; hd; hd = hd->next ) {
        bool range;
        bool magic;
        bool sum;

        check_ptr( hd->data, &range, &magic, &sum );


        sprintf(status, "%s%s%s",  
                        hd->free ? "free " : "", 
                        magic ? "" : "corrupted ",
                        ( !magic || sum ) ? "" : "modified ");
        fprintf( stderr, "%5d %p %6llu %6llu %-20s  %s(%d)", no, hd,
                 hd->len, hd->size, status, hd->allocated.file,
                 hd->allocated.line);
        no++;
        if( hd->allocated.file == hd->last_checked.file
            && hd->allocated.line == hd->last_checked.line ) {
            }                 
            else {
                fprintf(stderr, " / %s(%d)",
                            hd->last_checked.file,
                            hd->last_checked.line );
            }
        if(hd->freed.file) {
                            fprintf(stderr, " v %s(%d)",
                            hd->freed.file,
                            hd->freed.line );
        }

        fprintf(stderr, "\n");
    }
    fprintf( stderr, "*** end of report ***\n" );
}

void bc_mem_init(  ) {
    g_mem.realloc = mem_realloc;
    g_mem.unlink = mem_unlink;
    g_mem.is_valid = mem_is_valid;
    g_mem.checkpoint = mem_checkpoint;
    g_mem.report = mem_report;
}

