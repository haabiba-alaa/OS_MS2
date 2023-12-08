/*2023*/
/********* for BSD Priority Scheduler *************/
int env_get_nice(struct Env* e) ;
void env_set_nice(struct Env* e, int nice_value) ;
int env_get_recent_cpu(struct Env* e) ;
int get_load_average() ;