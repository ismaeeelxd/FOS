#include <inc/lib.h>

//#include "memory_manager.h"
//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
int arr[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE]={0};
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
//	panic("malloc() is not implemented yet...!!");

	if(!sys_isUHeapPlacementStrategyFIRSTFIT())
		return NULL;
	 if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
		 return alloc_block_FF(size);
	}
	 	 int numPagesNeeded=ROUNDUP((uint32)size,PAGE_SIZE)/PAGE_SIZE;
	 	 if(!myEnv){
	 		 return NULL;
	 	 }
	    uint32 start= myEnv->limit + PAGE_SIZE;

		if(USER_HEAP_MAX <= start + size){
			return NULL;
		}

		int numFreePages =0;
		uint32 firstpage_alloced=0;
		if(numPagesNeeded > sys_calculate_free_frames())
			{
				return NULL;
			}
		for(uint32 i = start;i<USER_HEAP_MAX;i+=PAGE_SIZE){
			if(!arr[(i-USER_HEAP_START)/PAGE_SIZE]){
					if(numFreePages==0)
						firstpage_alloced=i;
				numFreePages++;
				if(numFreePages == numPagesNeeded){
					break;
				}
			}else{

				 numFreePages =0;
				 firstpage_alloced=0;
			}
		}

		if( numFreePages < numPagesNeeded){
			return NULL;
		}

		int cnt =0;
	    while(numPagesNeeded){
//			LOG_STRING("infinite while loop??");

			uint32* ptr_page = NULL;
	    	struct FrameInfo* frameToBeAlloc = NULL;
	    	if(!arr[(firstpage_alloced+PAGE_SIZE*cnt-USER_HEAP_START)/PAGE_SIZE]){
	    		sys_allocate_user_mem(firstpage_alloced+PAGE_SIZE*cnt,numPagesNeeded*PAGE_SIZE);
//	    		frameToBeAlloc->vir_add=(uint32)(firstpage_alloced+(PAGE_SIZE*cnt));

	    		if(ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE == numPagesNeeded)
	    			arr[(firstpage_alloced-USER_HEAP_START)/PAGE_SIZE]=numPagesNeeded;

	    		numPagesNeeded--;
	    	}
	    	++cnt;

	    }
		if(firstpage_alloced==0){
			return NULL;
		}
	   	    return (void*)(firstpage_alloced);


	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	panic("free() is not implemented yet...!!");
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
