// Thread.h: thread main procs.
// Last Revision: y2008 m01 d29
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED


BOOL StartThread(LPTHREAD_START_ROUTINE lpRoutine, LPVOID lpParam, LPHANDLE lpEvent, LPHANDLE lpThread, LPDWORD lpThreadId);
DWORD WINAPI StopThreadProc(LPVOID lpParam);
void ShutdownMainThread(CTcpcom32Dlg* lpTcpcom32Dlg);
DWORD WINAPI InitializeThreadProc(LPVOID lpParam);
DWORD WINAPI AcceptThreadProc(LPVOID lpParam);
DWORD WINAPI ClientThreadProc(LPVOID lpParam);
DWORD WINAPI PortThreadProc(LPVOID lpParam);
DWORD WINAPI StopThreadProc(LPVOID lpParam);


#endif // THREAD_H_INCLUDED

