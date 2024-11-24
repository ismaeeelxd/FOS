#include "kheap.h"
#include "memory_manager.h"

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC


uint32 free_page_count;
struct FreePage free_pages[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / (PAGE_SIZE)];  // The array to track free pages

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

    free_page_count = 1;
    free_pages[0].starting_addr = hard_limit+PAGE_SIZE;
    free_pages[0].numOfPagesFreeAfter = ((KERNEL_HEAP_MAX - (hard_limit+PAGE_SIZE)) / (PAGE_SIZE)) - 1;
    dir = ptr_page_directory;
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
int arr[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / (PAGE_SIZE)]={0};

void* kmalloc_with_dir(uint32 size,uint32* directory){
	dir = directory;
	void* s = kmalloc(size);
	dir = ptr_page_directory;
	return s;
}
void* kmalloc(unsigned int size) {
    // Ensure First-Fit strategy is used
    if (!isKHeapPlacementStrategyFIRSTFIT()) {
        return NULL;
    }

    // Determine the number of pages needed
    if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
        return alloc_block_FF(size);
     }
    int numPagesNeeded = (ROUNDUP(size, PAGE_SIZE)) / PAGE_SIZE;

	if(KERNEL_HEAP_MAX-PAGE_SIZE+1 <= (hard_limit+PAGE_SIZE) + size){
		return NULL;
	}

    // Look for a suitable free block in the free_pages array
    for (uint32 i = 0; i < free_page_count; i++) {
        if (free_pages[i].numOfPagesFreeAfter + 1 >= numPagesNeeded) {
            // Found a suitable block
            uint32 first_page_allocated = free_pages[i].starting_addr;

            // Allocate pages from the block
            for (int j = 0; j < numPagesNeeded; j++) {
                struct FrameInfo* frameToBeAlloc = NULL;
                allocate_frame(&frameToBeAlloc);
                map_frame(dir, frameToBeAlloc,first_page_allocated + (j * PAGE_SIZE),
                          PERM_WRITEABLE | PERM_PRESENT);
                frameToBeAlloc->vir_add = first_page_allocated+(j*PAGE_SIZE);
	    		if(ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE == numPagesNeeded)
	    			arr[(first_page_allocated-KERNEL_HEAP_START)/PAGE_SIZE]=numPagesNeeded;            }

            // Update the free_pages array
            if (free_pages[i].numOfPagesFreeAfter + 1 == numPagesNeeded) {
                // Entire block is consumed, remove it
                for (uint32 j = i; j < free_page_count - 1; j++) {
                    free_pages[j] = free_pages[j + 1];
                }

                free_page_count--;
            } else {
                // Partially allocate, adjust the block
                free_pages[i].starting_addr += numPagesNeeded * PAGE_SIZE;
                free_pages[i].numOfPagesFreeAfter -= numPagesNeeded;
            }

            // Return the allocated virtual address
            return (void*)first_page_allocated;
        }
    }

    // If no suitable block is found, return NULL
    return NULL;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree

	uint32 add=(uint32)virtual_address;

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	if(add <KERNEL_HEAP_MAX&&add>=hard_limit+PAGE_SIZE){
		add=ROUNDDOWN(add,PAGE_SIZE);
		add_free_block(add,arr[(add-KERNEL_HEAP_START)/PAGE_SIZE]);

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





void add_free_block(uint32 addr, uint32 num_pages) {
    // Validate input
    if (num_pages == 0 || addr < KERNEL_HEAP_START || addr >= KERNEL_HEAP_MAX) {
        return;
    }

    // Find insertion point
    uint32 i;
    for (i = 0; i < free_page_count; i++) {
        if (addr < free_pages[i].starting_addr) {
            break;
        }
    }

    // Step 1: Merge with previous block if possible
    if (i > 0 && free_pages[i - 1].starting_addr +
        (free_pages[i - 1].numOfPagesFreeAfter + 1) * PAGE_SIZE == addr) {
        free_pages[i - 1].numOfPagesFreeAfter += num_pages;

        // Step 2: Check if the new extended block can merge with the next block
        if (i < free_page_count &&
            addr + num_pages * PAGE_SIZE == free_pages[i].starting_addr) {
            free_pages[i - 1].numOfPagesFreeAfter += free_pages[i].numOfPagesFreeAfter + 1;

            // Shift remaining entries to remove the merged next block
            for (uint32 j = i; j < free_page_count - 1; j++) {
                free_pages[j] = free_pages[j + 1];
            }
            free_page_count--;
        }
        return;
    }

    // Step 3: Merge with next block if possible
    if (i < free_page_count &&
        addr + num_pages * PAGE_SIZE == free_pages[i].starting_addr) {
        free_pages[i].starting_addr = addr;
        free_pages[i].numOfPagesFreeAfter += num_pages;
        return;
    }

    // Step 4: Insert as a new block
    if (i < free_page_count) {
        // Shift entries to make space for the new block
        for (uint32 j = free_page_count; j > i; j--) {
            free_pages[j] = free_pages[j - 1];
        }
    }

    // Add the new block
    free_pages[i].starting_addr = addr;
    free_pages[i].numOfPagesFreeAfter = num_pages - 1;
    free_page_count++;
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
