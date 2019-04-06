#pragma once
#include "mod.h"

class graphics :
	public norm_dll::mod
{
public:
	graphics(norm_dll::norm* c_state);
	virtual ~graphics();
	HRESULT WaitForVerticalBlank(DWORD*, HANDLE*);
	int get_talk_type(void**, void**, int*, int*, int*);
	int get_talk_type(void**, char**, int*, char**, int*);

private:
	bool vsync_active = false; //default: disable vsync
};
