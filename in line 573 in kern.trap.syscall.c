//TODO: [PROJECT'23.MS1 - #4] [2] SYSTEM CALLS - Add suitable code here

	case SYS_ENV_SET_NICE:
		 sys_env_set_nice(a1);
		 return 0;
		 break;