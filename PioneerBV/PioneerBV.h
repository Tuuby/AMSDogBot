// PioneerBV.h : Haupt-Header-Datei f�r die Anwendung PIONEERBV
//

#if !defined(AFX_PIONEERBV_H__421EAAC5_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_)
#define AFX_PIONEERBV_H__421EAAC5_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"							// Hauptsymbole
#include "PioneerBVDlg.h"						// Hinzugef�gt von der Klassenansicht

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVApp:
// Siehe PioneerBV.cpp f�r die Implementierung dieser Klasse
//

class CPioneerBVApp : public CWinApp
{
public:
	CPioneerBVDlg* GetDialog();
	CPioneerBVApp();
	~CPioneerBVApp();
	//LPSTR  com;
	HANDLE shared_memory;



// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPioneerBVApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CPioneerBVApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPioneerBVDlg* m_dlg;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_PIONEERBV_H__421EAAC5_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_)
