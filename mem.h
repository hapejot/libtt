/**
* @file mem.h
* @brief defines all memory component related stuff.
*
* This is using an abstract way of calling components.
* only a structure of function pointers is defined and a set of defines to call them properly.
*/

#ifndef MEM_H
#define MEM_H
#include <stdbool.h>
#include <string.h>
/**
* @defgroup mem Memory Management
*
* Memory management is used as a global component. The concrete implementation is hidden from the callers.
* this could be a very simple implementation or one that is really doing some garbage collection.
*
* @startuml
* Client -> Memory : init
* Client -> Memory : realloc
* Client -> Memory : free
* Client -> Memory : usage report
* @enduml
*
* @{
*/

/**
* @brief the structure defines the functions a memory components needs to implement
*/
struct mem {
    /** free memory */
    int ( *free ) ( void *ptr );
    /** getting the type of the chunk */
    void *( *get_type ) ( void *ptr, int type );
    /** allocating memory */
    void *( *realloc ) ( void *ctx, void *p, int type, int count,
                         const char *file, int line );
    /** increasing usage counter */
    void *( *link ) ( void *src, void *target );
    /** decreasing usage counter */
    void *( *unlink ) ( void *ptr, const char *file, int line );


    /** create checkpoint */
    void ( *checkpoint )( void *ptr, const char *file, int line );
    /** check validity */
    bool ( *is_valid )( void *ptr );

    void (*report)(void); /** < reporting of the current memory status */
};

/**
* there is one memory management component centrally defined.
* we are referencing this.
*/
extern struct mem g_mem;

/**
* @brief main call to allocate memory or eventually reallocate memory.
*/
#define bc_mem_realloc(ctx, p, type, count) (void*)(g_mem.realloc(ctx, p, sizeof(type), count, __FILE__, __LINE__))

/**
* @brief allocates a single structure
*/
#define bc_mem_alloc(ctx, type) (void*)(g_mem.realloc(ctx, NULL, sizeof(type), 1, __FILE__, __LINE__))

/**
* @brief allocates an array of a single type
*/
#define bc_mem_array(ctx, type, count) (void*)(g_mem.realloc(ctx, NULL, sizeof(type), count, __FILE__, __LINE__))

/**
* @brief getting the type of a memory chunk
*/
#define bc_mem_get_type(ptr, type) (void*)(g_mem.get_type(ptr, type))

/**
* @brief adding a link to a memory chunk
*
* this introduces reference counting. An unlink would only free
* this memory if the reference count goes to 0.
*/
#define bc_mem_link(src, target) (void*)(g_mem.link(src, target))

/**
* @brief reduce the usage counter of a memory chunk.
*
* if the usage goes to 0, this memory is freed.
*/
#define bc_mem_unlink(ptr) (g_mem.unlink((void**)(ptr), __FILE__, __LINE__))

/**
* @brief creates a checkpoint of this memory chunk. 
*
* Every change that is performed
* subsequent to the checkpoint is recognized
*/
#define bc_mem_checkpoint(ptr) (g_mem.checkpoint(ptr, __FILE__, __LINE__))

/**
* @brief check if the structure is valid.
* there are several levels of validity:
* - The header and sentinel magic areas are intact
* - the checksum over internal data is equal to the save one from the previous checkpoint
*/
#define bc_mem_is_valid(ptr) (g_mem.is_valid(ptr))


/**
 * @brief strdup, with additional handling of the memory context
 * 
 */
inline static char *bc_mem_strdup( void *ctx, const char *str ) {
    size_t l = strlen( str );
    char *result = bc_mem_array( ctx, char, l + 1 );
    memcpy( result, str, l + 1 );
    bc_mem_checkpoint(result);
    return result;
}


/**
 * @brief reports the memory usage for each individual chunk
 */
#define bc_mem_report() (g_mem.report()) 


/**
 * @brief initializes the memory manager and does the setup for the function calls.
 */
extern void bc_mem_init(void);

/**
* @}
*/



#endif
