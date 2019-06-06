#pragma once

//
// CRenderer defines
//
typedef bool(__thiscall* lpDrawScene)(void*);

//struct CRenderer {
#define RENDERER_DATA				\
    /* 0x0	*/ BYTE offset0[0x24];	\
    /* 0x24 */ ULONG width;			\
    /* 0x28 */ ULONG height;		\
    /* 0x2C */ BYTE offset2[0x18];	\
    /* 0x44 */ int fps;
//};

#define DRAWSCENE_FN 0x00423040
#define RENDERER_PTR *reinterpret_cast<DWORD*>(0x00ba9208)

//
// CSession defines
//
typedef signed int(__thiscall* lpGetTalkType)(void*, char*, int, char*);
typedef void(__thiscall* lpRecalcAveragePingTime)(void*, unsigned long);

#define PGETTALKTYPE_FN int __fastcall proxyGetTalkType(void* this_obj, DWORD EDX, char* a2, int a3, char* a4)

//struct CSession {
#define SESSION_DATA						\
	/* 0x0	  */	BYTE offset0[0x634];	\
	/* 0x634  */	ULONG average_ping_time;\
	/* 0x638  */	BYTE offset1[0xBCC];	\
	/* 0x1204 */	ULONG aid;				\
	/* 0x1208 */	ULONG gid;				\
	/* 0x120C */	BYTE offset2[0x8];		\
	/* 0x1214 */	int job;				\
	/* 0x1218 */	int exp;				\
	/* 0x121C */	int level;				\
	/* 0x1220 */	int point;				\
	/* 0x1224 */	int next_exp;			\
	/* 0x1228 */	int joblevel;			\
	/* 0x122C */	int skillPoints;			\
	/* 0x1230 */	BYTE offset3[0xA0];		\
	/* 0x12D0 */	int jobnextexp;			\
	/* 0x12D4 */	int jobexp;				
	//+5780
//};

#define GETTALKTYPE_FN				0x00925100
#define RECALCAVERAGEPINGTIME_FN	0x00935560
#define SESSION_PTR					0x00E0EE28
