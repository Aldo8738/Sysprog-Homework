#ifndef OS_MALLOC_H
#define OS_MALLOC_H

#include <stddef.h>

/* All possible allocation types */
typedef enum allocation_type {
	FIRST_FIT,
	BEST_FIT,
	WORST_FIT
} alloc_type;

/* This struct holds information about the memory section.
 * A memory block should always be on a 8 byte aligned address
 */
typedef struct _mem_block {

	/* Points to the next memory block.
	 * NULL if this is the last block
	 */
	struct _mem_block * next;

	/* Inidcates if block is available or used
	 *  0 -> Block is holding memory that is in use (not yet freed)
	 *  _ -> free
	 */
	unsigned int free;

	/* Total size of the memory section.
	 * Size should include:
	 *  + the requested size by the user
	 *  + size of this struct
	 *  + required padding
	 */
	size_t size;
} mem_block;

/* Initializes the memory and creates the first memory block.
 * @param mem: Pointer to the beginning of the memory. Has to be 8 Byte aligned
 * @param mem_size: Total size of the memory
 * @param alloc_type: The allocation strategy to use
 */
void os_init(void * mem, size_t mem_size, alloc_type type);

/* Allocates memory using the strategy set in os_init().
 * @param size: The requested size to allocate
 * @return Pointer to the usable memory (NOT the mem_block struct!)
 *         or NULL if no fitting block could be found
 */
void * os_malloc(size_t size);

/* Frees the memory and makes it usable again for later allocations
 * @param ptr: Pointer to the usable memory. This pointer should originate
 *             from a os_malloc() call
 */
void os_free(void *ptr);

#endif
