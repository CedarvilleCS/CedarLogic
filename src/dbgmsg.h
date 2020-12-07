//***********************************************************************************************************************************
// Pedro Casanova (casanova@ujaen.es) 2020/04-11
// Debug Macros
//***********************************************************************************************************************************
#ifndef DBGMSG_H
#define DBGMSG_H


// Uncomment to use
#define _MSG_
#ifdef _MSG_
#define DBGID "CL_Msg "
#define DBGLEN sizeof(DBGID)-1
#define _MSG(cad,...) \
	{char szDebStr[1024]=DBGID;sprintf(szDebStr+DBGLEN,cad,##__VA_ARGS__);OutputDebugString(szDebStr);}
#define _MSGC(cond,cad,...) \
	{if (cond) _MSG(cad,##__VA_ARGS__)}
#define _MSGNC(cond,cad,...) \
	{_MSGC(!cond,cad,##__VA_ARGS__)}
#else
#define _MSG(cad,...)
#define _MSGC(cond,cad,...)
#define _MSGNC(cond,cad,...)
#endif

#endif // DBGMSG_H
//***********************************************************************************************************************************