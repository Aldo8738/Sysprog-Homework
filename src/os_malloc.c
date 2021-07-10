#include <os_malloc.h>


static void * MEM;
static size_t MEM_SIZE;
static alloc_type ALLOC_TYPE;

void os_init(void * mem, size_t mem_size, alloc_type type){
	MEM = mem;
	MEM_SIZE = mem_size;
	ALLOC_TYPE = type;

	/* Beginning of memory is also where the first block is located */
	mem_block * beginning = MEM;

	/* Initialize first block
	 * Currently its the only block and entire memory available
	 */
	beginning->next = NULL;
	beginning->free = 1;
	beginning->size = MEM_SIZE;
}

int roundUp(int size) {
	int remainder = size % 8;
	if (remainder == 0)
		return size;
	
	return size + 8 - remainder; 
}

void insertElement(mem_block* memLoc, size_t size) {
	mem_block* freeSpace = (void*)((void*)memLoc + roundUp(size) + sizeof(mem_block));

	if (memLoc->size - sizeof(mem_block) - roundUp(size) < 8 + sizeof(mem_block)) {
		mem_block* it = MEM;
		
		memLoc->free = 0;
		while (it != NULL) {
			if (it->free == 1) {
				it->size += memLoc->size - sizeof(mem_block) - roundUp(size);
				break;
			}
			it = it->next;
		}
		memLoc->size = roundUp(size) + sizeof(mem_block);

		return;
	}

	freeSpace->next = memLoc->next;
	freeSpace->size = memLoc->size - sizeof(mem_block) - roundUp(size);
	freeSpace->free = 1;

	memLoc->size = roundUp(size) + sizeof(mem_block);
	memLoc->free = 0;
	memLoc->next = freeSpace;
	
}

void * os_malloc_first_fit(size_t size){
	if (size < 1) return NULL;
	
	mem_block* temp = MEM;
	while ((temp != NULL) && ((temp->size < roundUp(size) + sizeof(mem_block))||(temp->free == 0))) {
		temp = temp->next;
	}

	if (temp != NULL) {
		insertElement(temp, size);
		return (void*)(++temp);
	}
	return NULL;
}

void * os_malloc_best_fit(size_t size){
	if (size < 1) return NULL;
	mem_block* temp = MEM;
	mem_block* bestFit = MEM;
	size_t best = MEM_SIZE;
	while (temp != NULL) {
		if (temp->free != 0 && temp->size >= roundUp(size) + sizeof(mem_block)) {
			
			if (temp->size < best) {
				best = temp->size;
				bestFit = temp;
			}
		}
		temp = temp->next;
	}
	
	if (bestFit != NULL && bestFit->free != 0) {
		insertElement(bestFit, size);
		return (void*)(++bestFit);
	}
	return NULL;
}

void * os_malloc_worst_fit(size_t size){
	if (size < 1) return NULL;
	mem_block* temp = MEM;
	mem_block* worstFit = NULL;
	size_t worst = 0;
	while (temp != NULL) {
		if (temp->free != 0 && temp->size >= roundUp(size) + sizeof(mem_block)) {
			if (temp->size > worst) {
				worst = temp->size;
				worstFit = temp;
			}
		}
		temp = temp->next;
	}
	
	if (worstFit != NULL && worstFit->free == 1) {
		insertElement(worstFit, size);
		return (void*)(++worstFit);
	}
	
	return NULL;
}

void * os_malloc(size_t size) {
	switch(ALLOC_TYPE) {
		case FIRST_FIT: return os_malloc_first_fit(size);
		case BEST_FIT: return os_malloc_best_fit(size);
		case WORST_FIT: return os_malloc_worst_fit(size);
		default: return NULL;
	}
}

void os_free(void *ptr){
	
	if ((void*)MEM > ptr && ptr > (void*)MEM + MEM_SIZE)
	 	return;

	mem_block* temp = ptr;
	mem_block* it = MEM;

	--temp;
	
	if (it == temp){
		if (temp->next->free != 0) {
			temp->size += temp->next->size;
			temp->next = temp->next->next;
			temp->free = 1;
			return;
		} else {
			temp->free = 1;
		}
	} 
	
	mem_block* following = it->next;
	while (following->next != NULL) {

		if (following == temp){
			if (it->free == 0 && (following->next->free == 0)) {
				
				following->free = 1;
				
			} else if (it->free != 0 && following->next->free != 0){
				
				it->size += following->size + following->next->size;
				it->next = following->next->next;

			} else if (it->free == 1 && following->next->free == 0) {
				
				it->size += following->size;
				it->next = following->next;
				
			} else {
				following->size += following->next->size; 
				following->next = following->next->next;
				following->free = 1;
			}
			
			return;
		}
		following = it->next;
		it = it->next;
	}

	// if (following == temp) {
	// 	if (it->free == 1) {
	// 		it->size += following->size;
	// 	} else {
	// 		temp->free = 1;
	// 	}
	// 	return;
	// }
}
