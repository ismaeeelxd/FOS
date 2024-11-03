#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
    //[PROJECT'24.MS2] [USER HEAP - KERNEL SIDE] initialize_kheap_dynamic_allocator
    // Write your code here, remove the panic and write your code
//    panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
    start=daStart;
    segment_break=daStart+initSizeToAllocate;
    hard_limit=daLimit;
    struct FrameInfo *ptr_frame_info;
    cprintf("Start address : %p \n",start);
    cprintf("hard limit address : %p \n",hard_limit);

    if (initSizeToAllocate>daLimit-daStart)
    {
        panic("The size you want to allocate exceeds the limit");
        return E_NO_MEM;
    }
    for (uint32 current_page=daStart;current_page<segment_break;current_page+=PAGE_SIZE)
    {

//        create_page_table(&ptr_page_directory, current_page);
        allocate_frame(&ptr_frame_info);
        map_frame(ptr_page_directory,ptr_frame_info,current_page, PERM_WRITEABLE | PERM_PRESENT);
    }

    initialize_dynamic_allocator(start,initSizeToAllocate);
    return 0;
}
//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
// Write your code here, remove the panic and write your code
void* sbrk(int numOfPages)
{
	struct FrameInfo *ptr_frame_info;

    if (numOfPages>0){
		uint32 size =numOfPages*PAGE_SIZE;
		if ((size+segment_break)<= hard_limit){
			for (uint32 i=segment_break; i<=size+segment_break; i+=PAGE_SIZE){
				allocate_frame(&ptr_frame_info);
			 map_frame(ptr_page_directory,ptr_frame_info,i, PERM_WRITEABLE | PERM_PRESENT);
			}
			uint32 tempBrk=segment_break;
			segment_break+=size;
			return tempBrk;
		}
		else
			return -1;
	}
	else if (numOfPages ==0)
				return segment_break;
	else
		return -1;
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
	//panic("sbrk() is not implemented yet...!!");
	//return (void*)-1 ;
	//====================================================


}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
		if(!isKHeapPlacementStrategyFIRSTFIT()){
	        return NULL;
	    }

	    if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
	        return alloc_block_FF(size);
	     }

	    int numPagesNeeded=ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;

	    uint32 start= hard_limit + PAGE_SIZE;

		if(KERNEL_HEAP_MAX <= start + size){
			return NULL;
		}

		int numFreePages =0;
		for(int i = start;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE){
			uint32* ptr_page = NULL;
			if(!get_frame_info(ptr_page_directory,i,&ptr_page)){
				numFreePages++;
				if(numFreePages == numPagesNeeded){
					break;
				}
			}
		}

		if( numFreePages < numPagesNeeded){
			return NULL;
		}

		int cnt =0;
		int firstPageAlloc = -1;
	    while(numPagesNeeded){
	    	struct FrameInfo* frameToBeAlloc;
	    	uint32* ptr_page = NULL;
	    	if(!get_frame_info(ptr_page_directory,start+PAGE_SIZE*cnt,&ptr_page)){
	    		allocate_frame(&frameToBeAlloc);
	    		map_frame(ptr_page_directory,frameToBeAlloc,start+PAGE_SIZE*cnt, PERM_WRITEABLE | PERM_PRESENT);
	    		if(ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE == numPagesNeeded){
	    			firstPageAlloc = start+PAGE_SIZE*cnt;
	    		}
	    		numPagesNeeded--;
	    	}
	    	++cnt;
	    }
	    return (void*)(firstPageAlloc);
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
