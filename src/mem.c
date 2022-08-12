#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "internal.h"
#include "mem.h"
#include <malloc.h>

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
    size_t len;
    t_location allocated;
    t_location last_checked;
    struct mem_hd *next;
    bool free;

    char *data[0];
} t_mem_hd;

t_mem_hd *g_chunks = NULL;

typedef struct mem_sentinel {
    t_magic magic;
} t_mem_sentinel;

char *mem_limit_low = NULL;
char *mem_limit_high = NULL;


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


static void *mem_realloc( void *context, void *ptr, int size, int count,
                          const char *file, int line ) {
    size_t payload_size = size * count;
    size_t total_size =
            sizeof( t_mem_hd ) + sizeof( t_mem_sentinel ) + payload_size;
    t_mem_hd *hd = malloc( total_size );
    char *m = ( char * )hd;
    memset( hd, 0, total_size );
    hd->len = payload_size;

    if( mem_limit_low == NULL || mem_limit_low > m ) {
        mem_limit_low = m;
// fprintf(stderr,"low: %p\n", mem_limit_low);
    }
    m += total_size;
    if( mem_limit_high == NULL || mem_limit_high < m ) {
        mem_limit_high = m;
// fprintf(stderr,"high: %p\n", mem_limit_high);
    }

    hd->magic = calculate_magic( hd );
    t_mem_sentinel *s = sentinel_ptr( hd );
    s->magic = hd->magic;
    hd->allocated.file = file;
    hd->allocated.line = line;
    hd->last_checked = hd->allocated;
    hd->free = false;
    hd->next = g_chunks;
    g_chunks = hd;

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

static void mem_unlink( void **ptr ) {
    bool range;
    bool magic;

    check_ptr( *ptr, &range, &magic, NULL );

    if( range && magic ) {
        t_mem_hd *hd = header_ptr( *ptr );
// free( hd );
        hd->free = true;
        *ptr = NULL;
    }
}



void mem_init(  ) {
    g_mem.realloc = mem_realloc;
    g_mem.unlink = mem_unlink;
    g_mem.is_valid = mem_is_valid;
    g_mem.checkpoint = mem_checkpoint;
}


void mem_report(  ) {
    fprintf( stderr, "*** memory report ***\n" );
    for( t_mem_hd * hd = g_chunks; hd; hd = hd->next ) {
        bool range;
        bool magic;
        bool sum;

        check_ptr( hd->data, &range, &magic, &sum );

        fprintf( stderr, "chunk: %p %llu %s%s%s%s(%d) / %s(%d)\n", hd,
                 hd->len, hd->free ? "free " : "", magic ? "" : "corrupted ",
                 ( !magic
                   || sum ) ? "" : "modified ", hd->allocated.file,
                 hd->allocated.line, hd->last_checked.file,
                 hd->last_checked.line );
    }
    fprintf( stderr, "*** end of report ***\n" );
}
