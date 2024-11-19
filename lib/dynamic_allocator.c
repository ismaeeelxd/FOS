/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("Current Block Address : %p \n",blk);
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
    //==================================================================================
    //DON'T CHANGE THESE LINES==========================================================
    //==================================================================================
    {
        if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
        if (initSizeOfAllocatedSpace == 0)
            return ;
        is_initialized = 1;
    }
    //==================================================================================
    //==================================================================================

    //TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("initialize_dynamic_allocator is not implemented yet");
    LIST_INIT(&freeBlocksList);
    uint32* begBlock = (uint32*)daStart;
    uint32* endBlock = (uint32*)(daStart + initSizeOfAllocatedSpace - sizeof(int));
    *begBlock = 0 | 1;
    *endBlock = *begBlock;
    uint32* header = (uint32*)(daStart + sizeof(int));
    *header = initSizeOfAllocatedSpace-8;
    struct BlockElement* firstFreeBlock = (struct BlockElement*)(daStart + 2*sizeof(int));
    LIST_INSERT_HEAD(&freeBlocksList,firstFreeBlock);
    uint32* footer = (uint32*)(daStart +  initSizeOfAllocatedSpace - 2*sizeof(int));
    *footer = *header;
}

//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================

void makelSB1(uint32 *header,uint32 totalSize){
	*header = (totalSize) | (0x1);

}
void makelSB0(uint32 *header,uint32 totalSize){
	*header = (totalSize) & (~0x1);

}


void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data

	uint32 *header = (uint32*) va - 1;
	uint32 *footer = (uint32*)(va + (totalSize) - 8);
	if(isAllocated)
		makelSB1(header,totalSize);
	else
		makelSB0(header,totalSize);

	*footer = *header;


}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF

	if(!size){
		cprintf("1\n");
		return NULL;
	}	size+= 8;

	if(size < 16){
		size = 16;
	}
	if(!LIST_SIZE(&freeBlocksList)){
		void* address = sbrk((ROUNDUP(size,PAGE_SIZE)) / PAGE_SIZE);
		if(address == (void*)-1)
		{
			cprintf("2\n");
			return NULL;
		}
		uint32* newEndBlock = (uint32*)(address + (ROUNDUP(size,PAGE_SIZE)) - 4);
		*newEndBlock = 0 | 1;
		uint32* header = (uint32*)(address - 4);
		uint32* footer = (uint32*)(address + (ROUNDUP(size,PAGE_SIZE)) - 8);
		*header = (ROUNDUP(size,PAGE_SIZE)) & ~(0x1);
		*footer = (ROUNDUP(size,PAGE_SIZE)) & ~(0x1);
		LIST_INSERT_HEAD(&freeBlocksList,(struct BlockElement*) address);
	}

	struct BlockElement *freeBlk;
	LIST_FOREACH(freeBlk,&freeBlocksList){
		uint32 blkSize = get_block_size(freeBlk);
		if(size > blkSize){
			if(!(LIST_NEXT(freeBlk))){

				void* address = sbrk((ROUNDUP(size,PAGE_SIZE)) / PAGE_SIZE);
				if(address == (void*)-1)
				{
					cprintf("3\n");
					return NULL;
				}
				uint32 prevBlockSize = (*((uint32*)(address - 8))) & ~(0x1);
				void* prevBlock = address - prevBlockSize;
				uint32* footer = (uint32*)(address + (ROUNDUP(size,PAGE_SIZE)) - 8);
				uint32* prevBlockHeader = (uint32*)(address - prevBlockSize - 4);
				uint32* newEndBlock = (uint32*)(address + (ROUNDUP(size,PAGE_SIZE)) - 4);
				*newEndBlock = 0 | 1;
				*((uint32*)(address - 4)) = 0;
				if(is_free_block(prevBlock)){
					*prevBlockHeader = (prevBlockSize + ((ROUNDUP(size,PAGE_SIZE)) & ~(0x1)));
					*footer = (prevBlockSize + ((ROUNDUP(size,PAGE_SIZE)) & ~(0x1)));
					return alloc_block_FF(size-8);
					//OPTIMIZE THIS
				}
				uint32* header = (uint32*)(address - 4);
				*header = (ROUNDUP(size,PAGE_SIZE)) & (0x1);
				*footer = (ROUNDUP(size,PAGE_SIZE)) & (0x1);

				continue;
			} else
				continue;
		}
		else if(size == blkSize || blkSize-size < 16){
			set_block_data(freeBlk,blkSize,1);
			LIST_REMOVE(&freeBlocksList,freeBlk);
			return freeBlk;
		}
		else {
				struct BlockElement *newFreeBlk = (struct BlockElement*)((void*)freeBlk + size);
				set_block_data(newFreeBlk,blkSize-size,0);
				set_block_data(freeBlk,size,1);
				LIST_INSERT_AFTER(&freeBlocksList,freeBlk,newFreeBlk);
				LIST_REMOVE(&freeBlocksList,freeBlk);
				return freeBlk;
			}
	}


	cprintf("8\n");
	return NULL;

}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF


	if (!size)
		return NULL;
if(size%2!=0)size++;
	size += 8;

	struct BlockElement *bestFitBlk = NULL;
	uint32 bestFitSize = (uint32)-1;
	struct BlockElement *freeBlk;

	LIST_FOREACH(freeBlk, &freeBlocksList) {
		uint32 blkSize = get_block_size(freeBlk);

		if (blkSize >= size && blkSize < bestFitSize) {
			bestFitBlk = freeBlk;
			bestFitSize = blkSize;
		}
	}

	if (bestFitBlk == NULL)
		return NULL;

	if (bestFitSize == size || (bestFitSize - size) < 16) {
		set_block_data(bestFitBlk, bestFitSize, 1);
		LIST_REMOVE(&freeBlocksList, bestFitBlk);
		return bestFitBlk;
	}
	else {
		struct BlockElement *newFreeBlk = (struct BlockElement *)((uint32 *)bestFitBlk + size / sizeof(uint32));

		set_block_data(newFreeBlk, bestFitSize - size, 0);
		set_block_data(bestFitBlk, size, 1);

		LIST_INSERT_AFTER(&freeBlocksList, bestFitBlk, newFreeBlk);

		LIST_REMOVE(&freeBlocksList, bestFitBlk);

		return bestFitBlk;
	}
}


//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_one_block(struct BlockElement *curBlockElement) {
    struct BlockElement *freeBlk;
    if (!LIST_SIZE(&freeBlocksList)) {
        LIST_INSERT_HEAD(&freeBlocksList, curBlockElement);
        return;
    }

    LIST_FOREACH(freeBlk, &freeBlocksList) {
        if (curBlockElement < freeBlk) {
            LIST_INSERT_BEFORE(&freeBlocksList, freeBlk, curBlockElement);
            return;
        } else if(curBlockElement == freeBlk) {
            return;
        } else if (!LIST_NEXT(freeBlk)) {
            LIST_INSERT_TAIL(&freeBlocksList, curBlockElement);
            return;
        }
    }
}

void free_block(void *va) {
    if (!va) return;

    uint32 *curHeader = (uint32*)(va - 4);
    uint32 curBlockSize = get_block_size(va);
    uint32 *curfooter = (uint32 *)(va + (curBlockSize) - 8);


    uint32 *nextHeader = (uint32 *)(va - 4 + curBlockSize);
    uint32 nextSize = get_block_size(nextHeader + 1);
    uint32 *nextFooter = (uint32 *)(va + curBlockSize  + nextSize  - 8);


    uint32 prevSize = *((uint32*)(va - 8)) & (~0x1);
    uint32 *prevHeader = (uint32 *)(va - 4 - prevSize);


    struct BlockElement *nextBlockElement = (struct BlockElement *)(va + curBlockSize);
    struct BlockElement *prevBlockElement = (struct BlockElement *)(va - prevSize);
    struct BlockElement *curBlockElement = (struct BlockElement *)va;
    bool isFreeNext = is_free_block(nextBlockElement);
    bool isFreePrev = is_free_block(prevBlockElement);

    if ((prevSize) == 0) {
        isFreePrev = 0;
    }
    if ((nextSize) == 0) {
        isFreeNext = 0;
    }

    if (!isFreeNext && !isFreePrev) {
        set_block_data(curBlockElement, curBlockSize, 0);
        free_one_block(curBlockElement);
        return;
    } else if (isFreeNext && !isFreePrev) {
        uint32 totalSize = nextSize + curBlockSize;
        set_block_data(curBlockElement, totalSize, 0);

        LIST_REMOVE(&freeBlocksList,nextBlockElement);
        free_one_block(curBlockElement);
        return;
    } else if (!isFreeNext && isFreePrev) {
        uint32 totalSize = prevSize + curBlockSize;
        set_block_data(prevBlockElement, totalSize, 0);

        return;
    } else {
        uint32 totalSize = nextSize + curBlockSize + prevSize;
        set_block_data(prevBlockElement, totalSize, 0);


        LIST_REMOVE(&freeBlocksList,nextBlockElement);

        return;
    }
}



//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size){
	if(va==NULL&&new_size==0)
	    {
	         return NULL;
	     }
	   if(va!=NULL&&new_size==0)
	       {
	           free_block(va);
	           return NULL;
	       }

	     else if(va==NULL&&new_size!=0)
	         {
	             return alloc_block_FF(new_size);
	         }

	       uint32 old_size = get_block_size(va);
	       new_size+= 8;
	       if (new_size == old_size) return va;
	           if (new_size < old_size)
	           {
	               uint32 remaining_size = old_size - new_size;

	               if (remaining_size >= 16)
	               {
	                   set_block_data(va, new_size, 1);

	                   void *new_free_block = (void *)(va + new_size);

	                   set_block_data(new_free_block, remaining_size, 0);

	                   free_block((struct BlockElement *)new_free_block);
	               }

	               return va;
	           }

	     struct BlockElement *next_block = (struct BlockElement *)(va + old_size);

	     if (!is_free_block(next_block) || (get_block_size(next_block) < (new_size - old_size))){
	    	 struct BlockElement *c = alloc_block_FF(new_size-8);
	    	 if(!c){
	    		 return NULL;
	    	 } else {
	             memcpy(c, va, old_size-8);
		    	 free_block(va);
	    		 return c;
	    	 }
	     }  else{
	    	         uint32 combined_size = old_size + get_block_size(next_block);
	    	         if (combined_size < new_size) {
	    	             return NULL;
	    	         }

	    	         LIST_REMOVE(&freeBlocksList, next_block);
	    	         set_block_data(va, new_size, 1);

	    	         uint32 remaining_size = combined_size - new_size;

	    	         if (remaining_size >= 16) {
	    	             void *new_free_block = (void *)(va + new_size);
	    	             set_block_data(new_free_block, remaining_size, 0);
	    	             free_block((struct BlockElement *)new_free_block);
	    	         }
	     }

	     return va;

}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
