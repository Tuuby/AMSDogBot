#include "stdafx.h"
#include "Thread.h"
#include "PioneerBV.h"
#include "PioneerBVDlg.h"
#include "Image.h"
#include "cv.hpp"
#include "Shared_Memory.h"

using namespace cv;
using namespace std;

extern volatile bool bEndThread;	// Wunsch der GUI an den Thread, 
// sich zu beenden

double dThreadFrameCount = 0.0;		// Bestimmung der Framerate

// Anzahl der Hilfsbilder
#define ANZ_BILD 5
int iBild_Index = ANZ_BILD;			// Standardanzeige = gegrabbtes Bild


// Farbkan‰le
#define ROT		2
#define GRUEN	1
#define BLAU	0

// Subtraktion zweier Farbkan‰le: Fkt: Choose_Diff
int minuend=ROT, subtrahend=GRUEN;

// Init der Werte-Felder
int iValue_0 = 0;
int iValue_1 = 0;
int iValue_2 = 0;
int iValue_3 = 5;
int iValue_4 = 0;
int iValue_5 = 0;

// Namen der Variablenfelder
#define FIELD_VALUE_0 0
#define FIELD_VALUE_1 1
#define FIELD_VALUE_2 2
#define FIELD_VALUE_3 3
#define FIELD_VALUE_4 4
#define FIELD_VALUE_5 5

void zeichne_kreuz(long x,long y, long breite, CImage *pImage) ;

/* Die Funktion InitButtons initialisiert die angezeigten Texte 
und Werte der Variablenfelder, sowie die Beschriftung der OwnButtons */
void InitButtons()
{
	// Zeiger auf GUI besorgen
	CPioneerBVDlg* pDlg = ((CPioneerBVApp*)AfxGetApp())->GetDialog();

	// Init-Texte der OwnButtons und ihrer Werte
	pDlg->SetButtonDesc( 0, _T("R-G" ));
	minuend=ROT, subtrahend=GRUEN;

	pDlg->SetButtonDesc( 1, _T("Original" ));
	iBild_Index = ANZ_BILD;

	// Init-Texte und -Werte der Variablenfelder
	pDlg->SetFieldValue( FIELD_VALUE_0, iValue_0);
	pDlg->SetFieldDesc( FIELD_VALUE_0, _T("bv1 (x)") );

	pDlg->SetFieldValue( FIELD_VALUE_1, iValue_1 );
	pDlg->SetFieldDesc( FIELD_VALUE_1, _T("bv2 (y)" ));

	pDlg->SetFieldValue( FIELD_VALUE_2, iValue_2 );
	pDlg->SetFieldDesc( FIELD_VALUE_2, _T("bv3 (flag)" ));

	pDlg->SetFieldValue( FIELD_VALUE_3, iValue_3 );
	pDlg->SetFieldDesc( FIELD_VALUE_3, _T("Filterparm" ));

	pDlg->SetFieldValue( FIELD_VALUE_4, iValue_4 );
	pDlg->SetFieldDesc( FIELD_VALUE_4, _T("Schwelle" ));

	pDlg->SetFieldValue( FIELD_VALUE_5, iValue_5 );
	pDlg->SetFieldDesc( FIELD_VALUE_5, _T("Rahmendicke" ));
}


/* Die Funktion OnButton wird mit dem Index des gedrueckten
Buttons gerufen, hierbei handelt es sich um die +/- - Buttons
der Variablenfelder, die Variablenwerte werden dabei in der Regel 
auf [0..255] beschr‰nkt  */
void OnButton( CPioneerBVDlg* pDlg, int iButtonIndex )
{
	switch( iButtonIndex )
	{
		// Wert 0 wird ge‰ndert
	case 0:
		if (iValue_0 < 255)
		{	iValue_0++;
		pDlg->SetFieldValue( FIELD_VALUE_0, iValue_0 );
		}
		break;
	case 1:
		if (iValue_0 > 0)
		{	iValue_0--;
		pDlg->SetFieldValue( FIELD_VALUE_0, iValue_0 );
		}
		break;

		// Wert 1 wird ge‰ndert
	case 2:
		if (iValue_1 < 255)
		{	iValue_1++;
		pDlg->SetFieldValue( FIELD_VALUE_1, iValue_1 );
		}
		break;
	case 3:
		if (iValue_1 > 0)
		{	iValue_1--;
		pDlg->SetFieldValue( FIELD_VALUE_1, iValue_1 );
		}
		break;

		// Wert 2 wird ge‰ndert
	case 4:
		if (iValue_2 < 255)
		{	iValue_2++;
		pDlg->SetFieldValue( FIELD_VALUE_2, iValue_2 );
		}
		break;
	case 5:
		if (iValue_2 > 0)
		{	iValue_2--;
		pDlg->SetFieldValue( FIELD_VALUE_2, iValue_2 );
		}
		break;

		// Wert 3 wird ge‰ndert
	case 6:
		if (iValue_3 < 255)
		{	iValue_3++;
		pDlg->SetFieldValue( FIELD_VALUE_3, iValue_3 );
		}
		break;
	case 7:
		if (iValue_3 > 0)
		{	iValue_3--;
		pDlg->SetFieldValue( FIELD_VALUE_3, iValue_3 );
		}
		break;

		// Wert 4 wird ge‰ndert
	case 8:
		if (iValue_4 < 255)
		{	iValue_4++;
		pDlg->SetFieldValue( FIELD_VALUE_4, iValue_4 );
		}
		break;
	case 9:
		if (iValue_4 > 0)
		{	iValue_4--;
		pDlg->SetFieldValue( FIELD_VALUE_4, iValue_4 );
		}
		break;

		// Wert 5 wird ge‰ndert
	case 10:
		if (iValue_5 < 255)
		{	iValue_5++;
		pDlg->SetFieldValue( FIELD_VALUE_5, iValue_5 );
		}
		break;
	case 11:
		if (iValue_5 > 0)
		{	iValue_5--;
		pDlg->SetFieldValue( FIELD_VALUE_5, iValue_5 );
		}
		break;
	}
}


/* Die Funktion Choose_Diff kann von einem OwnButton gerufen werden 
und schaltet die globalen Variablen minuend, subtrahend um. Die 
Belegung der Variablen kann im Thread zur Binarisierung mittels
einer Farbkanal-Differenz benutzt werden */

void Choose_Diff(CPioneerBVDlg* pDlg, int OwnButton) {
	// zwischen den 6 mˆglichen Farbanteildifferenzen zirkulieren
	// (R-G, R-B, G-R, G-B, B-R, B-G)

	if ((minuend == ROT) && (subtrahend == GRUEN)) {
		minuend = ROT;
		subtrahend = BLAU;
		pDlg->Say(_T("Differenzbild: Rot-Blau"));
		pDlg->SetButtonDesc(OwnButton, _T("R - B"));
	}
	else if ((minuend == ROT) && (subtrahend == BLAU)) {
		minuend = GRUEN;
		subtrahend = ROT;
		pDlg->Say(_T("Differenzbild: Gr¸n-Rot"));
		pDlg->SetButtonDesc(OwnButton, _T("G - R"));
	}
	else  if((minuend == GRUEN) && (subtrahend == ROT)) {
		minuend = GRUEN;
		subtrahend = BLAU;
		pDlg->Say(_T("Differenzbild: Gr¸n-Blau"));
		pDlg->SetButtonDesc(OwnButton, _T("G - B"));
	}
	else if ((minuend == GRUEN) && (subtrahend == BLAU)) {
		minuend = BLAU;
		subtrahend = ROT;
		pDlg->Say(_T("Differenzbild: Blau-Rot"));
		pDlg->SetButtonDesc(OwnButton, _T("B - R"));
	}
	else if ((minuend == BLAU) && (subtrahend == ROT)) {
		minuend = BLAU;
		subtrahend = GRUEN;
		pDlg->Say(_T("Differenzbild: Blau-Gr¸n"));
		pDlg->SetButtonDesc(OwnButton, _T("B - G"));
	}
	else  if((minuend == BLAU) && (subtrahend == GRUEN)) {
		minuend = ROT;
		subtrahend = GRUEN;
		pDlg->Say(_T("Differenzbild: Rot-Gr¸n"));
		pDlg->SetButtonDesc(OwnButton, _T("R - G"));
	}
}

void Choose_Image(CPioneerBVDlg* pDlg, int OwnButton) {
	// schaltet die Anzeige der Bilder um
	// 0,1 .. ANZ-BILD-1, GrabBild
	TCHAR sStr[100];

	// Umschalten
	iBild_Index++;
	if (iBild_Index >ANZ_BILD) iBild_Index = 0;

	// Ausgabe
	if (iBild_Index == ANZ_BILD) {
		pDlg->Say(_T("Anzeige Originalbild"));
		pDlg->SetButtonDesc(OwnButton, _T("Original"));
	}
	else {
		_stprintf_s(sStr, 100, _T("Anzeige Bild %d"), iBild_Index);
		pDlg->Say(sStr);
		_stprintf_s(sStr, 100, _T("Bild %d"), iBild_Index);

		pDlg->SetButtonDesc(OwnButton,sStr);
	}
}

/* Die Funktion OnOwnButton wird mit dem Index des 
gedr¸ckten OwnButtons gerufen */

void OnButtonOwn( CPioneerBVDlg* pDlg, int iButtonIndex )
{
	switch( iButtonIndex )
	{
		// Knopf 0 gedrueckt
	case 0:
		//pDlg->Say("Knˆpche 0");
		Choose_Diff(pDlg,0);
		break;
		// Knopf 1 gedrueckt
	case 1:
		//pDlg->Say("Knˆpche 1");
		Choose_Image(pDlg,1);
		break;
		// Knopf 2 gedrueckt
	case 2:
		pDlg->Say(_T("Knˆpche 2"));
		break;
		// Knopf 3 gedrueckt
	case 3:
		pDlg->Say(_T("Knˆpche 3"));
		break;
		// Knopf 4 gedrueckt
	case 4:
		pDlg->Say(_T("Knˆpche 4"));
		break;
	}
}

/* HIER wird gearbeitet ! */

UINT WorkerThread( LPVOID pParam ){ 
	HWND hWnd = (HWND)pParam;
	dThreadFrameCount = 0.0;

	Shared_Memory * shared_mem = new Shared_Memory();
	shared_mem->SM_Init();

	int i;				// Pixelz‰hler
	TCHAR sStr[100];		// String f¸r Ausgabe 

	int xmax,ymax;		// Auflˆsung des Bildes
	int x,y;			// Laufvariablen durch die Pixel des Bildes
	int BytePerPixel;	// Anzahl Farbkan‰le

	int mitte_x,mitte_y;	// Koordinaten des Schwerpunktes
	int n;					// Anzahl klassifizierter Pixel
	int nz;					// Anzahl klassifizierter Pixel in der Zeile
	int xz;					// X-Summe einer Zeile

	int rahmenDicke;		// wird durch iValue_5 gestellt
	int schwelle;			// wird durch iValue_4 gestellt
	int zeilenSchwelle=10;		

	CPioneerBVDlg* pDlg;	// GUI
	CCamera* pCamera;		
	CImage* pImage;			// Zeiger auf das Image der Camera
	CImage aImage[ANZ_BILD];// Array mit vollst‰ndigen Bildern
	IplImage *p,*ph[ANZ_BILD];	// pIplImage, um ipl-Funktionen zu nutzen
	//Mat mat_p,mat_ph[ANZ_BILD];	// pMat, f¸r zugriff mit OpenCV2-funktionen, die Mat benˆtigen

	// Holen des GUI-Zeigers
	pDlg = ((CPioneerBVApp*)AfxGetApp())->GetDialog();

	InitButtons();

	// Holen der Camera und des Bildes der Camera
	pCamera = pDlg->GetCamera();
	//pCamera ->Start();

	// Viermal ein Bild holen, damit CImage seine beiden Bilder auf die eventuell neue Auflˆsung umstellen kann
	pImage = pCamera->GetFrame();	pCamera->ReleaseFrame( pImage ); 
	pImage = pCamera->GetFrame();	pCamera->ReleaseFrame( pImage ); 
	pImage = pCamera->GetFrame();	pCamera->ReleaseFrame( pImage ); 
	pImage = pCamera->GetFrame();	pCamera->ReleaseFrame( pImage ); 

	p = pImage->GetImage();

	// Erstelle ein Bild vom Datentyp Mat mit den gleichen Dimensionen wie das IplImage
	Mat mat_p = cvarrToMat(p);
	Mat mat_ph[ANZ_BILD];

	// Erzeugen gleichgroﬂer Bildkopien und ihrer pIplImage (Zeiger auf Ipl-Bild)
	for(int b=0;b<ANZ_BILD;b++) {
		aImage[b].Create( pImage->Width(), pImage->Height(), 24 );
		ph[b] = aImage[b].GetImage();		
		// Konvertiere Bild-Metadaten von *IplImage zu cv::Mat, Bilddaten sind gemeinsam! (siehe Operator IplImage der Klasse Mat)
		mat_ph[b] = cvarrToMat(ph[b]);
	}

	// Erstelle eine Bild vom Datentyp Mat mit den gleichen Dimensionen wie das IplImage mit Kopie des Bildinhaltes
	// Mat mat_image2(p,true); // hier nicht

	_stprintf_s(sStr, 100, _T("%d %dx%d Bilder angelegt ..."), ANZ_BILD, p->width, p->height);
	pDlg->Say(sStr);

	pCamera->ReleaseFrame( pImage ); 

	// Init der Variablenfelder
	schwelle = iValue_4 = 40;
	pDlg->SetFieldValue( FIELD_VALUE_4, iValue_4 );

	pDlg->Say(_T("Thread gestartet ..."));


	// und los ...
	while( !bEndThread ){

		// Holen der DialogWerte in sprechende Variablen
		rahmenDicke = iValue_5;
		schwelle = iValue_4;

		// Holen aktuelles Bild
		pImage = pCamera->GetFrame();		// Zeiger auf CImage
		p = pImage->GetImage();				// Zeiger auf IplImage
		// Konvertiere Bild-Metadaten von *IplImage zu cv::Mat, Bilddaten sind gemeinsam
		// Konvertierung erst in der Hauptschleife, da p durch das Grabben zwischen zwei Bildpuffern wechselt
		mat_p = cvarrToMat(p); 

		// Beispiel OpenCV-Funktion: Erosion des Originalbildes nach Bild0
		// OpenCV-Dokumentation unter file:///C:/OpenCV3.2.0/doc/doxygen/html/index.html
		cvErode( p, ph[0], NULL, iValue_3 );

		// Beispiel OpenCV-Funktion: Dilatation des Bild0 nach Bild1
		cvDilate( ph[0], ph[1], NULL, iValue_3 );

		// Beispiel OpenCV-Funktion: Umwandlung RGB nach HSV
		cvCvtColor( ph[1], ph[2], CV_BGR2HSV );

		// Beispiel OpenCV-2-Funktion mit Bildern als Datentyp cv:Mat
		blur( mat_ph[2], mat_ph[3], Size(30,30));

		// Beispiel OpenCV-2-Funktion mit Bildern als Datentyp cv:Mat
		Laplacian( mat_p, mat_ph[4], -1);

		// Ab hier eine einfache Bildverarbeitung, die auf Pixelebene
		// arbeitet und den Schwerpunkt aller Pixel bestimmt, deren 
		// Differenz zwischen zwei (einstellbaren) Farbkan‰len eine
		// (einstellbare) Schwelle ¸berschreitet und die nicht im
		// (einstellbaren) Rahmen liegen

		// Zeiger auf Pixel erzeugen
		BYTE *pix = (BYTE*)pImage->GetImage()->imageData; 

		// Auflˆsung des Bildes bestimmen
		xmax = pImage->Width();
		ymax = pImage->Height();
		BytePerPixel = pImage->Channels();

		mitte_x=0;
		mitte_y=0;
		n=0;
		i = 0;	// Pixelz‰hler

		// alle Bildpunkte durchlaufen
		for(y=0;y<ymax;y++)	{
			nz = xz = 0;
			for (x=0; x < xmax;x++) {
				// Rahmen schwarz ausf¸llen
				if ((x < rahmenDicke) || (y < rahmenDicke) || 
					(x >= xmax - rahmenDicke) || (y >= ymax - rahmenDicke))
				{
					pix[ROT+i] = pix[GRUEN+i] = pix[BLAU+i] = 0;
				}
				// Bildberechnung
				else if ((pix[i+minuend] - pix[i+subtrahend]) > schwelle) 	{

					pix[ROT + i] = 255;		// Rot markieren

					nz++;	// in dieser Zeile ein Pixel mehr gefunden
					xz+=x;	// X-Koordinaten klassifizierter Pixel aufsummieren
				}

				i+=BytePerPixel; // n‰chstes Pixel;

			}
			// Nach jeder Zeile
			if (nz >= zeilenSchwelle)
			{
				n += nz;
				mitte_x += xz;
				mitte_y += (y*nz);
			}
		}

		// schwerpunkt aller klassifizierten Pixel gefunden?
		if (n!=0) {
			// Mittelwerte f¸r X- und Y-Koordinaten (Schwerpunkt) bilden
			mitte_x = mitte_x/n;
			mitte_y = mitte_y/n;

			// Koordinaten des Schwerpunktes ausgeben
			zeichne_kreuz(mitte_x,mitte_y,100,pImage);

			// Werte in die Kommunikationsvariablen schreiben
			shared_mem->SM_SetFloat(1,(float)mitte_x-xmax/2);
			shared_mem->SM_SetFloat(2,(float)mitte_y-xmax/2);
			shared_mem->SM_SetFloat(3,(float)1);
			Sleep(50);
		}
		else {
			// wenn keine Pixel markiert wurden
			shared_mem->SM_SetFloat(3,0);
		}

		// Anzeige der Kommunikationsvariablen
		pDlg->SetFieldValue( FIELD_VALUE_0, (int)*shared_mem->SM_GetFloat(1));
		pDlg->SetFieldValue( FIELD_VALUE_1, (int)*shared_mem->SM_GetFloat(2));
		pDlg->SetFieldValue( FIELD_VALUE_2, (int)*shared_mem->SM_GetFloat(3));

		// Das gew¸nschte Bild anzeigen
		if (iBild_Index == ANZ_BILD) pDlg->ShowImage( pImage );	
		else	pDlg->ShowImage( &(aImage[iBild_Index]));

		// Man muss auch loslassen koennen
		pCamera->ReleaseFrame( pImage );							// Frame freigeben

		dThreadFrameCount += 1;		
	}

	shared_mem->SM_Close();

	::PostMessage( hWnd, WM_WORKER_THREAD_FINISHED, 0, 0 );
	pDlg->Say(_T("Thread gestoppt ..."));
	return 0;
}

void zeichne_kreuz(long x,long y, long breite, CImage *pImage) {
	long i,j,sx,sy,BytePerPixel;
	long size;

	BYTE *pix = (BYTE*)pImage->GetImage()->imageData; 

	sx = pImage->Width();
	sy = pImage->Height();
	BytePerPixel = pImage->Channels();

	size = sx*sy*BytePerPixel;
	// horizontale Linie
	for(i=(y*sx+x-breite)*BytePerPixel+BLAU;i<=(y*sx+x+breite)*BytePerPixel+BLAU;i+=BytePerPixel) 
		if (i>=0  && i< size) pix[i] = 255;
	// vertikale Linie
	for(i=y-breite;i<=y+breite;i++)   {
		j = (i*sx+x)*BytePerPixel+BLAU;
		if (j>=0  && j< size) pix[j] = 255;
	}

	// Zentrum des Kreuzes weiﬂ zeichnen
	pix[(sx*y+x)*BytePerPixel+ROT] = 0;
	pix[(sx*y+x)*BytePerPixel+GRUEN] = 255;
	pix[(sx*y+x)*BytePerPixel+BLAU] = 255;
}
