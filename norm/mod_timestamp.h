#pragma once
#include "mod.h"
class timestamp :
	public norm_dll::mod
{
public:
	timestamp(norm_dll::norm* c_state);
	virtual ~timestamp();
	void send_msg(void**, int*, void**, void**, int*, int*);
	void send_msg(void**, int*, int*, int*, int*, int*);
	int get_talk_type(void**, void**, int*, int*, int*);
	int get_talk_type(void**, char**, int*, char**, int*);

private:
	int enabled = 1;
	char msg_buf[256];
};

