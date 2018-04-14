/*
 * main.c - Testing Memory Allocator
 *
 *  Created on: Apr 13, 2018
 *      Author: Mohamed Raafat
 */

#include <stdio.h>

extern void mem_init(void);
extern int mm_init(void);
extern void mm_free(void *bp);
extern void *mm_malloc(size_t size);

int main()
{
	/* Initializing the simulation of memory system */
	mem_init();

    /* Preparing mm_malloc and mm_free functions*/
	mm_init();

	char *arr =  (char *)mm_malloc(2);

	*arr = 'R';

	printf("Testing .. %c",*arr);

}
