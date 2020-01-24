// PioneerBVDlg.h : Header-Datei
//

#if !defined(AFX_PIONEERBVDLG_H__421EAAC7_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_)
#define AFX_PIONEERBVDLG_H__421EAAC7_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_

#include "resource.h"
#include "Camera.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ID der benutzerdefinierten Nachricht
#define WM_WORKER_THREAD_FINISHED WM_USER + 5

#define FIELD_COUNT 6
#define OWN_BUTTON_COUNT 5

/////////////////////////////////////////////////////////////////////////////
// CPioneerBVDlg Dialogfeld

class CPioneerBVDlg : public CDialog
{
// Konstruktion
public:
	void ShowImage( CImage *pImage);
	CPioneerBVDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

	void SetFieldDesc( int index, LPCTSTR str);
	void SetFieldValue( int index, int value );
	void SetButtonDesc( int index, LPCTSTR str);
	void Say( LPCTSTR str );

	CCamera* GetCamera(){return &m_camera;};

// Dialogfelddaten
	//{{AFX_DATA(CPioneerBVDlg)
	enum { IDD = IDD_PIONEERBV_DIALOG };
	CListBox	m_listboxHistory;
	CButton	m_buttonOwn4;
	CButton	m_buttonOwn3;
	CButton	m_buttonOwn2;
	CButton	m_buttonOwn1;
	CButton	m_buttonOwn0;
	CStatic	m_staticValue5;
	CStatic	m_staticValue4;
	CStatic	m_staticValue3;
	CStatic	m_staticValue2;
	CStatic	m_staticValue1;
	CStatic	m_staticValue0;
	CStatic	m_staticDesc5;
	CStatic	m_staticDesc4;
	CStatic	m_staticDesc3;
	CStatic	m_staticDesc2;
	CStatic	m_staticDesc1;
	CStatic	m_staticDesc0;
	CString	m_strFrames;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPioneerBVDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CPioneerBVDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnCancel();
	afx_msg void OnVideoFormat();
	afx_msg void OnVideoSource();
	afx_msg void OnStartThread();
	afx_msg void OnStopThread();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonOwn0();
	afx_msg void OnButtonOwn1();
	afx_msg void OnButtonOwn2();
	afx_msg void OnButtonOwn3();
	afx_msg void OnButtonOwn4();
	afx_msg void OnVALUE0inc();
	afx_msg void OnVALUE0dec();
	afx_msg void OnVALUE1dec();
	afx_msg void OnVALUE1inc();
	afx_msg void OnVALUE2dec();
	afx_msg void OnVALUE2inc();
	afx_msg void OnVALUE3dec();
	afx_msg void OnVALUE3inc();
	afx_msg void OnVALUE4dec();
	afx_msg void OnVALUE4inc();
	afx_msg void OnVALUE5dec();
	afx_msg void OnVALUE5inc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
	CCamera m_camera;
	DWORD m_timeThreadStart;
	CWinThread* m_pWorkerThread;
	
	void Uninitialize();
	bool StopThread();
	LRESULT OnWorkerThreadFinished( WPARAM wParam, LPARAM lParam );

};

// Ausgabe eines Strings aus dem Thread in die History
void tsay(LPCTSTR s) ;
// Ausgabe eines Strings und einer int-Zahl 
// aus dem Thread in die History
void tsayi(LPCTSTR s,int i) ;
// Erzeugen eines Pixelzeigers aus Device Independent Bitmap

/*BYTE* PixelSpeicher(LPVOID h) ;
// Beschreiben der unterne Parameterfelder
void tsetFieldStr( LPSTR s, int field);
void tsetLabelStr( LPSTR s, int field);
void tsetFieldInt( long value, int field);
// Beschriften der selbstprogrammierten Buttons
void tsetButtonStr( LPSTR s, int button);
extern CWinThread*  worker;				// NULL = Thread läuft nicht
extern volatile BOOL thread_beenden;
#define FPSARRAYLENGTH 10 //Länge des Array zur fps-Berechnung
extern int frameCounter[FPSARRAYLENGTH];// Zählerarray für Frames per Second
extern int frameCounterIndex; // index für das Zählerarray
*/

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PIONEERBVDLG_H__421EAAC7_AB26_11D4_94EE_00D05C1F0A22__INCLUDED_)
