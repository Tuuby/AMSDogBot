// PioneerBV.cpp : Legt das Klassenverhalten für die Anwendung fest.
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
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVApp Konstruktion

CPioneerBVApp::CPioneerBVApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
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
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

	m_dlg = new CPioneerBVDlg();
	m_pMainWnd = m_dlg;
	int nResponse = m_dlg->DoModal();
	if( nResponse == IDOK )
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über OK zu steuern
	}
	else if( nResponse == IDCANCEL )
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	delete m_dlg;
	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
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
