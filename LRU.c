if (isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX)){
			uint32 ActiveCurrSize=LIST_SIZE(&curenv->ActiveList);
			uint32 SecondCurrSize=LIST_SIZE(&curenv->SecondList);
			if((ActiveCurrSize+SecondCurrSize) < (curenv->ActiveListSize+curenv->SecondListSize)){
				if(ActiveCurrSize < curenv->ActiveListSize){//Case 1 AC < Max
					cprintf("LRU: ActiveCurrSize:%d ,AC MAX size: %d\n",ActiveCurrSize,curenv->ActiveListSize);
					struct FrameInfo *ptr_frameInfo = NULL ;
					int alloc = allocate_frame(&ptr_frameInfo);
					if(alloc == 0){  //!= E_NO_MEM
						map_frame(curenv->env_page_directory ,ptr_frameInfo , fault_va,(PERM_PRESENT | PERM_USER | PERM_WRITEABLE));
						alloc = pf_read_env_page(curenv,(void *)fault_va);
						if (alloc == E_PAGE_NOT_EXIST_IN_PF){
						 // CHECK if it is a stack or heap page

							if ((fault_va < USTACKTOP && fault_va >= USTACKBOTTOM) || (fault_va < USER_HEAP_MAX  && fault_va >= USER_HEAP_START)){
								//pf_add_empty_env_page(curenv,fault_va,0);
							}
							else{
								//panic("invaild access %x",fault_va);
								sched_kill_env(curenv->env_id);
							}
						}

					}
					struct WorkingSetElement* ws_new_ele = env_page_ws_list_create_element( curenv, fault_va);
					LIST_INSERT_HEAD(&curenv->ActiveList,ws_new_ele);
					pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
					return;
				}
				else{//Case 2 Ac==Max
					//case 2.A Present in Sec
					struct WorkingSetElement *ptr_WS_element = NULL;
					LIST_FOREACH(ptr_WS_element, &(curenv->SecondList)){
						if(ROUNDDOWN(ptr_WS_element->virtual_address,PAGE_SIZE) == ROUNDDOWN(fault_va,PAGE_SIZE))
						{

							struct WorkingSetElement * temp_element = LIST_LAST(&curenv->ActiveList);
							LIST_REMOVE(&curenv->ActiveList,temp_element);
							LIST_INSERT_HEAD(&curenv->SecondList,temp_element);
							pt_set_page_permissions(curenv->env_page_directory,temp_element->virtual_address,0,PERM_PRESENT);
							LIST_REMOVE(&curenv->SecondList,ptr_WS_element);
							LIST_INSERT_HEAD(&curenv->ActiveList,ptr_WS_element);
							pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
							return;// for now

						}
				   }
					//Case 2.B NOT found in Sec
					struct FrameInfo *ptr_frameInfo = NULL ;
					int alloc = allocate_frame(&ptr_frameInfo);
					if(alloc == 0){  //!= E_NO_MEM
						map_frame(curenv->env_page_directory ,ptr_frameInfo , fault_va,(PERM_PRESENT | PERM_USER | PERM_WRITEABLE));
						alloc = pf_read_env_page(curenv,(void *)fault_va);
						if (alloc == E_PAGE_NOT_EXIST_IN_PF){
						 // CHECK if it is a stack or heap page

							if ((fault_va < USTACKTOP && fault_va >= USTACKBOTTOM) || (fault_va < USER_HEAP_MAX  && fault_va >= USER_HEAP_START)){
								//pf_add_empty_env_page(curenv,fault_va,0);
							}
							else{
								//panic("invaild access %x",fault_va);
								sched_kill_env(curenv->env_id);
							}
						}

					}
					struct WorkingSetElement* ws_new_ele = env_page_ws_list_create_element( curenv, fault_va);
					struct WorkingSetElement * temp_element = LIST_LAST(&curenv->ActiveList);
					LIST_REMOVE(&(curenv->ActiveList),temp_element);
					LIST_INSERT_HEAD(&(curenv->SecondList),temp_element);
					pt_set_page_permissions(curenv->env_page_directory,temp_element->virtual_address,0,PERM_PRESENT);
					LIST_INSERT_HEAD(&(curenv->ActiveList),ws_new_ele);
					pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
					return;

			   }
			}
			else{
				//Replacemet LRU
				struct WorkingSetElement *ptr_WS_element = NULL;
				//cprintf("LRU: Replacement LRU\n");
				LIST_FOREACH(ptr_WS_element, &(curenv->SecondList)){
					if(ROUNDDOWN(ptr_WS_element->virtual_address,PAGE_SIZE) == ROUNDDOWN(fault_va,PAGE_SIZE))
					{
						struct WorkingSetElement * temp_element = LIST_LAST(&curenv->ActiveList);
						LIST_REMOVE(&curenv->ActiveList,temp_element);
						LIST_REMOVE(&curenv->SecondList,ptr_WS_element);
						LIST_INSERT_HEAD(&curenv->ActiveList,ptr_WS_element);
						pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
						LIST_INSERT_HEAD(&curenv->SecondList,temp_element);
						pt_set_page_permissions(curenv->env_page_directory,temp_element->virtual_address,0,PERM_PRESENT);
						return;// for now
					}
				}
				struct WorkingSetElement* ws_new_ele = env_page_ws_list_create_element( curenv, fault_va);
				//victim element and write it on disk
				struct WorkingSetElement * vic_element = LIST_LAST(&curenv->SecondList);
				struct WorkingSetElement * temp_element = LIST_LAST(&curenv->ActiveList);
				//write code of writing on the disk
				struct FrameInfo *ptr_frameInfo = NULL ;
				int alloc = allocate_frame(&ptr_frameInfo);
				if(alloc == 0){  //!= E_NO_MEM
					map_frame(curenv->env_page_directory ,ptr_frameInfo , fault_va,(PERM_PRESENT | PERM_USER | PERM_WRITEABLE));
					alloc = pf_read_env_page(curenv,(void *)fault_va);
					if (alloc == E_PAGE_NOT_EXIST_IN_PF){
					 // CHECK if it is a stack or heap page

						if ((fault_va < USTACKTOP && fault_va >= USTACKBOTTOM) || (fault_va < USER_HEAP_MAX  && fault_va >= USER_HEAP_START)){
							//pf_add_empty_env_page(curenv,fault_va,0);
						}
						else{
							//panic("invaild access %x",fault_va);
							sched_kill_env(curenv->env_id);
						}
					}

				}
				uint32 page_permissions=pt_get_page_permissions(curenv->env_page_directory,vic_element->virtual_address);
				if(page_permissions & PERM_MODIFIED){
					uint32* ptr_page_table=NULL;
					get_page_table(curenv->env_page_directory,vic_element->virtual_address,&ptr_page_table);
					struct FrameInfo *ptr_frameInfo = get_frame_info(curenv->env_page_directory,vic_element->virtual_address,&ptr_page_table);
					pf_update_env_page(curenv,vic_element->virtual_address,ptr_frameInfo);

				}

				unmap_frame(curenv->env_page_directory,vic_element->virtual_address);
				LIST_REMOVE(&curenv->SecondList,vic_element);
				pt_set_page_permissions(curenv->env_page_directory,vic_element->virtual_address,0,PERM_PRESENT);
				LIST_REMOVE(&curenv->ActiveList,temp_element);
				LIST_INSERT_HEAD(&curenv->SecondList,temp_element);
				pt_set_page_permissions(curenv->env_page_directory,temp_element->virtual_address,0,PERM_PRESENT);
				LIST_INSERT_HEAD(&curenv->ActiveList,ws_new_ele);
				pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
				return;
			}
	}
