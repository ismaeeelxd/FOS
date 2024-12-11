#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("create_frames_storage is not implemented yet");
	//Your Code is Here...
	 if (numOfFrames <= 0) {
		        return NULL;
		    }
		void* va=kmalloc(PAGE_SIZE);
		if(va==NULL)
			return NULL;
	    struct FrameInfo** framesStorage = (struct FrameInfo**)va;
	    for (int i = 0; i < numOfFrames; i++) {
	            framesStorage[i] = 0;
	        }
	    return va;

}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("create_share is not implemented yet");
	//Your Code is Here...
	struct Share* element;
		void* va=kmalloc(PAGE_SIZE);
		element = (struct Share*)va;
		if (element == NULL) {
		    return NULL;
		}

		element->ID =(int32) ((uint32)element & 0x7FFFFFFF);  // not sure va or element
		element->isWritable = isWritable;
		element->ownerID = ownerID;
		element->references = 1;
		element->size = (int)size;
		strcpy(element->name,shareName);
		element->framesStorage = create_frames_storage(ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE);
		if (element->framesStorage == NULL) {
		    kfree(element);
		    return NULL;
		}

		return element;

}

//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("get_share is not implemented yet");
	//Your Code is Here...
	if (name == NULL) {
		        return NULL;
		    }
		struct Share* res=NULL;

		struct Share* temp=NULL;
		LIST_FOREACH(temp,&(AllShares.shares_list)){
		if( strcmp(name, temp->name)==0 && temp->ownerID==ownerID){

			 res=temp;
			 break;
		}
	}
		return res;

}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("createSharedObject is not implemented yet");
	//Your Code is Here...
	struct Env* myenv = get_cpu_proc(); //The calling environment
	   acquire_spinlock(&(AllShares.shareslock));
	   struct Share* checkIfFound= get_share(ownerID,shareName);
	   if (checkIfFound != NULL){
			release_spinlock(&(AllShares.shareslock));

		   return E_SHARED_MEM_EXISTS ;
	   }

		struct Share* NewCreate = create_share(ownerID, shareName, size, isWritable);

	  if (NewCreate == NULL){
		  	release_spinlock(&(AllShares.shareslock));
		   return E_NO_SHARE;
	  }

	  LIST_INSERT_TAIL(&(AllShares.shares_list),NewCreate);

	  uint32 numPages= (ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE);

	  for (int i=0;i<numPages;i++){

		 struct FrameInfo* ptr_frame_info=NULL;
		 allocate_frame(&ptr_frame_info);
		 map_frame(myenv->env_page_directory,ptr_frame_info,(uint32)(virtual_address+i*PAGE_SIZE), PERM_WRITEABLE | PERM_USER | PERM_PRESENT | PERM_USED | PERM_AVAILABLE);
		 NewCreate->framesStorage[i] = ptr_frame_info;
	    }
	  release_spinlock(&(AllShares.shareslock));

	  return NewCreate->ID ;
}


//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("getSharedObject is not implemented yet");
	//Your Code is Here...
	struct Env* myenv = get_cpu_proc(); //The calling environment
		struct Share* s=get_share(ownerID,shareName);

			if(s->ID==E_SHARED_MEM_NOT_EXISTS)
			{
				return E_SHARED_MEM_NOT_EXISTS;
			}
			int num_of_pages=ROUNDUP(s->size,PAGE_SIZE)/PAGE_SIZE;
			uint32 *page_directory=myenv->env_page_directory;
			uint32 *ptr_page_table ;
			for(int i=0;i<num_of_pages;i++)
			{


				 struct FrameInfo* sa= s->framesStorage[i];

				 if(s->isWritable)
				 {

					 map_frame(page_directory,sa,(uint32)(virtual_address+(i*PAGE_SIZE)),PERM_PRESENT|PERM_USER|PERM_WRITEABLE);
				 }
				 else
				 {

					 map_frame(page_directory,sa,(uint32)(virtual_address+(i*PAGE_SIZE)),PERM_PRESENT|PERM_USER);
				 }
			}
			s->references++;
			return s->ID;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
void free_share(struct Share* ptrShare)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("free_share is not implemented yet");
	//Your Code is Here...
	if (ptrShare == NULL)
	{
	   return;
    }
//	 cprintf("free share test 1\n");

//	 uint32 num_of_pages = ROUNDUP(ptrShare->size, PAGE_SIZE) / PAGE_SIZE;
//	 for (uint32 i = 0; i < num_of_pages; i++)
//	  {
//	    if (ptrShare->framesStorage[i]->references>0)
//	     {
//	    		free_frame(ptrShare->framesStorage[i]);
//	    		ptrShare->framesStorage[i]=NULL;
//	     }
//
//	  }
//	   cprintf("99\n");

 // Free_framesStorage_array
	 cprintf("Removing shared object %s\n",ptrShare->name);
	 kfree((void*)(ptrShare->framesStorage));

 // Remove_share
	acquire_spinlock(&(AllShares.shareslock));

     LIST_REMOVE(&(AllShares.shares_list), ptrShare);

     release_spinlock(&(AllShares.shareslock));

 // Free_object
	 kfree((void*)ptrShare);
	tlbflush();



}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("freeSharedObject is not implemented yet");
	//Your Code is Here...
//	if(sharedObjectID==E_SHARED_MEM_NOT_EXISTS)
//		return 0;
		struct Share* targetShare = NULL;
		struct Share *res=NULL;
	    // Get shared object

		acquire_spinlock(&(AllShares.shareslock));
		 LIST_FOREACH(targetShare, &(AllShares.shares_list))
		 {
		   if (targetShare->ID == sharedObjectID)
		   {
			   	   cprintf("Found target share Id: %d\n",targetShare->ID);
			   	   cprintf("Object name is %s\n",targetShare->name);
			   	   res=targetShare;
		            break;
		   }
		 }
	     release_spinlock(&(AllShares.shareslock));


		 if (res == NULL)
		 {
			 return E_SHARED_MEM_NOT_EXISTS;
		 }
		 struct Env* myenv = get_cpu_proc(); // The calling environment
		 uint32* page_directory = myenv->env_page_directory;
		 uint32 num_of_pages = res->size/PAGE_SIZE;
		 for (uint32 i = 0; i < num_of_pages; i++)
		 {
			 	 uint32* page_table=NULL;
		         void* sa = (void*)((uint32)startVA + (i * PAGE_SIZE));
//		         cprintf("Freeing va: %p\n",sa);
		         unmap_frame(page_directory, (uint32)sa);
		         get_page_table(page_directory,(uint32)sa,&page_table);
		         bool isEmpty=1;

		         if(page_table){
					 pt_clear_page_table_entry(page_directory, (uint32)sa);

		        	 for(int z=0;z<1024;z++)
		        	 {
		        		 uint32 *pt = NULL;
		        		 uint32 perms = page_table[z] & 0x00000FFF;
						 if(((perms & PERM_AVAILABLE) && (perms& PERM_PRESENT)))
						 {
							 isEmpty=0;
							 break;
						 }
					}

						 if(isEmpty==1){
							 cprintf("PAGE TABLE OF %p is EMPTY\n",sa);
							 cprintf("FREEING PAGE TABLE ADDRESS: %p\n",page_table);
							 pd_clear_page_dir_entry(page_directory, (uint32)sa);
							 kfree((void*)page_table);

		        	 }

		         }

		 }

		 res->references--;

		 if (res->references == 0)
		 {
			 	free_share(res);
		        return 0;
		 }

		 tlbflush();
		 return 0;


}
//==========================
// [B3] Get Shared ID:
//==========================
// Return the ID of the shared object
int32 getSharedid(void* virtual_address)
{
	struct Env* myenv = get_cpu_proc(); //The calling environment
	 uint32* page_directory = myenv->env_page_directory;
	 uint32* page_table = NULL;
	 struct FrameInfo* frame_info = get_frame_info(page_directory, (uint32)virtual_address, &page_table);
	 struct Share* tempShare = NULL;
	    uint32 va = (uint32)virtual_address;
//		acquire_spinlock(&(AllShares.shareslock));

	    LIST_FOREACH(tempShare, &(AllShares.shares_list))
	    {
	        uint32 num_of_pages = tempShare->size / PAGE_SIZE;
	        for (uint32 i = 0; i < num_of_pages; i++)
	        {
	            if (tempShare->framesStorage[i] != NULL)
	            {
	                if(frame_info== tempShare->framesStorage[i])
	                {
	                	return tempShare->ID;
	                }
	            }
	        }
	    }
//	     release_spinlock(&(AllShares.shareslock));


	    return E_SHARED_MEM_NOT_EXISTS;
}
