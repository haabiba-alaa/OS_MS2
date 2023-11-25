
uint32 user_heap_pages[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE] = {0};
//struct user_heap_info alloced_pages[((USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE)];
//int idx = 0;
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'23.MS2 - #09] [2] USER HEAP - malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy
	if(size<=DYN_ALLOC_MAX_SIZE){
		return alloc_block_FF(size);
	}
	    uint32 user_Available_Pages = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE;
	    size = ROUNDUP(size, PAGE_SIZE);
	    uint32 requiredPages=size/PAGE_SIZE;
	    uint32 found = 0;
	    uint32 start_of_first_fit_block = USER_HEAP_MAX;
	    uint32 startIndex = ((USER_LIMIT+PAGE_SIZE)-USER_HEAP_START)/PAGE_SIZE;

	    if (user_Available_Pages >= requiredPages && sys_isUHeapPlacementStrategyFIRSTFIT())
	    {
	        for (uint32 i = startIndex; i < user_Available_Pages; i++)
	        {
	            if (user_heap_pages[i] == 0)
	            {
	                uint32 j = i + 1;
	                while (j < user_Available_Pages && user_heap_pages[j] == 0)
	                {
	                    j++;
	                }
	                if (j - i >= requiredPages)
	                {
	                    start_of_first_fit_block = USER_HEAP_START + i * PAGE_SIZE;
	                    found = 1;
	                    break; // Stop searching after finding the first fit block
	                }
	                i = j - 1; // Skip to the last checked block
	            }
	        }


	        if (found==0)
	        {
	        	cprintf("Malloc: Not enough consecutive free pages");
	            return NULL;
	        }

	        sys_allocate_user_mem(start_of_first_fit_block, (size * PAGE_SIZE));

	        for (uint32 i = 0; i < requiredPages; i++)
	        {
	            user_heap_pages[(start_of_first_fit_block - USER_HEAP_START) / PAGE_SIZE + i] = requiredPages;
	        }

//	        alloced_pages[idx].address = start_of_first_fit_block;
//	        alloced_pages[idx].size = size * PAGE_SIZE;
//	        idx++;
	    }
	    else
	    {
	    	cprintf("Malloc: no enough pages\n");
	        return NULL;
	    }

	    return (void*)start_of_first_fit_block;


}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #11] [2] USER HEAP - free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	uint32 va = (uint32)virtual_address;
	if(va>=USER_HEAP_START && va<=USER_LIMIT){
		free_block(virtual_address);
		return;
	}

	uint32 page_address = ROUNDDOWN(va, PAGE_SIZE);
	uint32 startIndex=(page_address-USER_HEAP_START)/PAGE_SIZE;

			if (user_heap_pages[startIndex]!=0)
			{
				uint32 num_pages = user_heap_pages[startIndex];


				for (uint32 j = 0; j < num_pages; j++)
				{
					user_heap_pages[j] = 0;
				}

				sys_free_user_mem(va, num_pages * PAGE_SIZE);


//				alloced_pages[i] = alloced_pages[idx - 1];
//				idx--;

				return;

		}

}
