#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"
struct {
	struct spinlock clk;
//	struct spinlock dynclk;
	struct FreePage free_pages[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / (PAGE_SIZE)];
	uint32 free_page_count;
	int arr[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / (PAGE_SIZE)];

} Alloc;


void lock_init()
{
	init_spinlock(&Alloc.clk, "mylk");
}

void add_free_block(uint32 addr, uint32 num_pages) {

    if (num_pages == 0 || addr < KERNEL_HEAP_START || addr >= KERNEL_HEAP_MAX) {
        return;
    }


    uint32 i;
    for (i = 0; i < Alloc.free_page_count; i++) {
        if (addr < Alloc.free_pages[i].starting_addr) {
            break;
        }
    }


    if (i > 0 && Alloc.free_pages[i - 1].starting_addr +
        (Alloc.free_pages[i - 1].numOfPagesFreeAfter + 1) * PAGE_SIZE == addr) {
        Alloc.free_pages[i - 1].numOfPagesFreeAfter += num_pages;


        if (i < Alloc.free_page_count &&
            addr + num_pages * PAGE_SIZE == Alloc.free_pages[i].starting_addr) {
            Alloc.free_pages[i - 1].numOfPagesFreeAfter += Alloc.free_pages[i].numOfPagesFreeAfter + 1;

            for (uint32 j = i; j < Alloc.free_page_count - 1; j++) {
            	Alloc.free_pages[j] = Alloc.free_pages[j + 1];
            }
            Alloc.free_page_count--;
        }
        return;
    }


    if (i < Alloc.free_page_count &&
        addr + num_pages * PAGE_SIZE == Alloc.free_pages[i].starting_addr) {
    	Alloc.free_pages[i].starting_addr = addr;
    	Alloc.free_pages[i].numOfPagesFreeAfter += num_pages;
        return;
    }


    if (i < Alloc.free_page_count) {
        // Shift entries to make space for the new block
        for (uint32 j = Alloc.free_page_count; j > i; j--) {
        	Alloc.free_pages[j] = Alloc.free_pages[j - 1];
        }
    }


    Alloc.free_pages[i].starting_addr = addr;
    Alloc.free_pages[i].numOfPagesFreeAfter = num_pages - 1;
    Alloc.free_page_count++;
}




//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code
//	panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
	lock_init();
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

	    Alloc.free_page_count = 1;
	    Alloc.free_pages[0].starting_addr = hard_limit+PAGE_SIZE;
	    Alloc.free_pages[0].numOfPagesFreeAfter = ((KERNEL_HEAP_MAX - (hard_limit+PAGE_SIZE)) / (PAGE_SIZE)) - 1;
	    dir = ptr_page_directory;
	    initialize_dynamic_allocator(start,initSizeToAllocate);
	    return 0;
}

void* sbrk(int numOfPages)
{
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
//	return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code
//	panic("sbrk() is not implemented yet...!!");
	 if (numOfPages>0){
			uint32 size =numOfPages*PAGE_SIZE;
			if ((size+segment_break)<= hard_limit){
//				 cprintf("Lock is aquired sbrk\n");

				for (uint32 i=segment_break; i<size+segment_break; i+=PAGE_SIZE){
					struct FrameInfo *ptr_frame_info = NULL;
					allocate_frame(&ptr_frame_info);
					map_frame(ptr_page_directory,ptr_frame_info,i, PERM_WRITEABLE | PERM_PRESENT);
					ptr_frame_info->vir_add = i;
				}
				uint32 tempBrk=segment_break;
				segment_break+=size;
//				 cprintf("Lock is released sbrk\n");


				return (void*)tempBrk;
			}
			else
			{

				return (void*)-1;

		}}
		else if (numOfPages ==0){

					return (void*)segment_break;
		}else
			return (void*)-1;
}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator
void* kmalloc_with_dir(uint32 size,uint32* directory){
	dir = directory;
	void* s = kmalloc(size);
	dir = ptr_page_directory;
	return s;
}
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
	// Write your code here, remove the panic and write your code
//	kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	   if (!isKHeapPlacementStrategyFIRSTFIT()) {
	        return NULL;
	    }
//	   cprintf("kmalloc is called\n");
	    if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
	    	acquire_spinlock(&Alloc.clk);
	    	void* a = alloc_block_FF(size);
	    	release_spinlock(&Alloc.clk);
	        return a;
	     }
	    int numPagesNeeded = (ROUNDUP(size, PAGE_SIZE)) / PAGE_SIZE;

		if(KERNEL_HEAP_MAX-PAGE_SIZE <= (hard_limit+PAGE_SIZE) + size){
//		 	cprintf("Lock is released kmalloc a2al mn ely msh 3aref eh\n");
			return NULL;
		}
		acquire_spinlock(&Alloc.clk);

	    for (uint32 i = 0; i < Alloc.free_page_count; i++) {
	        if (Alloc.free_pages[i].numOfPagesFreeAfter + 1 >= numPagesNeeded) {
	            uint32 first_page_allocated = Alloc.free_pages[i].starting_addr;
	            for (int j = 0; j < numPagesNeeded; j++) {
	                struct FrameInfo* frameToBeAlloc = NULL;
	                allocate_frame(&frameToBeAlloc);
	                map_frame(dir, frameToBeAlloc,first_page_allocated + (j * PAGE_SIZE),
	                          PERM_WRITEABLE | PERM_PRESENT);
	                frameToBeAlloc->vir_add = first_page_allocated+(j*PAGE_SIZE);
		    		if(ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE == numPagesNeeded)
		    			Alloc.arr[(first_page_allocated-KERNEL_HEAP_START)/PAGE_SIZE]=numPagesNeeded;            }
	            if (Alloc.free_pages[i].numOfPagesFreeAfter + 1 == numPagesNeeded) {
	                for (uint32 j = i; j < Alloc.free_page_count - 1; j++) {
	                	Alloc.free_pages[j] = Alloc.free_pages[j + 1];
	                }

	                Alloc. free_page_count--;
	            } else {
	            	Alloc.free_pages[i].starting_addr += numPagesNeeded * PAGE_SIZE;
	            	Alloc.free_pages[i].numOfPagesFreeAfter -= numPagesNeeded;
	            }
//	     	   cprintf("Lock is released kmalloc 1\n");

				release_spinlock(&Alloc.clk);
	            return (void*)first_page_allocated;
	        }
	    }
//  	   cprintf("Lock is released kmalloc 2\n");

		release_spinlock(&Alloc.clk);

	    return NULL;
	}


void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
//	panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	acquire_spinlock(&Alloc.clk);
	uint32 add=(uint32)virtual_address;

	if(add <KERNEL_HEAP_MAX&&add>=hard_limit+PAGE_SIZE){
			add=ROUNDDOWN(add,PAGE_SIZE);
			add_free_block(add,Alloc.arr[(add-KERNEL_HEAP_START)/PAGE_SIZE]);

			for(int i=0 ;i<Alloc.arr[(add-KERNEL_HEAP_START)/PAGE_SIZE];i++){
				uint32 pa=kheap_physical_address((uint32)virtual_address);
				 struct FrameInfo* frame_info=	to_frame_info(pa);

				unmap_frame(ptr_page_directory,add+PAGE_SIZE*i);
				frame_info->vir_add=0;

			}
			Alloc.arr[(add - KERNEL_HEAP_START) / PAGE_SIZE] = 0;

		}
		else if(add>=KERNEL_HEAP_START&&add<hard_limit){
			free_block(virtual_address);
			release_spinlock(&Alloc.clk);
			return ;

		}else{
			panic("error");
		}

	release_spinlock(&Alloc.clk);

}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
//	panic("kheap_physical_address() is not implemented yet...!!");
	 uint32* page_table;
	    struct FrameInfo* frame_info;

	    get_page_table(ptr_page_directory, virtual_address, &page_table);
	    if (page_table == NULL) return 0;

	    frame_info = get_frame_info(ptr_page_directory, virtual_address,&page_table);
	    if (frame_info == NULL) return 0;
	    return to_physical_address(frame_info) | PGOFF(virtual_address);

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//	panic("kheap_virtual_address() is not implemented yet...!!");
	 struct FrameInfo* frame_info=to_frame_info(physical_address);
		 if(frame_info==NULL){
			 return 0;
		  }

		 uint32 frame_number = physical_address >> 12;
		 if(frame_info->references==0|| frame_info->vir_add == 0)
		 {
			 return 0;
		 }
	    return  frame_info->vir_add + PGOFF(physical_address);


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

uint32 searchforpages(int num_free_pages,uint32 first_free_page, uint32 num_pages_to_allocate){

     for (uint32 i = hard_limit+PAGE_SIZE; i < KERNEL_HEAP_MAX; i += PAGE_SIZE) {
                uint32 *ptr_page = NULL;
                if (!get_frame_info(ptr_page_directory, i, &ptr_page)) {
                    if (num_free_pages == 0) {
                        first_free_page = i;
                    }
                    num_free_pages++;
                    if (num_free_pages == num_pages_to_allocate) {
                        return first_free_page;
                        break;
                    }
                } else {
                    num_free_pages = 0;  // Reset if pages are not free
                    first_free_page = 0;
                }
            }
     return first_free_page;
}

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	virtual_address = ROUNDDOWN(virtual_address,PAGE_SIZE);
	new_size = ROUNDUP(new_size,PAGE_SIZE);
	 if (virtual_address == NULL) {
	        return kmalloc(new_size);
	    }

	    if (new_size == 0) {
	        kfree(virtual_address);
	        return NULL;
	    }
	    if((uint32)virtual_address >=KERNEL_HEAP_START &&  (uint32)virtual_address< hard_limit )
	    	{
	    		if(new_size<=DYN_ALLOC_MAX_BLOCK_SIZE)
	    		{

	    			return realloc_block_FF(virtual_address,  new_size);
	    	    }
	    		else
	    		{
	    			void* address= kmalloc(new_size);
	    			if( address != NULL)
	    			{
	    			 free_block(virtual_address);
	    			 return address;
	    			}
	    				else {
	    				return NULL;
	    				}
	    		}

	    	}

	    int index = ((uint32)virtual_address - KERNEL_HEAP_START) / PAGE_SIZE;
	    if ((uint32)virtual_address <= KERNEL_HEAP_MAX && (uint32)virtual_address >= hard_limit + PAGE_SIZE) {
	        if (new_size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
	            void *address = kmalloc(new_size);
	            if (address != NULL) {
	                kfree(virtual_address);
	                return address;
	            }

	            return NULL;
	        }
	        else{
	        if (Alloc.arr[index] ==new_size/PAGE_SIZE ) {

	               return virtual_address;
	           }
	        else  if (Alloc.arr[index] > new_size/PAGE_SIZE ) {

	        	int newidx = ((uint32)virtual_address+ new_size - KERNEL_HEAP_START) / PAGE_SIZE;
	        	Alloc.arr[newidx] = Alloc.arr[index] - new_size / PAGE_SIZE;
	        	Alloc.arr[index] = new_size/PAGE_SIZE;
	        	kfree(virtual_address + new_size);
	            return virtual_address;
	        }
	        uint32 t = Alloc.arr[index];
	        kfree(virtual_address);
	        uint32 num_pages_to_allocate = ROUNDUP(new_size,PAGE_SIZE)/PAGE_SIZE;
	        uint32 num_free_pages = 0;
	        uint32 first_free_page = 0;

	        for (uint32 i = hard_limit+PAGE_SIZE; i < KERNEL_HEAP_MAX; i += PAGE_SIZE) {
	        	first_free_page=searchforpages(0,first_free_page,num_pages_to_allocate);
	        }

	        if (num_free_pages < num_pages_to_allocate || num_pages_to_allocate > LIST_SIZE(&MemFrameLists.free_frame_list)) {
	            return NULL;
	        }
	        int cnt = 0;
	        while (num_pages_to_allocate > 0) {
	            uint32 *ptr_page = NULL;
	            struct FrameInfo *frame_to_alloc = NULL;
	            if (!get_frame_info(ptr_page_directory, first_free_page + PAGE_SIZE * cnt, &ptr_page)) {
	                allocate_frame(&frame_to_alloc);
	                map_frame(ptr_page_directory, frame_to_alloc, first_free_page + PAGE_SIZE * cnt, PERM_WRITEABLE | PERM_PRESENT);
	                Alloc.arr[index] = num_pages_to_allocate +t;  // Update the array with new page count
	                num_pages_to_allocate--;
	            }
	            ++cnt;
	        }

	        return (void *)first_free_page;  // Return new address
	    }
	    }
	    return NULL;
	}
