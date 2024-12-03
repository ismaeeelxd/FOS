#include <inc/lib.h>

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
int arr[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE]={0};

void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}
//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
//	panic("malloc() is not implemented yet...!!");
//	return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy
	cprintf("malloc is called\n");
	  if(!sys_isUHeapPlacementStrategyFIRSTFIT())
	        return NULL;
	     if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
	         return alloc_block_FF(size);
	    }
	          int pages_3yzhom=ROUNDUP((uint32)size,PAGE_SIZE)/PAGE_SIZE;
	          if(!myEnv){
	              return NULL;
	          }
	        uint32 start= myEnv->limit + PAGE_SIZE;

	        if(USER_HEAP_MAX <= start + size){
	            return NULL;
	        }

	        int numfreepages =0;
	        uint32 firstpagealloced=0;

	        for(uint32 i = start;i<USER_HEAP_MAX;i+=PAGE_SIZE){
	            if(!arr[(i - USER_HEAP_START)/PAGE_SIZE]){
	                    if(numfreepages==0)
	                    	firstpagealloced=i;
	                    numfreepages++;
	                if(numfreepages == pages_3yzhom){
	                    break;
	                }
	            }else{

	            	numfreepages =0;
	            	firstpagealloced=0;
	            }
	        }

	        if( numfreepages < pages_3yzhom){
	            return NULL;
	        }

	        if(numfreepages >= pages_3yzhom && firstpagealloced != 0){
	            sys_allocate_user_mem(firstpagealloced,size);

	            for(int i = 0;i<pages_3yzhom;++i){
	                if(i==0)
	                {
	                    arr[((firstpagealloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=pages_3yzhom;
	                    continue;
	                }
	                arr[((firstpagealloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=-2;

	            }
	            return (void*)(firstpagealloced);
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
//	panic("free() is not implemented yet...!!");
	  uint32 address=(uint32)virtual_address;
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
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
//	panic("smalloc() is not implemented yet...!!");
//	cprintf("smalloc is called\n");

    if( USER_HEAP_MAX <= myEnv->limit+PAGE_SIZE + size)
            return NULL;

    uint32 pages3ahzhom=ROUNDUP((uint32)size,PAGE_SIZE)/PAGE_SIZE;

    int FreePages =0;
        uint32 firstpage_alloced=0;
        uint32 start=myEnv-> limit+PAGE_SIZE;
            for(uint32 i = start;i<USER_HEAP_MAX;i+=PAGE_SIZE){
                if(!arr[(i - USER_HEAP_START)/PAGE_SIZE]){
                        if(FreePages==0)
                            firstpage_alloced=i;
                        FreePages++;
                    if(FreePages == pages3ahzhom){
                        break;
                    }
                }else{

                	FreePages =0;
                     firstpage_alloced=0;
                }
            }

            if( FreePages < pages3ahzhom){
                return NULL;
            }

            if(FreePages >= pages3ahzhom && firstpage_alloced != 0){
                uint32 ret = sys_createSharedObject(sharedVarName,size,isWritable,(void*)firstpage_alloced);
                if(ret == E_SHARED_MEM_EXISTS ||ret == E_NO_SHARE){
                    return NULL;
                }

                for(int i = 0;i<pages3ahzhom;++i){
                    if(i==0)
                    {
                        arr[((firstpage_alloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=pages3ahzhom;
                        continue;
                    }
                    arr[((firstpage_alloced+PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE]=-2;

                }
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
	int size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);
			if (size == E_SHARED_MEM_NOT_EXISTS)
				return NULL;

			if (USER_HEAP_MAX <= myEnv->limit + PAGE_SIZE + size)
				return NULL;

			uint32 pages3ahzom = ROUNDUP((uint32)size, PAGE_SIZE)/PAGE_SIZE;

			int numFreePages = 0;
			uint32 firstpage_alloced = 0;
			uint32 start = myEnv->limit + PAGE_SIZE;

			for(uint32 i = start; i < USER_HEAP_MAX; i += PAGE_SIZE) {
				if(!arr[(i - USER_HEAP_START)/PAGE_SIZE]) {

					if(numFreePages == 0)
						firstpage_alloced = i;
					numFreePages++;
					if(numFreePages == pages3ahzom) {

						break;
					}
				} else {

					numFreePages = 0;
					firstpage_alloced = 0;
				}
			}

			if(numFreePages < pages3ahzom) {
				return NULL;
			}

			if(numFreePages >= pages3ahzom && firstpage_alloced != 0) {
				int ret = sys_getSharedObject(ownerEnvID, sharedVarName, (void*)firstpage_alloced);
				if(ret >= 0) {
					for(int i = 0; i < pages3ahzom; ++i) {

						if(i == 0) {
							arr[((firstpage_alloced + PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE] = pages3ahzom;
							continue;
						}
						arr[((firstpage_alloced + PAGE_SIZE*i) - USER_HEAP_START)/PAGE_SIZE] = -2;
					}
					return (void*)firstpage_alloced;
				}
			}

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
