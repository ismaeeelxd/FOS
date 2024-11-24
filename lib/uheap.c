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
//	    cprintf("start at %p\n",start);
//	    cprintf("user heap max at %p\n",USER_HEAP_MAX);
		if(USER_HEAP_MAX <= start + size){
			return NULL;
		}

		int numFreePages =0;
		uint32 firstpage_alloced=0;

		for(uint32 i = start;i<USER_HEAP_MAX;i+=PAGE_SIZE){
			if(!arr[(i - USER_HEAP_START)/PAGE_SIZE]){
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

		if(numFreePages >= numPagesNeeded && firstpage_alloced != 0){
			sys_allocate_user_mem(firstpage_alloced,size);

			for(int i = 0;i<numPagesNeeded;++i){
				if(i==0)
				{
					arr[((firstpage_alloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=numPagesNeeded;
					continue;
				}
				arr[((firstpage_alloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=1;

			}
//			cprintf("First page allocated: %p\n",firstpage_alloced);
			return (void*)(firstpage_alloced);
		}
		return NULL;
}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	uint32 address=(uint32)virtual_address;
				//page Alloc
				if(address>=(myEnv->limit+ PAGE_SIZE)&&address<USER_HEAP_MAX )
				{
					address=ROUNDDOWN(address,PAGE_SIZE);
					int c=arr[(address-USER_HEAP_START)/PAGE_SIZE];
					uint32 total_size=(uint32)(c*PAGE_SIZE);
					int r=(address-USER_HEAP_START)/PAGE_SIZE;
					sys_free_user_mem(address,total_size);
					while(c--)
					{
						arr[r]=0;
						r++;
					}
				}
				//Block alloc
				else if(address>=USER_HEAP_START&&address<myEnv->limit)
				{
					free_block(virtual_address);
				}
				else
				{
					panic("invalid address!!");
				}
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
struct FreePage free_pages[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE];
int arr2[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE]={0};

uint32 free_page_count;
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
//	panic("smalloc() is not implemented yet...!!");
	if( USER_HEAP_MAX <= myEnv->limit+PAGE_SIZE + size)
			return NULL;

//	free_page_count = 1;
//	free_pages[0].starting_addr =myEnv-> limit+PAGE_SIZE;
//	free_pages[0].numOfPagesFreeAfter = ((USER_HEAP_MAX - (myEnv-> limit+PAGE_SIZE)) / (PAGE_SIZE)) - 1;
	uint32 no_OFpages_needed=ROUNDUP((uint32)size,PAGE_SIZE)/PAGE_SIZE;
//	        if (free_pages[i].numOfPagesFreeAfter + 1 >= no_OFpages_needed) {
//	            uint32 first_page_allocated = free_pages[i].starting_addr;
//	            sys_createSharedObject(sharedVarName,size,isWritable,(void*)first_page_allocated);
//	            if(free_pages[i].numOfPagesFreeAfter+1==no_OFpages_needed)
//	            {
//	            	for(int j=i;j<free_page_count-1;j++)
//	            	{
//	            		free_pages[j]=free_pages[j+1];
//	            	}
//	            		free_page_count--;
//	            }else{
//	            	free_pages[i].numOfPagesFreeAfter-=no_OFpages_needed;
//	            	free_pages[i].starting_addr += no_OFpages_needed * PAGE_SIZE;
//
//	            }
//	            return (void*)first_page_allocated;
//	        }
//
//
//	return NULL;

	int numFreePages =0;
		uint32 firstpage_alloced=0;
		uint32 start=myEnv-> limit+PAGE_SIZE;
			for(uint32 i = start;i<USER_HEAP_MAX;i+=PAGE_SIZE){
				if(!arr2[(i - USER_HEAP_START)/PAGE_SIZE]){
						if(numFreePages==0)
							firstpage_alloced=i;
					numFreePages++;
					if(numFreePages == no_OFpages_needed){
						break;
					}
				}else{

					 numFreePages =0;
					 firstpage_alloced=0;
				}
			}

			if( numFreePages < no_OFpages_needed){
				return NULL;
			}

			if(numFreePages >= no_OFpages_needed && firstpage_alloced != 0){
				sys_createSharedObject(sharedVarName,size,isWritable,(void*)firstpage_alloced);

				for(int i = 0;i<no_OFpages_needed;++i){
					if(i==0)
					{
						arr2[((firstpage_alloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=no_OFpages_needed;
						continue;
					}
					arr2[((firstpage_alloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=-2;

				}
	//			cprintf("First page allocated: %p\n",firstpage_alloced);
				return (void*)(firstpage_alloced);
			}
			return NULL;

}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
//	panic("sget() is not implemented yet...!!");
//	return NULL;
	//	 struct BlockElement *b = NULL;
	//	    uint32 size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);
		cprintf("1661\n");
		int size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);
		if (size == E_SHARED_MEM_NOT_EXISTS)
			return NULL;

		if (USER_HEAP_MAX <= myEnv->limit + PAGE_SIZE + size)
			return NULL;

		uint32 no_OFpages_needed = ROUNDUP((uint32)size, PAGE_SIZE)/PAGE_SIZE;

		// Search for free space using First Fit
		int numFreePages = 0;
		uint32 firstpage_alloced = 0;
		uint32 start = myEnv->limit + PAGE_SIZE;

		// Find consecutive free pages
		for(uint32 i = start; i < USER_HEAP_MAX; i += PAGE_SIZE) {
			if(!arr2[(i - USER_HEAP_START)/PAGE_SIZE]) {
				if(numFreePages == 0)
					firstpage_alloced = i;
				numFreePages++;
				if(numFreePages == no_OFpages_needed) {
					break;
				}
			} else {
				numFreePages = 0;
				firstpage_alloced = 0;
			}
		}

		// If we don't have enough consecutive free pages
		if(numFreePages < no_OFpages_needed) {
			return NULL;
		}

		// If we found enough space, get the shared object
		if(numFreePages >= no_OFpages_needed && firstpage_alloced != 0) {
			int ret = sys_getSharedObject(ownerEnvID, sharedVarName, (void*)firstpage_alloced);
			if(ret >= 0) {  // Success case - assuming non-negative return means success
				// Mark the pages as used in our tracking array
				for(int i = 0; i < no_OFpages_needed; ++i) {
					if(i == 0) {
						arr2[((firstpage_alloced + PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE] = no_OFpages_needed;
						continue;
					}
					arr2[((firstpage_alloced + PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE] = -2;
				}
				return (void*)firstpage_alloced;
			}
		}

		return NULL;
	//		if(sys_isUHeapPlacementStrategyFIRSTFIT())
	//		{
	//			b=alloc_block_FF(size);
	//			if(b==NULL)
	//			{
	//				return NULL;
	//			}
	//			int obj_id=sys_getSharedObject(ownerEnvID,sharedVarName,b);
	//			if(obj_id==E_SHARED_MEM_NOT_EXISTS)
	//			{
	//				return NULL;
	//			}
	//			else
	//				return b;
	//
	//		}
	//		else
	//			return NULL;
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
