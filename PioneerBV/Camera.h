//////////////////////////////////////////////////////////////////////
// Camera.cpp: interface of the CCamera class.
//
// Orginal - Code by Intel(TM).
//
// Additionals by Enrico Ehrich 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__A499AE7A_13AB_4297_AE1E_25B23734F6FA__INCLUDED_)
#define AFX_CAMERA_H__A499AE7A_13AB_4297_AE1E_25B23734F6FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "image.h"
#include "vfw.h"
#include <afxmt.h>

UINT GrabThread( LPVOID camera );

class CCamera  
{
	CWinThread*		m_pGrabThread;

    CImage*			m_pFrame1;
	CImage*			m_pFrame2;
	
	CImage*			m_pFrame4Grabbing;
	CImage*			m_pFrame4Processing;
    
	bool			m_isRunning;
    CAPDRIVERCAPS	m_caps;

	HANDLE			m_hMutexFrame1;
	HANDLE			m_hMutexFrame2;

	bool			bNewFrame1;
	bool			bNewFrame2;

	bool			canCaptureNewFrame() const
					{
						if(m_pFrame4Grabbing == m_pFrame1)
							return !bNewFrame1; 
						else
							return !bNewFrame2;

						return false;
					}

public:
	void			GrabFrame();
    HWND			m_capWnd;
	void			ReleaseFrame( CImage *pImage );

    CImage*			GetFrame(); 

					CCamera();
	virtual			~CCamera();
    
    bool			IsInitialized() 
					{ 
						return m_capWnd != 0; 
					}

    bool			IsRunning() 
					{ 
						return m_isRunning; 
					}

    void			OnFrame( BYTE* data, int width, int height, int format, int bpp );
    bool			Initialize( HWND parent );
    void			Uninitialize();

    void			ClearRect();
    void			Start();
    void			Stop();
    void			VideoFormatDlg();
    void			VideoSourceDlg();
    void			UpdateParent( IplImage* pImage, bool whole );
};

#endif // !defined(AFX_CAMERA_H__A499AE7A_13AB_4297_AE1E_25B23734F6FA__INCLUDED_)
