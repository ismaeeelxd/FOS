#include "kheap.h"
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
//    panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
    start=daStart;
    segment_break=daStart+initSizeToAllocate;
    hard_limit=daLimit;
    struct FrameInfo *ptr_frame_info;
    if (initSizeToAllocate>daLimit-daStart)
    {
        panic("The size you want to allocate exceeds the limit");
        return E_NO_MEM;
    }
    for (uint32 current_page=daStart;current_page<segment_break;current_page+=PAGE_SIZE)
    {
        allocate_frame(&ptr_frame_info);
        map_frame(ptr_page_directory,ptr_frame_info,current_page, PERM_WRITEABLE | PERM_PRESENT);
        ptr_frame_info->vir_add = current_page;
    }
    initialize_dynamic_allocator(start,initSizeToAllocate);
    return 0;
}
//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
// Write your code here, remove the panic and write your code
void* sbrk(int numOfPages)
{

    if (numOfPages>0){
		uint32 size =numOfPages*PAGE_SIZE;
		if ((size+segment_break)<= hard_limit){
			for (uint32 i=segment_break; i<size+segment_break; i+=PAGE_SIZE){
				struct FrameInfo *ptr_frame_info = NULL;
				allocate_frame(&ptr_frame_info);
				map_frame(ptr_page_directory,ptr_frame_info,i, PERM_WRITEABLE | PERM_PRESENT);
				ptr_frame_info->vir_add = i;
			}
			uint32 tempBrk=segment_break;
			segment_break+=size;
			return (void*)tempBrk;
		}
		else
			return (void*)-1;
	}
	else if (numOfPages ==0)
				return (void*)segment_break;
	else
		return (void*)-1;
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
		return (void*)-1 ;
	//====================================================


}
int arr[1024] = {0};
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

	    int numPagesNeeded=ROUNDUP((uint32)size,PAGE_SIZE)/PAGE_SIZE;

	    uint32 start= hard_limit + PAGE_SIZE;

		if(KERNEL_HEAP_MAX <= start + size){
			return NULL;
		}

		int numFreePages =0;
		uint32 firstpage_alloced=0;
		for(uint32 i = start;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE){
//			LOG_STRING("infinite loop??");
			uint32* ptr_page = NULL;
			if(!get_frame_info(ptr_page_directory,i,&ptr_page)){
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
		if(numPagesNeeded > LIST_SIZE(&MemFrameLists.free_frame_list))
			{
				return NULL;
			}

		int cnt =0;
	    while(numPagesNeeded){
//			LOG_STRING("infinite while loop??");

			uint32* ptr_page = NULL;
	    	struct FrameInfo* frameToBeAlloc = NULL;
	    	if(!get_frame_info(ptr_page_directory,firstpage_alloced+PAGE_SIZE*cnt,&ptr_page)){
	    		allocate_frame(&frameToBeAlloc);
	    		map_frame(ptr_page_directory,frameToBeAlloc,firstpage_alloced+(PAGE_SIZE*cnt), PERM_WRITEABLE | PERM_PRESENT);
	    		frameToBeAlloc->vir_add=(uint32)(firstpage_alloced+(PAGE_SIZE*cnt));

	    		if(ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE == numPagesNeeded)
	    			arr[(firstpage_alloced-KERNEL_HEAP_START)/PAGE_SIZE]=numPagesNeeded;

	    		numPagesNeeded--;
	    	}
	    	++cnt;

	    }
		if(firstpage_alloced==0){
			return NULL;
		}
	   	    return (void*)(firstpage_alloced);

}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree

	uint32 add=(uint32)virtual_address;
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	if(add <KERNEL_HEAP_MAX&&add>=hard_limit+PAGE_SIZE){
		add=ROUNDDOWN(add,PAGE_SIZE);

		for(int i=0 ;i<arr[(add-KERNEL_HEAP_START)/PAGE_SIZE];i++){
			uint32 pa=kheap_physical_address((uint32)virtual_address);
			 struct FrameInfo* frame_info=	to_frame_info(pa);

			unmap_frame(ptr_page_directory,add+PAGE_SIZE*i);
			frame_info->vir_add=0;


		}
        arr[(add - KERNEL_HEAP_START) / PAGE_SIZE] = 0;



	}
	else if(add>=KERNEL_HEAP_START&&add<hard_limit){
		free_block(virtual_address);
		return ;

	}else{
		panic("error");

	}

}


uint32 kheap_physical_address(uint32 virtual_address) {
    uint32* page_table;
    struct FrameInfo* frame_info;

    get_page_table(ptr_page_directory, virtual_address, &page_table);
    if (page_table == NULL) return 0;

    frame_info = get_frame_info(ptr_page_directory, virtual_address,&page_table);
    if (frame_info == NULL) return 0;
    return to_physical_address(frame_info) | PGOFF(virtual_address);
}



uint32 kheap_virtual_address(uint32 physical_address) {
	 struct FrameInfo* frame_info=to_frame_info(physical_address);
	 if(frame_info==NULL){
//		 cprintf("frame is null case\n");
		 return 0;
	  }

	 uint32 frame_number = physical_address >> 12;
	 if(frame_info->references==0|| frame_info->vir_add == 0)
	 {
//		 cprintf("frame is weird case\n");
//		 cprintf("PA: %p\n",physical_address);
//		 cprintf("Current va: %d\n",frame_info->vir_add);
//		 cprintf("Current frame number: %d\n",frame_number);
//		 cprintf("References: %d\n",frame_info->references);

		 return 0;
	 }
    return  frame_info->vir_add + PGOFF(physical_address);
}













//#define MAX_FRAMES (total_physical_memory / PAGE_SIZE)
//uint32 frame_to_va[MAX_FRAMES]; // Reverse mapping table
//
//void init_reverse_mapping() {
//    for (int i = 0; i < MAX_FRAMES; i++) {
//        frame_to_va[i] = 0; // 0 indicates no mapping
//    }
//}
//void update_reverse_mapping(uint32 physical_address, uint32 virtual_address) {
//    uint32 frame_no = PPN(physical_address);
//    frame_to_va[frame_no] = virtual_address;
//}
//
//void clear_reverse_mapping(uint32 physical_address) {
//    uint32 frame_no = PPN(physical_address);
//    frame_to_va[frame_no] = 0; // Clear the mapping
//}

//}
//#define MAX_FRAMES (total_physical_memory / PAGE_SIZE)
//uint32 frame_to_va[MAX_FRAMES]; // Reverse mapping table
//
//void init_reverse_mapping() {
//    for (int i = 0; i < MAX_FRAMES; i++) {
//        frame_to_va[i] = 0; // 0 indicates no mapping
//    }
//}
//void update_reverse_mapping(uint32 physical_address, uint32 virtual_address) {
//    uint32 frame_no = PPN(physical_address);
//    frame_to_va[frame_no] = virtual_address;
//}
//
//void clear_reverse_mapping(uint32 physical_address) {
//    uint32 frame_no = PPN(physical_address);
//    frame_to_va[frame_no] = 0; // Clear the mapping
//}

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
