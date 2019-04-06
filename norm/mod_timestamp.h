#pragma once
#include "mod.h"
class timestamp :
	public norm_dll::mod
{
public:
	timestamp(norm_dll::norm* c_state);
	virtual ~timestamp();
#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621) 
	void send_msg(void**, int*, void**, void**, int*, int*);
#elif CLIENT_VER == 20150000
	void send_msg(void**, int*, int*, int*, int*, int*);
#endif
	int get_talk_type(void**, void**, int*, int*, int*);
	int get_talk_type(void**, char**, int*, char**, int*);
	//void register_hooks();

private:
	int enabled = 1;
	char msg_buf[256];
};

