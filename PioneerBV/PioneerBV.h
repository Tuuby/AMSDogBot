// PioneerBV.h : Haupt-Header-Datei für die Anwendung PIONEERBV
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
#include "PioneerBVDlg.h"						// Hinzugefügt von der Klassenansicht

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVApp:
// Siehe PioneerBV.cpp für die Implementierung dieser Klasse
//

class CPioneerBVApp : public CWinApp
{
public:
	CPioneerBVDlg* GetDialog();
	CPioneerBVApp();
	~CPioneerBVApp();
	//LPSTR  com;
	HANDLE shared_memory;



// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPioneerBVApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CPioneerBVApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPioneerBVDlg* m_dlg;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PIONEERBV_H__421EAAC5_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_)
