// PioneerBV.cpp : Legt das Klassenverhalten f�r die Anwendung fest.
//

#include "stdafx.h"
#include "PioneerBV.h"
#include "PioneerBVDlg.h"
//#include "shared_mem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVApp

BEGIN_MESSAGE_MAP(CPioneerBVApp, CWinApp)
	//{{AFX_MSG_MAP(CPioneerBVApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVApp Konstruktion

CPioneerBVApp::CPioneerBVApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einf�gen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}


/////////////////////////////////////////////////////////////////////////////
// Das einzige CPioneerBVApp-Objekt

CPioneerBVApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVApp Initialisierung

BOOL CPioneerBVApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Gr��e Ihrer fertigen 
	//  ausf�hrbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht ben�tigen, entfernen.

	m_dlg = new CPioneerBVDlg();
	m_pMainWnd = m_dlg;
	int nResponse = m_dlg->DoModal();
	if( nResponse == IDOK )
	{
		// ZU ERLEDIGEN: F�gen Sie hier Code ein, um ein Schlie�en des
		//  Dialogfelds �ber OK zu steuern
	}
	else if( nResponse == IDCANCEL )
	{
		// ZU ERLEDIGEN: F�gen Sie hier Code ein, um ein Schlie�en des
		//  Dialogfelds �ber "Abbrechen" zu steuern
	}

	delete m_dlg;
	// Da das Dialogfeld geschlossen wurde, FALSE zur�ckliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}


CPioneerBVDlg* CPioneerBVApp::GetDialog()
{
	return m_dlg;
}


CPioneerBVApp::~CPioneerBVApp()
{
	//SM_Close();
}
