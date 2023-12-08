uint32 sys_env_set_nice( int nice_value)
{
	env_set_nice(curenv,nice_value);
	return 0;
}
