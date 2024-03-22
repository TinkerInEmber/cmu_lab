/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam-b517",
    /* First member's full name */
    "Mao Mao",
    /* First member's email address */
    "2471680960@qq.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
/* Basic constant and macros */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x)>(y)? (x): (y))

/* Pack a size and alloccated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next adn previous blocks */
#define NEXT_BLKP(p) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))
#define PREV_BLKP(p) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

static char *heap_listp;

/* Function declaration */
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void *best_fit(size_t asize);
static void place(void *bp, size_t asize);
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	/* creater the initial empty heap */
	if((heap_listp = mem_sbrk(4*WSIZE)) == (void*) -1)
		return -1;
	PUT(heap_listp, 0);
	PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));
	PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));
	PUT(heap_listp + (3*WSIZE), PACK(0, 1));
	heap_listp += (2*WSIZE);
	
	/* Extend the empty head with a free block of CHUNKSIZE bytes */
	if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
		return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;		/* Adjusted block size */
    size_t extendsize;	/* Amount to extend heap if no fit */
    char *bp;
    
    /* Ignore spurious requests */
    if(size == 0)
    	return NULL;
	
	/* Adjust block size to include overhead and alignment reqs. */
	if(size <= DSIZE)
		asize = 2*DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE)+(DSIZE-1)) / DSIZE);
		
	/* Search the free list for a fit */
	if((bp = best_fit(asize))!= NULL) {
		place(bp, asize);
		return bp;
	}
	
	/* No fit found. Get more memory and place the block */
	extendsize = MAX(asize, CHUNKSIZE);
	if((bp = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;
	place(bp, asize);	
	return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	size_t size = GET_SIZE(HDRP(ptr));
	
	PUT(HDRP(ptr), PACK(size, 0));
	PUT(FTRP(ptr), PACK(size, 0));
	coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 * (1) change the calculation of copySize
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    newptr = mm_malloc(size);

    if (newptr == NULL)
      return NULL;
    // copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}



/*
 * extend heap
*/
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;

    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));           /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));           /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

static void* coalesce(void *bp) 
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));
	
	if(prev_alloc && next_alloc) { 			/* case 1 */
		return bp;
	}
	else if(prev_alloc && !next_alloc) { 	/* case 2 */
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));		/* first modify header */
		PUT(FTRP(bp), PACK(size, 0));		/* footer is modified accorded to header */	
	}
	else if(!prev_alloc && next_alloc) { 	/* case 3 */
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	else { 									/* case 4 */
		size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	return bp;
}
/*
*	(1)if can't find fit free block, return NULL and extend_heap in malloc function
* 	extend_heap is done in mm_malloc
*	(2)if find fit, return the block's address
*	realize a Implicit Free List
*/
static void *find_fit(size_t asize){
	/* find if any fit block int current free list */
	void *bp = heap_listp;
	size_t alloc = GET_ALLOC(HDRP(bp));
	size_t size = GET_SIZE(HDRP(bp));
	
	while(size > 0){
		if(size >= asize && alloc == 0){
			return bp;
		}
		bp = NEXT_BLKP(bp);
		alloc = GET_ALLOC(HDRP(bp));
		size = GET_SIZE(HDRP(bp));
	}
	return NULL;
}

static void *best_fit(size_t asize){
	void *bp = heap_listp;
	size_t alloc = GET_ALLOC(HDRP(bp));
	size_t size = GET_SIZE(HDRP(bp));
	size_t bestSize = 0;
	while(size > 0){
		if(size >= asize && alloc == 0){
			if(size == asize) 
				return bp;
			else{
				if(bestSize == 0 || size < bestSize) bestSize = size;
			}
		}
		bp = NEXT_BLKP(bp);
		alloc = GET_ALLOC(HDRP(bp));
		size = GET_SIZE(HDRP(bp));
	}
	bp = heap_listp;
	alloc = GET_ALLOC(HDRP(bp));
	size = GET_SIZE(HDRP(bp));
	while(size > 0){
		if(size == bestSize && alloc == 0){
			return bp;
		}
		bp = NEXT_BLKP(bp);
		alloc = GET_ALLOC(HDRP(bp));
		size = GET_SIZE(HDRP(bp));
	}
	return NULL;
}
/*
 * 	place: 
 *	if the size of the left of the fit block >= min block size, cut apart
 *	else just put the appied block at the start place of the free block.
*/
static void place(void *bp, size_t asize){
	size_t size = GET_SIZE(HDRP(bp));
	// char* footer = FTRP(bp);
	if((size - asize) >= DSIZE*2){ // min block size is 16 bytes
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		PUT(HDRP(NEXT_BLKP(bp)), PACK(size - asize, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size - asize, 0));
	}
	else{
		PUT(HDRP(bp), PACK(size, 1)); // there must use size but not asize, for alignment
		PUT(FTRP(bp), PACK(size, 1));
	}
	return;
}














