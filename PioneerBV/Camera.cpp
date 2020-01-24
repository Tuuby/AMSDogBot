//////////////////////////////////////////////////////////////////////
// Camera.cpp: implementation of the CCamera class.
//
// Orginal - Code by Intel(TM).
//
// Additionals by Enrico Ehrich 
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
    m_capWnd			= 0;
    m_isRunning			= false;
	m_hMutexFrame1		= NULL;
	m_hMutexFrame2		= NULL;
	m_pFrame1			= new CImage();
	m_pFrame2			= new CImage();
	m_pFrame4Grabbing	= m_pFrame1;
	m_pFrame4Processing = m_pFrame1;
	bNewFrame1			= false;
	bNewFrame2			= false;
	m_pGrabThread		= NULL;
}

CCamera::~CCamera()
{
    Uninitialize();
	if( m_hMutexFrame1 )
		CloseHandle( m_hMutexFrame1 );
	if( m_hMutexFrame2 )
		CloseHandle( m_hMutexFrame2 );

	delete m_pFrame1;
	delete m_pFrame2;
}

/* conversion YUV->RGB */
#define TWIST_BITS 12
#define TAP(x) ((int)((x)*(1<<TWIST_BITS) + 0.5 ))

static const int YUV2RGB[] = 
{
    TAP(1.1644), TAP(1.596), TAP(    0), -TAP(222.9184),
    TAP(1.1644),-TAP(0.815),-TAP( 0.39),  TAP(135.6096),
    TAP(1.1644), TAP(0),     TAP(2.016), -TAP(276.6784)
};

#define PROCESS_UV( uv_ofs, r0, g0, b0 )                                       \
    b0 = U[uv_ofs]*YUV2RGB[1+4*0] +           /* 0 + */        YUV2RGB[3+4*0]; \
    g0 = U[uv_ofs]*YUV2RGB[1+4*1] + V[uv_ofs]*YUV2RGB[2+4*1] + YUV2RGB[3+4*1]; \
    r0 =      /*  0 + */            V[uv_ofs]*YUV2RGB[2+4*2] + YUV2RGB[3+4*2];

#define PROCESS_Y( y_ofs, r, g, b, r0, g0, b0 ) \
    r = Y[y_ofs]*YUV2RGB[0+4*0];                \
    b = (r + b0) >> TWIST_BITS;                 \
    g = (r + g0) >> TWIST_BITS;                 \
    r = (r + r0) >> TWIST_BITS;

#define SATURATE(x) (BYTE)(!((x)&~255)?(x):~((x)>>31))

#define SAVE_RGB(ofs,r,g,b)     \
    BGR[(ofs)]   = SATURATE(b); \
    BGR[(ofs)+1] = SATURATE(g); \
    BGR[(ofs)+2] = SATURATE(r);


static void ConvertYUV420_TO_BGR( int width, int height, BYTE* Y, BYTE* U, BYTE* V, BYTE* BGR, int bgrStep )
{
    ASSERT( ((width|height)&7) == 0 );

    int delta = bgrStep - width*3;
    width /= 2;

    for( int i = 0; i < height; i += 2, Y += 4*width, U += width, V += width, BGR += delta + bgrStep )
	{
        for( int j = 0; j < width; j++, BGR += 6 )
		{
            int r0, g0, b0, r, g, b;

            PROCESS_UV( j, r0, g0, b0 );
            
            PROCESS_Y( j*2, r, g, b, r0, g0, b0 );
			SAVE_RGB( 0, r, g, b );
			
            PROCESS_Y( j*2 + 1, r, g, b, r0, g0, b0 );
			SAVE_RGB( 3, r, g, b );
			
            PROCESS_Y( j*2 + width*2, r, g, b, r0, g0, b0 );
			SAVE_RGB( bgrStep, r, g, b );
			
            PROCESS_Y( j*2 + width*2 + 1, r, g, b, r0, g0, b0 );
			SAVE_RGB( bgrStep + 3, r, g, b );
        }
    }
}


// yuy2 and uyvy to BGR from I. Boersch for ATI Radeon VFW WDM
static void ConvertYUY2_TO_BGR( int width, int height, BYTE* data, BYTE* BGR)
{
	int r0, g0, b0, r, g, b;
	BYTE y0,y1,u,v;

//	ASSERT( ((width|height)&7) == 0 );

	for( int i = 0; i < height; i++)
		for( int j = 0; j < width; j+=2)
		{
			y0=data[0];
			u=data[1];
			y1=data[2];
			v=data[3];

			b0 = u*YUV2RGB[1+4*0] +           /* 0 + */        YUV2RGB[3+4*0]; 
			g0 = u*YUV2RGB[1+4*1] + v*YUV2RGB[2+4*1] + YUV2RGB[3+4*1]; 
			r0 =      /*  0 + */            v*YUV2RGB[2+4*2] + YUV2RGB[3+4*2];


			r = y0*YUV2RGB[0+4*0];                
			b = (r + b0) >> TWIST_BITS;                 
			g = (r + g0) >> TWIST_BITS;                 
			r = (r + r0) >> TWIST_BITS;

			SAVE_RGB( 0, r, g, b );

			BGR+=3;

			r = y1*YUV2RGB[0+4*0];                
			b = (r + b0) >> TWIST_BITS;                 
			g = (r + g0) >> TWIST_BITS;                 
			r = (r + r0) >> TWIST_BITS;

			SAVE_RGB( 0, r, g, b );

			BGR+=3;
			data+=4;
		}
}

static void ConvertUYVY_TO_BGR( int width, int height, BYTE* data, BYTE* BGR)
{
	int r0, g0, b0, r, g, b;
	BYTE y0,y1,u,v;

	ASSERT( ((width|height)&7) == 0 );

	for( int i = 0; i < height; i++)
		for( int j = 0; j < width; j+=2)
		{
			u=data[0];
			y0=data[1];
			v=data[2];
			y1=data[3];

			b0 = u*YUV2RGB[1+4*0] +           /* 0 + */        YUV2RGB[3+4*0]; 
			g0 = u*YUV2RGB[1+4*1] + v*YUV2RGB[2+4*1] + YUV2RGB[3+4*1]; 
			r0 =      /*  0 + */            v*YUV2RGB[2+4*2] + YUV2RGB[3+4*2];


			r = y0*YUV2RGB[0+4*0];                
			b = (r + b0) >> TWIST_BITS;                 
			g = (r + g0) >> TWIST_BITS;                 
			r = (r + r0) >> TWIST_BITS;

			SAVE_RGB( 0, r, g, b );

			BGR+=3;

			r = y1*YUV2RGB[0+4*0];                
			b = (r + b0) >> TWIST_BITS;                 
			g = (r + g0) >> TWIST_BITS;                 
			r = (r + r0) >> TWIST_BITS;

			SAVE_RGB( 0, r, g, b );

			BGR+=3;
			data+=4;
		}
}


static void CopyBGR_TO_BGR( int width, int height, BYTE* srcBGR, int srcStep, BYTE* dstBGR, int dstStep )
{
	if(srcStep==dstStep)
	{
		memcpy( dstBGR, srcBGR, width*height*3 ); 
		return;
	}

	int w = width*3;
	
    for( int i = 0; i < height; i++, srcBGR += srcStep, dstBGR += dstStep )
	{
        memcpy( dstBGR, srcBGR, w);            
    }
}


void CCamera::UpdateParent( IplImage* pImage, bool whole )
{
    HWND parent = GetParent( m_capWnd );
    RECT r = { 0, 0, 0, 0 };
    
    if( pImage )
	{
        r.right = pImage->width;
        r.bottom = pImage->height;
    }
    
    InvalidateRect( parent, whole ? 0 : &r, whole );
    UpdateWindow( parent );
}

// Wrapper for frame callback
void CCamera::OnFrame( BYTE* data, int width, int height, int format, int bpp )
{
    const int mirror = 1;

	if( m_isRunning )
	{
        BYTE*		BGR;
        int			bgrStep;
        IplImage*	img;
		CImage*		pImage;
		HANDLE		hMutex;

		if( m_pFrame4Grabbing == m_pFrame1 )
		{
			
			if( m_hMutexFrame1 == NULL )
				m_hMutexFrame1 = CreateMutex( NULL, TRUE, _T("MutexFrame1") );
			else
				WaitForSingleObject( m_hMutexFrame1, INFINITE );
			

			OutputDebugString(_T("MUTEX1 - Locked by Grab\n"));
			bNewFrame1 = true;
			pImage = m_pFrame1;
			m_pFrame4Grabbing = m_pFrame2;
			hMutex = m_hMutexFrame1;
		}
		else
		{
			
			if( m_hMutexFrame2 == NULL )
				m_hMutexFrame2 = CreateMutex( NULL, TRUE, _T("MutexFrame2" ));
			else
				WaitForSingleObject( m_hMutexFrame2, INFINITE );
			
			OutputDebugString(_T("MUTEX2- Locked by Grab\n"));
			bNewFrame2 = true;
			pImage = m_pFrame2;
			m_pFrame4Grabbing = m_pFrame1;
			hMutex = m_hMutexFrame2;
		}

        //bool refresh = width != pImage->Width() || height != pImage->Height();
    
        /* check if need reallocate pImage */
        pImage->Create( width, height, 24 );

        img = pImage->GetImage();

        BGR = (BYTE*)(img->imageData);
        bgrStep = img->widthStep;

		
		if( format == MAKEFOURCC('Y','U','Y','2'))
		{
			ConvertYUY2_TO_BGR( width, height, data, BGR);

		} else if( format == MAKEFOURCC('U','Y','V','Y'))
		{
			ConvertUYVY_TO_BGR( width, height, data, BGR);
		} else  if( format == MAKEFOURCC('Y','V','1','2') || format == MAKEFOURCC('I','4','2','0'))
		{
            BYTE *Y = data,
                 *U = Y + width*height,
                 *V = U + width*height/4;

            ConvertYUV420_TO_BGR( width, height, Y, U, V, BGR, bgrStep );
        }
        else if( format == 0 && bpp == 24 )
		{
            int step = (width*3 + 3) & -4;

            if( mirror )
			{
                BGR += bgrStep*(height - 1);
                bgrStep = -bgrStep;
            }

            CopyBGR_TO_BGR( width, height, data, step, BGR, bgrStep );
        }
		
        //UpdateParent( img, refresh );
		
		ReleaseMutex( hMutex );
		if( hMutex == m_hMutexFrame1 )
			OutputDebugString(_T("MUTEX1 - Unlocked by Grab\n"));
		else
			OutputDebugString(_T("MUTEX2 - Unlocked by Grab\n"));
		
    }
			
}

// Frame callback
LRESULT PASCAL FrameCallbackProc( HWND hWnd, VIDEOHDR* hdr )
{ 
    BITMAPINFO vfmt;

	if (!hWnd) return FALSE;

    int sz = capGetVideoFormat( hWnd, &vfmt, sizeof(vfmt));
    
    if( hdr && hdr->lpData && sz != 0 )
	{
        int width		= vfmt.bmiHeader.biWidth;
        int height		= vfmt.bmiHeader.biHeight;
        int format		= vfmt.bmiHeader.biCompression;
        int bpp			= vfmt.bmiHeader.biBitCount;
        BYTE *data		= (BYTE*)(hdr->lpData);
        CCamera* camera_obj = (CCamera*)capGetUserData(hWnd);
        if( camera_obj )
		{
            camera_obj->OnFrame( data, width, height, format, bpp );
			//camera_obj->GrabFrame();
		}
    }

    return (LRESULT) TRUE; 
} 

// Initialize camera input
bool  CCamera::Initialize( HWND parent )
{
	TCHAR szDeviceName[80];
	TCHAR szDeviceVersion[80];
	int width, height, format, bits_per_pixel;
    HWND hWndC = 0;


    
    for( int wIndex = 0; wIndex < 10; wIndex++ )
	{
        if( capGetDriverDescription( wIndex, szDeviceName, sizeof(szDeviceName), szDeviceVersion, sizeof(szDeviceVersion) ) )
		{
            // Unsichtbares CaptureWindow erzeugen, sichtbar wäre "WS_CHILD | WS_VISIBLE"
			hWndC = capCreateCaptureWindow( _T("My Own Capture Window"), WS_CHILD , 0, 0, 160, 120, parent, 0);

            if( capDriverConnect( hWndC, wIndex ) )
			{
                BITMAPINFO bmi;
 
                // Low resolution with ATI Rage Theater Video input
				width = 320;
				height = 240;
				
				// Higher resolution with ATI Rage Theater Video Input
				// width = 640;
				// height = 480;

				format = MAKEFOURCC('Y','U','Y','2');
				
				bits_per_pixel = 16;
				
				memset( &bmi.bmiHeader, 0, sizeof(bmi.bmiHeader));
                bmi.bmiHeader.biSize		= sizeof(bmi.bmiHeader);
                bmi.bmiHeader.biBitCount	= bits_per_pixel;
				bmi.bmiHeader.biWidth		= width;
                bmi.bmiHeader.biHeight		= height;
                bmi.bmiHeader.biPlanes		= 1;
                bmi.bmiHeader.biCompression = format;
                bmi.bmiHeader.biSizeImage	= ((bmi.bmiHeader.biWidth*
                                              bmi.bmiHeader.biBitCount/8 + 3)&-4)*
                                              bmi.bmiHeader.biHeight;

				// Format is accepted by the camera?
				if( capSetVideoFormat( hWndC, &bmi, sizeof(bmi)-4)) break;
				
				// für USB-Framegrabber DFG-it, ansonsten wie vorher
				format = MAKEFOURCC('U','Y','V','Y');
				bmi.bmiHeader.biCompression = format;
                // Format is accepted by the camera?
				if( capSetVideoFormat( hWndC, &bmi, sizeof(bmi)-4)) break;
			
				// Low resolution with USB Framegrabber Videobus II, RGB-Format
				width = 320;
				height = 240;
				format = 0;	
				bits_per_pixel = 24;
				
				memset( &bmi.bmiHeader, 0, sizeof(bmi.bmiHeader));
                bmi.bmiHeader.biSize		= sizeof(bmi.bmiHeader);
                bmi.bmiHeader.biBitCount	= bits_per_pixel;
				bmi.bmiHeader.biWidth		= width;
                bmi.bmiHeader.biHeight		= height;
                bmi.bmiHeader.biPlanes		= 1;
                bmi.bmiHeader.biCompression = format;
                bmi.bmiHeader.biSizeImage	= ((bmi.bmiHeader.biWidth*
                                              bmi.bmiHeader.biBitCount/8 + 3)&-4)*
                                              bmi.bmiHeader.biHeight;

				// Format is accepted by the camera?
				if( capSetVideoFormat( hWndC, &bmi, sizeof(bmi)-4)) break;

				// This camera is not usable, try next
				capDriverDisconnect( hWndC );
            }
            DestroyWindow( hWndC );
            hWndC = 0;
        }
    }
    
    if( hWndC )
	{
        m_capWnd = hWndC;
        
        memset( &m_caps, 0, sizeof(m_caps));
        capDriverGetCaps( hWndC, &m_caps, sizeof(m_caps));
        capSetUserData( hWndC, (long)this );
        capSetCallbackOnFrame( m_capWnd, FrameCallbackProc ); 
        //::MoveWindow( m_capWnd, 0, 0, 1, 1, TRUE );
		::MoveWindow( m_capWnd, 200, 0, 200, 200, FALSE );	// CaptureWindow setzen, aber nicht anzeigen
    }
    return m_capWnd != 0;
}


// Uninitialize camera input
void  CCamera::Uninitialize()
{
    Stop();
    capSetCallbackOnFrame( m_capWnd, NULL ); 
    capDriverDisconnect( m_capWnd );
    DestroyWindow( m_capWnd );
}

// Start capture
void  CCamera::Start()
{
    if( m_capWnd )
	{
        m_isRunning = true;
		//m_pGrabThread = AfxBeginThread( GrabThread, (LPVOID)this);//, THREAD_PRIORITY_LOWEST);
		GrabFrame();
		GrabFrame();
    }
}

// Stop capture
void  CCamera::Stop()
{
    if( m_capWnd )
	{
        m_isRunning = false;
		if( m_pGrabThread )
			m_pGrabThread = NULL;
    }
}

void  CCamera::VideoFormatDlg()
{
	if( m_capWnd && m_caps.fHasDlgVideoFormat )
        capDlgVideoFormat( m_capWnd );
}


void  CCamera::VideoSourceDlg()
{
    if( m_capWnd && m_caps.fHasDlgVideoSource )
        capDlgVideoSource( m_capWnd );
}

CImage* CCamera::GetFrame()
{
	GrabFrame();

	if( m_pFrame4Processing == m_pFrame1 )
	{
		while( m_hMutexFrame1 == NULL || !bNewFrame1 );
		WaitForSingleObject( m_hMutexFrame1, INFINITE );
		OutputDebugString(_T("MUTEX1 - Locked by IP\n"));

		return m_pFrame1;
	}
	else
	{
		while( m_hMutexFrame2 == NULL || !bNewFrame2 );
		WaitForSingleObject( m_hMutexFrame2, INFINITE );
		OutputDebugString(_T("MUTEX2 - Locked by IP\n"));
	
		return m_pFrame2;
	}
}

void CCamera::ReleaseFrame( CImage *pImage )
{
	if( pImage == m_pFrame1 )
	{
		m_pFrame4Processing = m_pFrame2;
		bNewFrame1 = false;
		ReleaseMutex( m_hMutexFrame1 );
		OutputDebugString(_T("MUTEX1 - Unlocked by IP\n"));
	}
	else
	{
		m_pFrame4Processing = m_pFrame1;
		bNewFrame2 = false;
		ReleaseMutex( m_hMutexFrame2 );
		OutputDebugString(_T("MUTEX2 - Unlocked by IP\n"));
	}
}

void CCamera::GrabFrame()
{
	if( canCaptureNewFrame() )
		capGrabFrame( m_capWnd );
}

UINT GrabThread( LPVOID camera )
{ 
	CCamera* pCam = (CCamera*)camera;
	while(true)
		pCam->GrabFrame();

	return 0;
}


