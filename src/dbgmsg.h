//***********************************************************************************************************************************
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Debug Macros
// Use with DEBUGVIEW (Sysinternals)
//***********************************************************************************************************************************
#ifndef DBGMSG_H
#define DBGMSG_H


// Uncomment to use
//#define _MSG_
#ifdef _MSG_
#define _MSGGUI_
#define _MSGCOM_
#define _MSGW_
#define DBGID "CL_Msg "
#define DBGLEN sizeof(DBGID)-1
#define _MSG(cad,...) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN,cad,##__VA_ARGS__);OutputDebugString(szDebStr);}
#define _MSGC(cond,cad,...) \
	{if (cond) _MSG(cad,##__VA_ARGS__)}
#define _MSGNC(cond,cad,...) \
	{_MSGC(!cond,cad,##__VA_ARGS__)}
#ifdef _MSGGUI_
#define _MSGGUI(cad,...) \
	{_MSG(cad,##__VA_ARGS__)}
#else
#define _MSGGUI(cad,...)
#endif
#ifdef _MSGCOM_
#define _MSGCOM(cad,...) \
	{_MSG(cad,##__VA_ARGS__)}
#else
#define _MSGCOM(cad,...)
#endif
#ifdef _MSGW_
#define _MSGW(cad,...) \
	{_MSG(cad,##__VA_ARGS__)}
#else
#define _MSGW(cad,...)
#endif
#else
#define _MSG(cad,...)
#define _MSGC(cond,cad,...)
#define _MSGNC(cond,cad,...)
#define _MSGGUI(cad,...)
#define _MSGW(cad,...)
#define _MSGCOM(cad,...)
#endif



#endif // DBGMSG_H
//***********************************************************************************************************************************