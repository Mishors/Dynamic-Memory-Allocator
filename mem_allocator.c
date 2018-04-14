/*
 * mem_allocator.c  - Memory Allocator implementation
 * 					(The base code is from Computer Systems - Programmer Prespective
 * 					by  R. Bryant and D. O'Hallaron)
 *
 *  Created on: Apr 13, 2018
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "memlib.h"
#include "config.h"

/*
 * Assumptions:
 *
 * 	1- 	Our allocator uses a model of the memory system provided by the memlib.c
 * 		The purpose of the model is to allow us to run our allocator without
 * 		interfering with the existing system-level malloc package.
 *
 * 	2- 	The mem_init function models the virtual memory available to the heap as
 * 	 	a large double-word aligned array of bytes.
 *
 * 	3-	The bytes between mem_heap and mem_brk represent allocated virtual memory.
 * 		The bytes following mem_brk represent unallocated virtual memory.
 *
 * 	4- 	The allocator requests additional heap memory by calling the mem_sbrk
 * 		function, which has the same interface as the system's sbrk function,
 * 		as well as the same semantics, except that it rejects requests to shrink
 * 		the heap.
 *
 *	5- 	The minimum block size is 16 bytes
 *
 *	6-	The free list is organized as an implicit free list (The initial Commit
 *		Only will be edited later)
 *
 *	7- 	In heap :
 *		The first word is an unused padding word aligned to a double-word
 *		boundary.
 *
 *	8-	In heap:
 *		The padding is followed by a special prologue block, which is an 8-byte
 *		allocated block consisting of only a header and a footer.
 *
 *	9-	In heap:
 *		always ends with a special epilogue block, which is a zero-size allocated
 *		block that consists of only a header.
 *
 *	10-	HINT:
 *		The prologue and epilogue blocks are tricks that eliminate the edge
 *		conditions during merging free blocks.
 *
 *	11- The allocator uses a single private (static) global variable
 *		(heap_listp) that always points to the prologue block.
 *
 */

#define 	WSIZE		4
#define 	DSIZE		8
#define 	CHUNCKSIZE	(1<<12)

char* heap_listp;
void * alloc_ptr;

//Macros

#define	MAX(x,y)			((x) > (y) ? x : y)

/*Pack a size and allocated bit into a word*/
#define PACK(size, alloc)	((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)				(*(unsigned int *)(p))
#define PUT(p,val)			(*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)			(GET(p) & ~0x7)
#define GET_ALLOC(p)		(GET(p) &  0x1)

#define HDRP(bp)			((char *)(bp) - WSIZE)
#define FTRP(bp)			((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp)		((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)		((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/*
 * The merging of free blocks function. The function handles 4 case:
 *  Case 1:
 *  	The previous and next blocks are both allocated.
 *  Case 2:
 *  	The previous block is allocated and the next block is free.
 *  Case 3:
 *  	The previous block is free and the next block is allocated.
 *  Case 4:
 *  	The previous and next blocks are both free.
 *
 * Hint: (The free list format we have chosen with its prologue and epilogue blocks that are
 * always marked as allocated-allows us to ignore the potentially troublesome edge
 * conditions where the requested block bp is at the beginning or end of the heap.)
 *
 * Returns Address of merged block
 * */
static void *merge(void *bp)
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size= GET_SIZE(HDRP(bp));

	if (prev_alloc && next_alloc) { 			/* Case 1 */
		return bp;
	}

	else if(prev_alloc && !next_alloc) {		/* Case 2 */
		size+= GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size,0));
	}

	else if (!prev_alloc && next_alloc) {	/* Case 3 */
		size+= GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}

	else {
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	return bp;
}
/*
 * Frees the requested block (bp) and then merges adjacent
 * free blocks using the boundary-tags merging technique *
 * */
void mm_free(void *bp)
{
	size_t size= GET_SIZE(HDRP(bp));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	merge(bp);
}
/*
 * The extend_heap function is invoked in two different circumstances:
 * 	1st:
 * 		When the heap is initialized
 * 	2nd:
 * 		when mm_malloc is unable to find a suitable fit
 *
 * 	To maintain alignment, extend_heap rounds up the requested size to
 * 	the nearest multiple of 2 words (8 bytes) and then requests the
 * 	additional heap space from the memory system.
 * */
static void *extend_heap(size_t words)
{
	void *bp;
	size_t size;
	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	bp= mem_sbrk(size);

	    //not enough space
	    if(bp == (void * ) -1)
	    	return NULL;

	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));

	return merge(bp);
}

/*
 * first-fit search of the implicit free list to find free block
 */

static void *find_fit(size_t asize)
{
	char* iter = heap_listp;

	while(GET_SIZE(HDRP(iter)) != 0)
	{
		if(!GET_ALLOC(HDRP(iter))){
			if(GET_SIZE(HDRP(iter)) >= asize)
				return iter;
		}
		iter = NEXT_BLKP(iter);
	}
	return NULL;
}
/*
 * Places the requested block at the beginning of the free block, splitting
 * only if the size of the remainder would equal or exceed the minimum
 * block size.
 */
void place (void *bp, size_t asize)
{
	size_t csize = GET_SIZE(HDRP(bp));
	void *temp;
	if((csize - asize) >= 2*DSIZE)
	{	//Split
		PUT(HDRP(bp), PACK(asize,1));
		PUT(FTRP(bp), PACK(asize,1));
		temp = NEXT_BLKP(bp);
		PUT(HDRP(temp), PACK(csize - asize,0));
		PUT(FTRP(temp), PACK(csize - asize,0));
	}else{
		PUT(HDRP(bp), PACK(csize,1));
		PUT(FTRP(bp), PACK(csize,1));
	}

}

/*
 * Allocates block in memory of size (size_t size)
 * Returns * (void *) pointer to the address of the block beginning
 */
void *mm_malloc(size_t size)
{
	size_t asize;
	size_t extendsize;

	if (size == 0)
		return NULL;

	if(size <= DSIZE)
		asize = 2*DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

	alloc_ptr = find_fit(asize);

	if(alloc_ptr != NULL){
		place(alloc_ptr,size);
		return alloc_ptr;
	}

	extendsize = MAX(asize, CHUNCKSIZE);
	if((alloc_ptr = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;
	place(alloc_ptr,asize);
	return alloc_ptr;
}

/*
 * Creates the heap and initializes it with free block
 */

int mm_init()
{
	/* Create initial empty heap */
	if((long)(heap_listp = mem_sbrk(4 * WSIZE)) == -1)
		return -1;

	PUT(heap_listp,0);
	PUT(heap_listp + (1*WSIZE),PACK(DSIZE,1));
	PUT(heap_listp + (2*WSIZE),PACK(DSIZE,1));
	PUT(heap_listp + (3*WSIZE),PACK(0,1));
	heap_listp += (2*WSIZE);

	if(extend_heap(CHUNCKSIZE/WSIZE) == NULL)
		return -1;
	return 0;
}






