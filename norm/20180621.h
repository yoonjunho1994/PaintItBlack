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
typedef signed int(__thiscall* lpGetTalkType)(void*, char*, int, char*);
typedef void(__thiscall* lpRecalcAveragePingTime)(void*, unsigned long);

//struct CSession {
#define SESSION_DATA					\
    BYTE offset0[0x44];					\
    ULONG aid;							\
    ULONG gid;							\
    BYTE offset1[0x44];					\
    int job;							\
    int exp;							\
    int level;							\
    BYTE offset2[0x44];					\
    int next_exp;						\
    int joblevel;						\
    BYTE offset3[0x44];					\
    int jobnextexp;						\
    int jobexp;							\
    /* 0x134 */ BYTE offset4[0x4FC];	\
    /* 0x630 */ ULONG average_ping_time;
//};

#define GETTALKTYPE_FN				0x00A0CF40
#define RECALCAVERAGEPINGTIME_FN	0x00A1F510
#define SESSION_PTR					0x010130C8