#ifndef MEM_H
#define MEM_H

extern struct mem {
    int ( *free )( void *ptr );
    void *( *get_type )( void *ptr, int type );
    void *( *realloc )( void *ctx, void *p, int type, int count, const char* file, int line );
    void *( *link )( void *src, void *target );
    void ( *unlink )( void **ptr );


    void (*checkpoint)(void *ptr, const char* file, int line);
    bool (*is_valid)(void * ptr);
} g_mem;

#define bc_mem_free(ptr) (int)(g_mem.free(ptr))
#define bc_mem_get_type(ptr, type) (void*)(g_mem.get_type(ptr, type))
#define bc_mem_realloc(ctx, p, type, count) (void*)(g_mem.realloc(ctx, p, sizeof(type), count, __FILE__, __LINE__))
#define bc_mem_link(src, target) (void*)(g_mem.link(src, target))
#define bc_mem_unlink(ptr) (g_mem.unlink((void**)(ptr)))

#define bc_mem_checkpoint(ptr) (g_mem.checkpoint(ptr, __FILE__, __LINE__))
#define bc_mem_is_valid(ptr) (g_mem.is_valid(ptr))


#endif