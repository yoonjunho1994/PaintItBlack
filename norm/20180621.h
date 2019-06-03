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

#define PGETTALKTYPE_FN int __fastcall pGetTalkType(void* this_obj, DWORD EDX, void* a2, int a3, int a4)

//struct CSession {
#define SESSION_DATA                      \
    /* 0x0	  */ BYTE offset0[0x630]; \
    /* 0x630  */ ULONG average_ping_time; \
    /* 0x634  */ BYTE offset1[0xE3C];     \
    /* 0x1470 */ ULONG aid;               \
    /* 0x1XXX */ ULONG gid;               \
    /* 0x1XXX */ BYTE offset2[0x8];       \
    /* 0x1XXX */ int job;                 \ // correct until here
				 BYTE offset3[0x4];       \
    /* 0x1XXX */ int exp;                 \
				 BYTE offset4[0x4];       \
    /* 0x1XXX */ int level;               \ //better search with ghidra, sucks to do it by hand
    /* 0x1XXX */ int point;               \
    /* 0x1XXX */ int next_exp;            \
    /* 0x1XXX */ int joblevel;            \
    /* 0x1XXX */ int skillPoint;          \
    /* 0x1XXX */ BYTE offset5[0xA0];      \
    /* 0x1XXX */ int jobnextexp;          \
    /* 0x1XXX */ int jobexp;
	//+5780
//};

#define GETTALKTYPE_FN				0x00A0CF40
#define RECALCAVERAGEPINGTIME_FN	0x00A1F510
#define SESSION_PTR					0x010130C8