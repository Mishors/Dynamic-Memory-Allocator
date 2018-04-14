/*
 * memlib.h
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */

#ifndef MEMLIB_H_
#define MEMLIB_H_

#include <unistd.h>


void mem_init(void);
void mem_deinit(void);
void *mem_sbrk(int incr);
void mem_reset_brk(void);
void *mem_heap_lo(void);
void *mem_heap_hi(void);
size_t mem_heapsize(void);
size_t mem_pagesize(void);

#endif /* MEMLIB_H_ */
