// PioneerBVDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "PioneerBV.h"
#include "PioneerBVDlg.h"
#include "Thread.h"
//#include "shared_mem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern double dThreadFrameCount;


// Ausgabe eines Strings aus dem Thread in die History
void tsay(LPCTSTR s)
{
  CPioneerBVDlg *pDlg;
  pDlg = ((CPioneerBVApp*)AfxGetApp())->GetDialog();
  pDlg->Say(s);
}
// Ausgabe eines Strings und einer int-Zahl 
// aus dem Thread in die History
void tsayi(LPCTSTR s,int i)
{
	TCHAR buf[300];
	_stprintf_s(buf, 300, _T("%s%d"), s, i);
	tsay(buf);
}

// Ausgabe in die History von C-Programmen ausführbar
extern "C" void say(LPCTSTR s) {
  CPioneerBVDlg *pDlg;
  pDlg = ((CPioneerBVApp*)AfxGetApp())->GetDialog();
  pDlg->Say(s);
		
}

volatile bool bEndThread;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD){
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVDlg Dialogfeld

CPioneerBVDlg::CPioneerBVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPioneerBVDlg::IDD, pParent){
	//{{AFX_DATA_INIT(CPioneerBVDlg)
	m_strFrames = _T("");
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bEndThread = false;
	m_pWorkerThread = NULL;
}

void CPioneerBVDlg::DoDataExchange( CDataExchange* pDX ){
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPioneerBVDlg)
	DDX_Control(pDX, IDC_LIST_HISTORY, m_listboxHistory);
	DDX_Control(pDX, IDC_BUTTON_OWN_4, m_buttonOwn4);
	DDX_Control(pDX, IDC_BUTTON_OWN_3, m_buttonOwn3);
	DDX_Control(pDX, IDC_BUTTON_OWN_2, m_buttonOwn2);
	DDX_Control(pDX, IDC_BUTTON_OWN_1, m_buttonOwn1);
	DDX_Control(pDX, IDC_BUTTON_OWN_0, m_buttonOwn0);
	DDX_Control(pDX, IDC_VALUE_5, m_staticValue5);
	DDX_Control(pDX, IDC_VALUE_4, m_staticValue4);
	DDX_Control(pDX, IDC_VALUE_3, m_staticValue3);
	DDX_Control(pDX, IDC_VALUE_2, m_staticValue2);
	DDX_Control(pDX, IDC_VALUE_1, m_staticValue1);
	DDX_Control(pDX, IDC_VALUE_0, m_staticValue0);
	DDX_Control(pDX, IDC_DESCRIPTION_5, m_staticDesc5);
	DDX_Control(pDX, IDC_DESCRIPTION_4, m_staticDesc4);
	DDX_Control(pDX, IDC_DESCRIPTION_3, m_staticDesc3);
	DDX_Control(pDX, IDC_DESCRIPTION_2, m_staticDesc2);
	DDX_Control(pDX, IDC_DESCRIPTION_1, m_staticDesc1);
	DDX_Control(pDX, IDC_DESCRIPTION_0, m_staticDesc0);
	DDX_Text(pDX, IDC_STATIC_FRAMECOUNT, m_strFrames);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPioneerBVDlg, CDialog)
	ON_MESSAGE( WM_WORKER_THREAD_FINISHED, OnWorkerThreadFinished )
	//{{AFX_MSG_MAP(CPioneerBVDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_VIDEO_FORMAT, OnVideoFormat)
	ON_BN_CLICKED(IDC_VIDEO_SOURCE, OnVideoSource)
	ON_BN_CLICKED(IDC_START_THREAD, OnStartThread)
	ON_BN_CLICKED(IDC_STOP_THREAD, OnStopThread)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OWN_0, OnButtonOwn0)
	ON_BN_CLICKED(IDC_BUTTON_OWN_1, OnButtonOwn1)
	ON_BN_CLICKED(IDC_BUTTON_OWN_2, OnButtonOwn2)
	ON_BN_CLICKED(IDC_BUTTON_OWN_3, OnButtonOwn3)
	ON_BN_CLICKED(IDC_BUTTON_OWN_4, OnButtonOwn4)
	ON_BN_CLICKED(IDC_VALUE_0_inc, OnVALUE0inc)
	ON_BN_CLICKED(IDC_VALUE_0_dec, OnVALUE0dec)
	ON_BN_CLICKED(IDC_VALUE_1_dec, OnVALUE1dec)
	ON_BN_CLICKED(IDC_VALUE_1_inc, OnVALUE1inc)
	ON_BN_CLICKED(IDC_VALUE_2_dec, OnVALUE2dec)
	ON_BN_CLICKED(IDC_VALUE_2_inc, OnVALUE2inc)
	ON_BN_CLICKED(IDC_VALUE_3_dec, OnVALUE3dec)
	ON_BN_CLICKED(IDC_VALUE_3_inc, OnVALUE3inc)
	ON_BN_CLICKED(IDC_VALUE_4_dec, OnVALUE4dec)
	ON_BN_CLICKED(IDC_VALUE_4_inc, OnVALUE4inc)
	ON_BN_CLICKED(IDC_VALUE_5_dec, OnVALUE5dec)
	ON_BN_CLICKED(IDC_VALUE_5_inc, OnVALUE5inc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVDlg Nachrichten-Handler

BOOL CPioneerBVDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if( pSysMenu != NULL ){
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if( !strAboutMenu.IsEmpty() ){	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	

//	SM_Init();			// init shared memory

	//m_camera.Initialize( 160, 120, 0, GetSafeHwnd() );
	if( !m_camera.Initialize( GetSafeHwnd() ) ){
		MessageBox( _T("Konnte Kamera nicht initialisieren!" ));
		return FALSE;
	}
	m_camera.Start();
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}


void CPioneerBVDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if( (nID & 0xFFF0) == IDM_ABOUTBOX )
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CPioneerBVDlg::OnPaint()
{
	if( IsIconic() )
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
// das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CPioneerBVDlg::OnQueryDragIcon(){
	return (HCURSOR) m_hIcon;
}

void CPioneerBVDlg::OnClose()
{
	if( !StopThread() )
	{
		return;
	}
	Uninitialize();
	CDialog::OnClose();
}


void CPioneerBVDlg::Uninitialize()
{
	if( m_camera.IsInitialized() && m_camera.IsRunning() )
		m_camera.Stop();
	if( m_camera.IsInitialized() )
		m_camera.Uninitialize();
}


void CPioneerBVDlg::OnCancel()
{
	if( !StopThread() )
	{
		return;
	}
	Uninitialize();	
	CDialog::OnCancel();
}


void CPioneerBVDlg::OnVideoFormat()
{
	if( m_pWorkerThread != NULL )
	{
		MessageBox( _T("Erst Thread beenden!" ));
		return;
	}
	m_camera.VideoFormatDlg();
	//m_camera.Start();
	
}


void CPioneerBVDlg::OnVideoSource()
{
	if( m_pWorkerThread != NULL )
	{
		MessageBox( _T("Erst Thread beenden!" ));
		return;
	}
	m_camera.VideoSourceDlg();
}


void CPioneerBVDlg::OnStartThread(){
	if( m_pWorkerThread == NULL )
	{
		bEndThread = false;
		m_pWorkerThread = AfxBeginThread( WorkerThread, (HWND)GetSafeHwnd() );
		SetTimer( ID_TIMER_FRAMECOUNT, 2000, NULL );
		m_timeThreadStart = timeGetTime();
	}
	else
	{
		MessageBox( _T("Thread läuft schon" ));
	}
}


void CPioneerBVDlg::OnStopThread()
{
	StopThread();
}


void CPioneerBVDlg::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == ID_TIMER_FRAMECOUNT )
	{
		m_strFrames.Format( _T("%.2f"), dThreadFrameCount/(timeGetTime()-m_timeThreadStart)*1000 );
		UpdateData( FALSE );
	}
	CDialog::OnTimer(nIDEvent);
}

bool CPioneerBVDlg::StopThread()
{
	if( m_pWorkerThread != NULL )
	{
		KillTimer( ID_TIMER_FRAMECOUNT );
		bEndThread = true;	
		return false;
	}
	return true;
}

LRESULT CPioneerBVDlg::OnWorkerThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_pWorkerThread = NULL;
	return 0L;
}

void CPioneerBVDlg::OnButtonOwn0()
{
	OnButtonOwn( this, 0 );

}

void CPioneerBVDlg::OnButtonOwn1()
{
	OnButtonOwn( this, 1 );
}

void CPioneerBVDlg::OnButtonOwn2()
{
	OnButtonOwn( this, 2 );
}

void CPioneerBVDlg::OnButtonOwn3()
{
	OnButtonOwn( this, 3 );
}

void CPioneerBVDlg::OnButtonOwn4()
{
	OnButtonOwn( this, 4 );
}

/**
 * Setzt die Bezeichnung des Feldes #index
 *
 */
void CPioneerBVDlg::SetFieldDesc( int index, LPCTSTR str )
{
	if( index == 0 )
		m_staticDesc0.SetWindowText( str );
	else if( index == 1 )
		m_staticDesc1.SetWindowText( str );
	else if( index == 2 )
		m_staticDesc2.SetWindowText( str );
	else if( index == 3 )
		m_staticDesc3.SetWindowText( str );
	else if( index == 4 )
		m_staticDesc4.SetWindowText( str );
	else if( index == 5 )
		m_staticDesc5.SetWindowText( str );
}

/**
 * Setzt den Wert des Feldes #index
 *
 */
void CPioneerBVDlg::SetFieldValue( int index, int value )
{
	TCHAR buf[20];
	_stprintf_s(buf, 20, _T("%d"), value);
	//itoa( value, buf, 20, 10 );
	if( index == 0 )
		m_staticValue0.SetWindowText( buf );
	else if( index == 1 )
		m_staticValue1.SetWindowText( buf );
	else if( index == 2 )
		m_staticValue2.SetWindowText( buf );
	else if( index == 3 )
		m_staticValue3.SetWindowText( buf );
	else if( index == 4 )
		m_staticValue4.SetWindowText( buf );
	else if( index == 5 )
		m_staticValue5.SetWindowText( buf );
}

/**
 * Setzt die Bezeichnung des Buttons #index
 *
 */
void CPioneerBVDlg::SetButtonDesc( int index, LPCTSTR str )
{
	if( index == 0 )
		m_buttonOwn0.SetWindowText( str );	
	else if( index == 1 )
		m_buttonOwn1.SetWindowText( str );	
	else if( index == 2 )
		m_buttonOwn2.SetWindowText( str );	
	else if( index == 3 )
		m_buttonOwn3.SetWindowText( str );	
	else if( index == 4 )
		m_buttonOwn4.SetWindowText( str );	
}

/**
 * Gibt in die History-Listbox den String str aus
 *
 */
void CPioneerBVDlg::Say( LPCTSTR str )
{
	m_listboxHistory.AddString( str );
	m_listboxHistory.SetTopIndex(m_listboxHistory.GetCount()-1);
}


void CPioneerBVDlg::ShowImage( CImage *pImage )
{
	CDC* dc = GetDC();
	HDC hdc = dc->GetSafeHdc();
	if( hdc )
	{
		pImage->Show( hdc, 5, 5, pImage->Width(), pImage->Height(), 0, 0 );
	}
	ReleaseDC( dc );
}

void CPioneerBVDlg::OnVALUE0inc() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 0 );
}

void CPioneerBVDlg::OnVALUE0dec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 1 );
}

void CPioneerBVDlg::OnVALUE1inc() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 2 );
}

void CPioneerBVDlg::OnVALUE1dec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 3 );
}

void CPioneerBVDlg::OnVALUE2dec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 5 );
}

void CPioneerBVDlg::OnVALUE2inc() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 4 );
}

void CPioneerBVDlg::OnVALUE3dec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 7 );
}

void CPioneerBVDlg::OnVALUE3inc() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 6 );
}

void CPioneerBVDlg::OnVALUE4dec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 9 );
}

void CPioneerBVDlg::OnVALUE4inc() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 8 );
}

void CPioneerBVDlg::OnVALUE5dec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 11 );
}

void CPioneerBVDlg::OnVALUE5inc() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	OnButton( this, 10 );
}
