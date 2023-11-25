uint32 sys_get_hard_limit(){
	return syscall(SYS_GET_LIMIT,0,0,0,0,0);
}
