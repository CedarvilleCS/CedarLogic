//***********************************************************************************************************************************
// Pedro Casanova (casanova@ujaen.es) 2020/04-10
// Debug Macros
//***********************************************************************************************************************************
#define _MSG_
//#define _SAVE_
#ifdef _MSG_
#define DBGID "CL_Msg "
#define DBGLEN sizeof(DBGID)-1
#define _MSG(cad,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);OutputDebugString(szDebStr);}
#define _MSG0(cad) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad);OutputDebugString(szDebStr);}
#define _MSG1(cad,p1) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1);OutputDebugString(szDebStr);}
#define _MSG2(cad,p1,p2) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2);OutputDebugString(szDebStr);}
#define _MSG3(cad,p1,p2,p3) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2, p3);OutputDebugString(szDebStr);}
#define _MSG4(cad,p1,p2,p3,p4) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2, p3, p4);OutputDebugString(szDebStr);}
#define _MSG5(cad,p1,p2,p3,p4,p5) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2, p3, p4, p5);OutputDebugString(szDebStr);}
#define _MSG6(cad,p1,p2,p3,p4,p5,p6) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2, p3, p4, p5, p6);OutputDebugString(szDebStr);}
#define _MSGC0(cond,cad) \
	{if (cond) _MSG0(cad)}
#define _MSGC1(cond,cad,p1) \
	{if (cond) _MSG1(cad,p1)}
#define _MSGC2(cond,cad,p1,p2) \
	{if (cond) _MSG2(cad,p1,p2}
#define _MSGC3(cond,cad,p1,p2,p3) \
	{if (cond) _MSG3(cad,p1,p2,p3)}
#define _MSGC4(cond,cad,p1,p2,p3,p4) \
	{if (cond) _MSG4(cad,p1,p2,p3,p4)}
#define _MSGC5(cond,cad,p1,p2,p3,p4,p5) \
	{if (cond) _MSG5(cad,p1,p2,p3,p4,p5)}
#define _MSGC(cond,cad,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) \
	{if (cond) _MSG(cad,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)}
#ifdef _SAVE_
#define _SAVEMSG(cad,p1,p2) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN, cad, p1, p2);OutputDebugString(szDebStr);}
#else
#define _SAVEMSG(cad,p1,p2)
#endif
#else
#define _MSG(cad,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define _MSG0(cad)
#define _MSG1(cad,p1)
#define _MSG2(cad,p1,p2)
#define _MSG3(cad,p1,p2,p3)
#define _MSG4(cad,p1,p2,p3,p4)
#define _MSG5(cad,p1,p2,p3,p4,p5)
#define _MSG6(cad,p1,p2,p3,p4,p5,p6)
#define _MSGC(cond,cad,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define _MSGC0(cond,cad)
#define _MSGC1(cond,cad,p1)
#define _MSGC2(cond,cad,p1,p2)
#define _MSGC3(cond,cad,p1,p2,p3)
#define _MSGC4(ccond,ad,p1,p2,p3,p4)
#define _MSGC5(cad,p1,p2,p3,p4,p5)
#define _SAVEMSG(cad,p1,p2)
#endif
//***********************************************************************************************************************************