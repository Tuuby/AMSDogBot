#ifndef _thread_h
#define _thread_h

#include "PioneerBVDlg.h"

UINT WorkerThread( LPVOID pParam );
void OnButton( CPioneerBVDlg *pDlg, int iButtonIndex );
void OnButtonOwn( CPioneerBVDlg *pDlg, int iButtonIndex );
void InitButtons();
	
#endif