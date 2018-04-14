# Dynamic Memory Allocator

Dynamic memory allocator project to implement malloc and free functions in C to perform like the built-int functions in malloc Package of C. 
**Hint:** The base code for this project is taken from Computer Systems_ A Programmer's Perspective by Randal E. Bryant, David R. O'Hallaron 

## Assumptions

* Our allocator uses a model of the memory system provided by the ```memlib.c``` .The purpose of the model is to allow us to run our allocator without interfering with the existing system-level malloc package.
* The ```mem_init``` function models the virtual memory available to the heap as a large double-word aligned array of bytes.
* The bytes between ```mem_heap``` and ```mem_brk``` represent allocated virtual memory. The bytes following ```mem_brk``` represent unallocated virtual memory.
* The allocator requests additional heap memory by calling the ```mem_sbrk``` function, which has the same interface as the system's ```sbrk``` function, as well as the same semantics, except that it rejects requests to shrink the heap.
* The minimum block size is **16 bytes**.
* The free list is organized as an implicit free list (The initial Commit Only will have another efficient lists like explicit lists or segregatted lists).
* In heap : The first word is an unused padding word aligned to a double-word boundary.
* In heap : The padding is followed by a special **prologue** block, which is an 8-byte allocated block consisting of only a header and a footer.
* In heap : Always ends with a special **epilogue** block, which is a zero-size allocated block that consists of only a header.
* HINT : The prologue and epilogue blocks are tricks that eliminate the edge conditions during merging free blocks.
* The allocator uses a single private (```static```) global variable ```heap_listp``` that always points to the prologue block.

## mem_allocator.c
This is the file in which ```mm_malloc``` and ```mm_free``` are implemented 


### Functions Description

* ```int mm_init()``` : Creates the heap and initializes it with free block
* ```void *mm_malloc(size_t size)``` : Allocates block in memory of size (size_t size) . Returns (void *) pointer to the address of the block beginning
* ```void mm_free(void *bp)``` : Frees the requested block (bp) and then merges adjacent free blocks using the boundary-tags merging technique 
* ```static void *extend_heap(size_t words)``` : The extend_heap function is invoked in two different circumstances: 1st: When the heap is initialized 2nd: when mm_malloc is unable to find a suitable fit To maintain alignment, extend_heap rounds up the requested size to the nearest multiple of 2 words (8 bytes) and then requests the additional heap space from the memory system.
* ```static void *merge(void *bp)``` : Returns Address of merged free blocks
* ```static void *find_fit(size_t asize)``` : First-fit search of the implicit free list to find free block
* ``` void place (void *bp, size_t asize) ``` : Places the requested block at the beginning of the free block, splitting only if the size of the remainder would equal or exceed the minimum block size.






