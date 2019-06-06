#pragma once
//
// CRenderer defines
//
typedef bool(__thiscall* lpDrawScene)(void*);

//struct CRenderer {
#define RENDERER_DATA					\
    /* 0x0	*/ BYTE offset0[0x24];		\
    /* 0x24 */ ULONG width;				\
    /* 0x28 */ ULONG height;			\
    /* 0x2C */ BYTE offset2[0x18];		\
    /* 0x44 */ int fps;
//};

#define DRAWSCENE_FN	0x0043FA20
#define RENDERER_PTR	*reinterpret_cast<DWORD*>(0x00DA2068)

//
// CSession defines
//
typedef int(__thiscall* lpGetTalkType)(void*, void*, int, int);
typedef void(__thiscall* lpRecalcAveragePingTime)(void*, unsigned long);

#define PGETTALKTYPE_FN int __fastcall proxyGetTalkType(void* this_obj, DWORD EDX, void* a2, int a3, int a4)

//struct CSession {
#define SESSION_DATA                      	\
    /* +0x0	  */ BYTE offset0[0x630]; 		\
    /* +0x630  */ ULONG average_ping_time; 	\
    /* +0x634  */ BYTE offset1[0xE3C];     	\
    /* +0x1470 */ ULONG aid;               	\
    /* +0x1474 */ ULONG gid;               	\
    /* +0x1478 */ BYTE offset2[0x8];       	\
    /* +0x1480 */ int job;                 	\
	/* +ox1484 */ BYTE offset3[0x4];       	\
    /* +0x1488 */ int exp;                 	\
	/* +0x148C */ BYTE offset4[0x4];       	\
    /* +0x1490 */ int next_exp;            	\
	/* +0x1494 */ BYTE offset5[0x4];		\
	/* +0x1498 */ int jobnextexp;			\
	/* +0x149C */ BYTE offset6[0x4];		\
	/* +0x14A0 */ int jobexp;				\
	/* +0x14A4 */ BYTE offset7[0x4];		\
	/* +0x14A8 */ int level;				\
	/* +0x14AC */ BYTE offset8[0x4];		\
    /* +0x14B0 */ int joblevel;            	\
    /* +0x14B4 */ int skillPoints;          
//};

#define GETTALKTYPE_FN				0x00A0CF40
#define RECALCAVERAGEPINGTIME_FN	0x00A1F510
#define SESSION_PTR					0x010130C8
