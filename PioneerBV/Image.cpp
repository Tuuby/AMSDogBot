// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImage::CImage(){
    Clear();
}

void CImage::Clear(){ 
    memset( &m_img, 0, sizeof(m_img));

    m_memDC = 0; 
    m_old = 0;
}

void CImage::Destroy(){
    if( m_memDC ){
        DeleteObject( SelectObject( m_memDC, m_old ));
        DeleteDC( m_memDC );
    }
    Clear();
}

CImage::~CImage(){
    Destroy();
}


void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp ){
    ASSERT( bmi && width > 0 && height > 0 &&
            (bpp == 8 || bpp == 24 || bpp == 32) );

    BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

    memset( bmih, 0, sizeof(*bmih));
    bmih->biSize   = sizeof(BITMAPINFOHEADER); 
    bmih->biWidth  = width;
    bmih->biHeight = -abs(height);
    bmih->biPlanes = 1; 
    bmih->biBitCount = bpp;
    bmih->biCompression = BI_RGB;

    if( bpp == 8 ){
        RGBQUAD* palette = bmi->bmiColors;
        int i;
        for( i = 0; i < 256; i++ ){
            palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
            palette[i].rgbReserved = 0;
        }
    }
}


bool  CImage::Create( int w, int h, int bpp ){
    char buffer[sizeof(BITMAPINFOHEADER) + 1024];
    BITMAPINFO* bmi = (BITMAPINFO*)buffer;
    void* data = 0;
    int new_step = (w*(bpp/8) + 3) & -4;
    
    ASSERT( bpp == 8 || bpp == 24 || bpp == 32 );

    if( (m_img.depth & 255) == bpp && 
        m_img.width == w && m_img.height == h ){
        return true;
    }

    Destroy();
    
    m_memDC = CreateCompatibleDC(0);
    FillBitmapInfo( bmi, w, h, bpp );

    HBITMAP hbmp = CreateDIBSection( m_memDC, bmi, DIB_RGB_COLORS, &data, 0, 0 );
    if( !hbmp ){
        DeleteDC( m_memDC );
        m_memDC = 0;
    }
    else{
        BITMAP bmp;
        m_old = SelectObject( m_memDC, hbmp );

        GetObject( hbmp, sizeof(bmp), &bmp );

        /* prepare IPL header */
        memset( &m_img, 0, sizeof(m_img));
        m_img.nSize = sizeof( m_img );
        m_img.nChannels = bpp/8;
        m_img.depth = IPL_DEPTH_8U;
        strncpy_s( m_img.colorModel, 4, bpp > 8 ? "RGB\0" : "GRAY", 4 );
        strncpy_s( m_img.channelSeq, 4, bpp > 8 ? "BGR\0" : "GRAY", 4 );
        m_img.align = 4;
        m_img.width = w;
        m_img.height = abs(h);
        m_img.roi  = 0;
        m_img.widthStep = (w*(bpp/8) + 3)& -4;
        m_img.imageSize = m_img.widthStep*m_img.height;
        m_img.imageData = m_img.imageDataOrigin = (char*)bmp.bmBits;

    }
    return m_old != 0;
}


#if 0
static HINSTANCE hdll = 0;
static int (__cdecl *gr_fmt_find_filter)( const char* file_name ) = 0; 
static int (__cdecl *gr_fmt_read_header)( int filter, int* width, int* height, int* color ) = 0;
static int (__cdecl *gr_fmt_read_data)( int filter, void* data, int pitch, int color ) = 0;
static int (__cdecl *gr_fmt_close_filter)( int filter ) = 0;
static int (__cdecl *gr_fmt_write_image)( void* data, int pitch,
                                          int width, int height, int color,
                                          const char* filename, const char* format ) = 0;

bool  LoadGrFmtLib(){
    if( hdll != 0 ) return true;

    // load image formats dll
    hdll = LoadLibrary( "cvlgrfmts.dll");
    if( !hdll ) return false;

    (FARPROC&)gr_fmt_find_filter = GetProcAddress( hdll, "gr_fmt_find_filter" );
    (FARPROC&)gr_fmt_read_header = GetProcAddress( hdll, "gr_fmt_read_header" );
    (FARPROC&)gr_fmt_read_data   = GetProcAddress( hdll, "gr_fmt_read_data" );
    (FARPROC&)gr_fmt_close_filter= GetProcAddress( hdll, "gr_fmt_close_filter" );
    (FARPROC&)gr_fmt_write_image = GetProcAddress( hdll, "gr_fmt_write_image" );

    if( !gr_fmt_find_filter || !gr_fmt_read_header || !gr_fmt_read_data ){
        FreeLibrary( hdll );
        hdll = 0;
        return false;
    }
    return true;
}
#endif


IplImage* CImage::GetImage(){
    return m_memDC != 0 ? &m_img : 0;
}


HDC  CImage::GetDC(){
    return m_memDC;
}


void CImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y ){
    if( m_img.width > 0 ){
        uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        int bmp_w = Width();
        int bmp_h = Height();

        FillBitmapInfo( bmi, bmp_w, bmp_h, m_img.nChannels*8 );

        int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
        int sh = MAX( MIN( bmp_h - from_y, h ), 0 );

        int res = SetDIBitsToDevice(
              dc,                        // handle to DC
              x,                         // x-coord of destination upper-left corner
              y,                         // y-coord of destination upper-left corner 
              sw,                        // source rectangle width
              sh,                        // source rectangle height
              from_x,                    // x-coord of source lower-left corner
              from_y,                    // y-coord of source lower-left corner
              from_y,                    // first scan line in array
              sh,                        // number of scan lines
              m_img.imageData + from_y*m_img.widthStep/* +
              from_x*m_img.nChannels*/,    // array of DIB bits
              (BITMAPINFO*)bmi,          // bitmap information
              DIB_RGB_COLORS );          // RGB or palette indexes
    }
}

void  CImage::Fill( COLORREF color ){
    if( m_memDC ){
        HBRUSH br = CreateSolidBrush( color );
        RECT rect;
        GetClipBox( m_memDC, &rect );
        FillRect( m_memDC, &rect, br );
        DeleteObject( br );
    }
}

