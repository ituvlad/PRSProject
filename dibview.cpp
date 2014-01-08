// dibview.cpp : implementation of the CDibView class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "diblook.h"
#include <afxdisp.h>
#include <algorithm>
#include <iostream>
#include "Knn.h"
using namespace std;

#include "dibdoc.h"
#include "dibview.h"
#include "dibapi.h"
#include "mainfrm.h"
#include <math.h>
#include "HRTimer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define BEGIN_PROCESSING() INCEPUT_PRELUCRARI()

#define END_PROCESSING(Title) SFARSIT_PRELUCRARI(Title)

#define INCEPUT_PRELUCRARI() \
	CDibDoc* pDocSrc=GetDocument();										\
	CDocTemplate* pDocTemplate=pDocSrc->GetDocTemplate();				\
	CDibDoc* pDocDest=(CDibDoc*) pDocTemplate->CreateNewDocument();		\
	BeginWaitCursor();													\
	HDIB hBmpSrc=pDocSrc->GetHDIB();									\
	HDIB hBmpDest = (HDIB)::CopyHandle((HGLOBAL)hBmpSrc);				\
	if ( hBmpDest==0 ) {												\
	pDocTemplate->RemoveDocument(pDocDest);							\
	return;															\
	}																	\
	BYTE* lpD = (BYTE*)::GlobalLock((HGLOBAL)hBmpDest);					\
	BYTE* lpS = (BYTE*)::GlobalLock((HGLOBAL)hBmpSrc);					\
	int iColors = DIBNumColors((char *)&(((LPBITMAPINFO)lpD)->bmiHeader)); \
	RGBQUAD *bmiColorsDst = ((LPBITMAPINFO)lpD)->bmiColors;	\
	RGBQUAD *bmiColorsSrc = ((LPBITMAPINFO)lpS)->bmiColors;	\
	BYTE * lpDst = (BYTE*)::FindDIBBits((LPSTR)lpD);	\
	BYTE * lpSrc = (BYTE*)::FindDIBBits((LPSTR)lpS);	\
	int dwWidth  = ::DIBWidth((LPSTR)lpS);\
	int dwHeight = ::DIBHeight((LPSTR)lpS);\
	int w=WIDTHBYTES(dwWidth*((LPBITMAPINFOHEADER)lpS)->biBitCount);	\
	HRTimer my_timer;	\
	my_timer.StartTimer();	\

#define BEGIN_SOURCE_PROCESSING \
	CDibDoc* pDocSrc=GetDocument();										\
	BeginWaitCursor();													\
	HDIB hBmpSrc=pDocSrc->GetHDIB();									\
	BYTE* lpS = (BYTE*)::GlobalLock((HGLOBAL)hBmpSrc);					\
	int iColors = DIBNumColors((char *)&(((LPBITMAPINFO)lpS)->bmiHeader)); \
	RGBQUAD *bmiColorsSrc = ((LPBITMAPINFO)lpS)->bmiColors;	\
	BYTE * lpSrc = (BYTE*)::FindDIBBits((LPSTR)lpS);	\
	int dwWidth  = ::DIBWidth((LPSTR)lpS);\
	int dwHeight = ::DIBHeight((LPSTR)lpS);\
	int w=WIDTHBYTES(dwWidth*((LPBITMAPINFOHEADER)lpS)->biBitCount);	\



#define END_SOURCE_PROCESSING	\
	::GlobalUnlock((HGLOBAL)hBmpSrc);								\
	EndWaitCursor();												\
	/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------
#define SFARSIT_PRELUCRARI(Titlu)	\
	double elapsed_time_ms = my_timer.StopTimer();	\
	CString Title;	\
	Title.Format("%s - Proc. time = %.2f ms", Titlu, elapsed_time_ms);	\
	::GlobalUnlock((HGLOBAL)hBmpDest);								\
	::GlobalUnlock((HGLOBAL)hBmpSrc);								\
	EndWaitCursor();												\
	pDocDest->SetHDIB(hBmpDest);									\
	pDocDest->InitDIBData();										\
	pDocDest->SetTitle((LPCSTR)Title);									\
	CFrameWnd* pFrame=pDocTemplate->CreateNewFrame(pDocDest,NULL);	\
	pDocTemplate->InitialUpdateFrame(pFrame,pDocDest);	\

/////////////////////////////////////////////////////////////////////////////
// CDibView

IMPLEMENT_DYNCREATE(CDibView, CScrollView)

	BEGIN_MESSAGE_MAP(CDibView, CScrollView)
		//{{AFX_MSG_MAP(CDibView)
		ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
		ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
		ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
		ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
		ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
		ON_COMMAND(ID_PROCESSING_PARCURGERESIMPLA, OnProcessingParcurgereSimpla)
		//}}AFX_MSG_MAP

		// Standard printing commands
		ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
		ON_COMMAND(ID_LAB1_RANSAC, &CDibView::OnLab1Ransac)
		ON_COMMAND(ID_LAB1_RANSAC32776, &CDibView::OnLab1Ransac32776)
		ON_COMMAND(ID_LAB2_RANSAC, &CDibView::OnLab2Ransac)
		ON_COMMAND(ID_LAB3_HOUGH, &CDibView::OnLab3Hough)
		ON_COMMAND(ID_LAB3_DRAWHOUGH, &CDibView::OnLab3Drawhough)
		ON_COMMAND(ID_LAB4_DT, &CDibView::OnLab4Dt)
		ON_COMMAND(ID_LAB4_PATTERNMATCHING, &CDibView::OnLab4Patternmatching)
		ON_COMMAND(ID_LAB5_MAGNITUDEANDGRADIENT, &CDibView::OnLab5Magnitudeandgradient)
		ON_COMMAND(ID_LAB5_INTERVALCOLORING, &CDibView::OnLab5Intervalcoloring)
		ON_COMMAND(ID_LAB5_CELLDIVISION, &CDibView::OnLab5Celldivision)
		ON_COMMAND(ID_LAB5_HOG, &CDibView::OnLab5Hog)
		ON_COMMAND(ID_LAB5_DEGREEOFSIMILARITY, &CDibView::OnLab5Degreeofsimilarity)
		ON_COMMAND(ID_LAB6_LOAD400, &CDibView::OnLab6Load400)
		ON_COMMAND(ID_LAB6_CORELATIONCOEFFICIENTS, &CDibView::OnLab6Corelationcoefficients)
		ON_COMMAND(ID_LAB6_CORE, &CDibView::OnLab6Core)
		ON_COMMAND(ID_LAB7_ASS1, &CDibView::OnLab7Ass1)
		ON_COMMAND(ID_LAB7_NORMALDISTR, &CDibView::OnLab7Normaldistr)
		ON_COMMAND(ID_LAB7_HISTOGRAMPOINTS, &CDibView::OnLab7Histogrampoints)
		ON_COMMAND(ID_LAB7_BIVARIATEDISTRIBUTION, &CDibView::OnLab7Bivariatedistribution)
		ON_COMMAND(ID_LAB5_LOAD2, &CDibView::OnLab5Load2)
		ON_COMMAND(ID_LAB7_LOADTRAINNONFACE, &CDibView::OnLab7Loadtrainnonface)
		ON_COMMAND(ID_LAB9_PERCEPTRON, &CDibView::OnLab9Perceptron)
		ON_COMMAND(ID_LAB10_LDA, &CDibView::OnLab10Lda)
		ON_COMMAND(ID_LAB11_ADABOOST, &CDibView::OnLab11Adaboost)
		ON_COMMAND(ID_PRS_PROIECT, &CDibView::OnPrsProiect)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CDibView construction/destruction

	CDibView::CDibView()
	{
	}

	CDibView::~CDibView()
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	// CDibView drawing

	void CDibView::OnDraw(CDC* pDC)
	{
		CDibDoc* pDoc = GetDocument();

		HDIB hDIB = pDoc->GetHDIB();
		if (hDIB != NULL)
		{
			LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
			int cxDIB = (int) ::DIBWidth(lpDIB);         // Size of DIB - x
			int cyDIB = (int) ::DIBHeight(lpDIB);        // Size of DIB - y
			::GlobalUnlock((HGLOBAL) hDIB);
			CRect rcDIB;
			rcDIB.top = rcDIB.left = 0;
			rcDIB.right = cxDIB;
			rcDIB.bottom = cyDIB;
			CRect rcDest;
			if (pDC->IsPrinting())   // printer DC
			{
				// get size of printer page (in pixels)
				int cxPage = pDC->GetDeviceCaps(HORZRES);
				int cyPage = pDC->GetDeviceCaps(VERTRES);
				// get printer pixels per inch
				int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
				int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

				//
				// Best Fit case -- create a rectangle which preserves
				// the DIB's aspect ratio, and fills the page horizontally.
				//
				// The formula in the "->bottom" field below calculates the Y
				// position of the printed bitmap, based on the size of the
				// bitmap, the width of the page, and the relative size of
				// a printed pixel (cyInch / cxInch).
				//
				rcDest.top = rcDest.left = 0;
				rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch)
					/ ((double)cxDIB * cxInch));
				rcDest.right = cxPage;
			}
			else   // not printer DC
			{
				rcDest = rcDIB;
			}
			::PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
				&rcDIB, pDoc->GetDocPalette());
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// CDibView printing

	BOOL CDibView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		// default preparation
		return DoPreparePrinting(pInfo);
	}

	/////////////////////////////////////////////////////////////////////////////
	// CDibView commands


	LRESULT CDibView::OnDoRealize(WPARAM wParam, LPARAM)
	{
		ASSERT(wParam != NULL);
		CDibDoc* pDoc = GetDocument();
		if (pDoc->GetHDIB() == NULL)
			return 0L;  // must be a new document

		CPalette* pPal = pDoc->GetDocPalette();
		if (pPal != NULL)
		{
			CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
			ASSERT_KINDOF(CMainFrame, pAppFrame);

			CClientDC appDC(pAppFrame);
			// All views but one should be a background palette.
			// wParam contains a handle to the active view, so the SelectPalette
			// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
			CPalette* oldPalette = appDC.SelectPalette(pPal, ((HWND)wParam) != m_hWnd);

			if (oldPalette != NULL)
			{
				UINT nColorsChanged = appDC.RealizePalette();
				if (nColorsChanged > 0)
					pDoc->UpdateAllViews(NULL);
				appDC.SelectPalette(oldPalette, TRUE);
			}
			else
			{
				TRACE0("\tSelectPalette failed in CDibView::OnPaletteChanged\n");
			}
		}

		return 0L;
	}

	void CDibView::OnInitialUpdate()
	{
		CScrollView::OnInitialUpdate();
		ASSERT(GetDocument() != NULL);

		SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
	}


	void CDibView::OnActivateView(BOOL bActivate, CView* pActivateView,
		CView* pDeactiveView)
	{
		CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);

		if (bActivate)
		{
			ASSERT(pActivateView == this);
			OnDoRealize((WPARAM)m_hWnd, 0);   // same as SendMessage(WM_DOREALIZE);
		}
	}

	void CDibView::OnEditCopy()
	{
		CDibDoc* pDoc = GetDocument();
		// Clean clipboard of contents, and copy the DIB.

		if (OpenClipboard())
		{
			BeginWaitCursor();
			EmptyClipboard();
			SetClipboardData (CF_DIB, CopyHandle((HANDLE) pDoc->GetHDIB()) );
			CloseClipboard();
			EndWaitCursor();
		}
	}



	void CDibView::OnUpdateEditCopy(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable(GetDocument()->GetHDIB() != NULL);
	}


	void CDibView::OnEditPaste()
	{
		HDIB hNewDIB = NULL;

		if (OpenClipboard())
		{
			BeginWaitCursor();

			hNewDIB = (HDIB) CopyHandle(::GetClipboardData(CF_DIB));

			CloseClipboard();

			if (hNewDIB != NULL)
			{
				CDibDoc* pDoc = GetDocument();
				pDoc->ReplaceHDIB(hNewDIB); // and free the old DIB
				pDoc->InitDIBData();    // set up new size & palette
				pDoc->SetModifiedFlag(TRUE);

				SetScrollSizes(MM_TEXT, pDoc->GetDocSize());
				OnDoRealize((WPARAM)m_hWnd,0);  // realize the new palette
				pDoc->UpdateAllViews(NULL);
			}
			EndWaitCursor();
		}
	}


	void CDibView::OnUpdateEditPaste(CCmdUI* pCmdUI)
	{
		pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));
	}

	void CDibView::OnProcessingParcurgereSimpla() 
	{
		// TODO: Add your command handler code here
		BEGIN_PROCESSING();

		// Makes a grayscale image by equalizing the R, G, B components from the LUT
		for (int k=0;  k < iColors ; k++)
			bmiColorsDst[k].rgbRed=bmiColorsDst[k].rgbGreen=bmiColorsDst[k].rgbBlue=k;


		//  Goes through the bitmap pixels and performs their negative	
		for (int i=0;i<dwHeight;i++)
			for (int j=0;j<dwWidth;j++)
			{	
				lpDst[i*w+j]= 255 - lpSrc[i*w+j]; //makes image negative
			}

			END_PROCESSING("Negativ imagine");
	}

	void CDibView::OnLab1Ransac()
	{

	}

	typedef struct Point
	{
		int X;
		int Y;
	};

	double ComputeDistance(double a, double b, double c, Point p) 
	{
		double up = abs(a * p.X + b * p.Y + c);
		double down = sqrt(a*a + b * b);
		return up / down;
	}


	struct StructureLine{
		float a;
		float b;
		float c;
	};
	Point Points[999];
	Point resultP1;
	Point resultP2;

	int initPoints(BYTE* lpSrc, int dwHeight,int dwWidth,int w){
		int index = 0;
		for (int i=0;i<dwHeight;i++)
			for (int j=0;j<dwWidth;j++)
			{	
				if(lpSrc[i*w+j]== 0)
				{
					Points[index].X = j;
					Points[index].Y = i;
					index++;
				}

			}
			return index;
	}

	void RANSAC(int N, int counter,int t, int T, int maxInliers){
		int rand1;
		int rand2;
		for(int i=0; i<N; i++)
		{
			rand1 = rand() % counter;
			rand2 = rand() % counter;

			while(rand1 == rand2)
			{
				rand1 = rand() % counter;
				rand2 = rand() % counter;
			}

			double a = Points[rand1].Y - Points[rand2].Y;
			double b = Points[rand2].X - Points[rand1].X;
			double c = Points[rand1].X * Points[rand2].Y - Points[rand2].X * Points[rand1].Y;

			int nrInliners = 0;

			for (int k =0; k < counter; k++)
			{
				if ( ComputeDistance(a,b,c, Points[k]) < t)
					nrInliners++;
			}

			if(nrInliners >= T) 
			{
				resultP1 = Points[rand1];
				resultP2 = Points[rand2];
				break;
			}
			else
			{
				if(nrInliners > maxInliers)
				{
					maxInliers = nrInliners;
					resultP1 = Points[rand1];
					resultP2 = Points[rand2];
				}
			}
		}
	}

	Point centerResult;
	double radiusResult;
	void RANSACCircle(int N, int counter,int t, int T){
		int maxInliers = 0;
		int rand1=0;
		int rand2=0;
		int rand3=0;
		double ma=0,mb=0;
		Point center;
		double radius=0;

		for(int i=0; i < N; i++)
		{
			do{
				rand1 = rand() % counter;
				rand2 = rand() % counter;
				rand3 = rand() % counter;
				ma = ((double)(Points[rand2].Y - Points[rand1].Y)) / ((double)(Points[rand2].X - Points[rand1].X));
				mb = ((double)(Points[rand3].Y - Points[rand2].Y)) / ((double)(Points[rand3].X - Points[rand2].X));
			}
			while (rand1 == rand2 || rand2 == rand3 || rand1 == rand3
				|| Points[rand1].X == Points[rand2].X
				|| Points[rand2].X == Points[rand3].X
				|| ma == 0
				|| mb == 0
				|| ma == mb);

			center.X = (ma*mb*(Points[rand1].Y - Points[rand3].Y) + mb*(Points[rand1].X + Points[rand2].X) - ma*(Points[rand2].X + Points[rand3].X)) / (float) (2*(mb-ma));
			center.Y = (-1/(float)ma) * (center.X - (Points[rand1].X + Points[rand2].X)/(float)2) + (Points[rand1].Y + Points[rand2].Y)/(float)2;

			radius = sqrt ((double) (center.X - Points[rand1].X)*(center.X - Points[rand1].X) + (center.Y - Points[rand1].Y)*(center.Y - Points[rand1].Y) );

			int nrInliers = 0;

			for (int k =0; k < counter; k++)
			{
				double d = abs ((sqrt ((double) (center.X - Points[k].X) * (center.X - Points[k].X) + (center.Y - Points[k].Y) * (center.Y - Points[k].Y) )) - radius );
				if ( d < t)
					nrInliers++;
			}

			if(nrInliers >= T) 
			{
				centerResult = center;
				radiusResult = radius;
				break;
			}
			else
			{
				if(nrInliers > maxInliers)
				{
					maxInliers = nrInliers;
					centerResult = center;
					radiusResult = radius;
				}
			}
		}
	}


	void CDibView::OnLab1Ransac32776()
	{
		BEGIN_PROCESSING();
		int t = 10;
		double p = 0.99;
		double W = 0.3;
		int s = 2;

		int counter = initPoints(lpSrc,dwHeight,dwWidth,w);

		int limit = log(1-p) / log(1-W*W);

		double T = W * counter;

		int maxLiners = 0;

		RANSAC(limit, counter,t, T, maxLiners);


		CDC dc;
		dc.CreateCompatibleDC(0);
		CBitmap ddBitmap;
		HBITMAP hDDBitmap = CreateDIBitmap(::GetDC(0),
			&((LPBITMAPINFO)lpS)->bmiHeader, CBM_INIT, lpSrc,
			(LPBITMAPINFO) lpS, DIB_RGB_COLORS);

		ddBitmap.Attach(hDDBitmap);
		CBitmap* pTempBmp = dc.SelectObject(&ddBitmap);

		CPen pen(PS_SOLID, 1, RGB(255,0,0));
		CPen *pTempPen = dc.SelectObject(&pen);

		dc.MoveTo(resultP1.X,dwHeight-1-resultP1.Y);
		dc.LineTo(resultP2.X,dwHeight-1-resultP2.Y);

		dc.SelectObject(pTempPen);
		dc.SelectObject(pTempBmp);

		GetDIBits(dc.m_hDC, ddBitmap, 0, dwHeight, lpDst,
			(LPBITMAPINFO)lpD, DIB_RGB_COLORS);


		END_PROCESSING("RASAC Line");
	}


	void CDibView::OnLab2Ransac()
	{
		BEGIN_PROCESSING();
		int t = 10;
		double p = 0.99;
		double W = 0.5;

		int counter = initPoints(lpSrc,dwHeight,dwWidth,w);

		int N = log(1-p) / log(1-W*W*W);

		double T = W * counter;

		radiusResult = 0;

		RANSACCircle(N, counter,t, T);


		CDC dc; 
		dc.CreateCompatibleDC(0); 
		CBitmap ddBitmap; 
		HBITMAP hDDBitmap = CreateDIBitmap(::GetDC(0), 
			&((LPBITMAPINFO)lpS)->bmiHeader, CBM_INIT, lpSrc, 
			(LPBITMAPINFO)lpS, DIB_RGB_COLORS); 

		ddBitmap.Attach(hDDBitmap); 
		CBitmap* pTempBmp = dc.SelectObject(&ddBitmap); 
		CPen pen(PS_SOLID, 1, RGB(255,0,0)); 
		CPen *pTempPen = dc.SelectObject(&pen); 

		/* draw a circle having radius r and center a point of 
		coordinates (x,y)*/ 

		dc.MoveTo ( (int)(centerResult.X + radiusResult), dwHeight-1-centerResult.Y ); 
		dc.AngleArc(centerResult.X, dwHeight-1-centerResult.Y, radiusResult, 0, 360); 

		dc.SelectObject(pTempPen); 
		dc.SelectObject(pTempBmp); 
		GetDIBits(dc.m_hDC, ddBitmap, 0, dwHeight, lpDst, 
			(LPBITMAPINFO)lpD, DIB_RGB_COLORS); 


		END_PROCESSING("RANSAC Line");
	}

	double HoughAccumulator[1000][1000];

	int hsize;

	void CDibView::OnLab3Hough()
	{
		BEGIN_PROCESSING();
		int dd=1;

		int p=1;
		int dmax = 360;

		double diagonal = sqrt ((double) dwHeight*dwHeight + dwWidth*dwWidth);

		int size = 360 * (diagonal + 1);
		double **accumulator = (double **)malloc((diagonal+1) * sizeof(double*));
		for(int i = 0; i < diagonal+1; i++) accumulator[i] = (double *)malloc(360 * sizeof(double));

		for (int i=0;i < diagonal+1; i++)
			for (int j=0; j<360;j++)
				accumulator[i][j] = 0;

		for (int i=0;i < dwHeight; i++)
			for (int j=0; j<dwWidth;j++){
				if (lpSrc[i*w+j] == 255){
					for (int k=0;k<360;k+=dd){
						double radiani = k * 3.1415 / 180.0;
						int ro =(int) (i*cos(radiani) + j*sin(radiani));
						if (ro > 0 && ro<diagonal + 1) 
							accumulator[ro][k] ++;
					}
				}
			}
			for (int i=0;i<diagonal+1;i++)
				for (int j=0;j<360;j++){
					HoughAccumulator[i][j] = accumulator[i][j]*3;
					//lpDst[i*w+j] = accumulator[i][j];
				}

				hsize = diagonal + 1;


				END_PROCESSING("RANSAC Line");

	}


	struct hafShit{
		int i;
		int j;
		int val;
	};


	hafShit peaks[1000];
	hafShit peaksSorted[1000];

	Point intersectsWith(int ro, int theta){
		int y = 0;
		double radiani = theta * 3.1415 / 180.0;
		int x = ro/cos(radiani);

		Point dr;
		dr.X = x;
		dr.Y = y;

		return dr;
	}

	void CDibView::OnLab3Drawhough()
	{
		BEGIN_PROCESSING();
		int windowSize=3;

		int margin = windowSize/2;


		for (int i=0;i < hsize; i++){
			for (int j=0; j<360;j++){
				lpSrc[i*w+j] = HoughAccumulator[i][j];
			}
		}
		bool isMax=true;
		int peakCounter=0;
		for (int i=margin;i<hsize-margin;i++){
			for (int j=margin;j<360-margin;j++){
				isMax = true;
				int max = HoughAccumulator[i][j];
				for(int k = -margin; k<margin+1; k++){
					for(int l = -margin; l<margin+1; l++){
						if(HoughAccumulator[i][j] < HoughAccumulator[k+margin][l+margin]){
							isMax = false;	
						}
					}
				}
				if(isMax){
					peaks[peakCounter].val = max;
					peaks[peakCounter].i = i;
					peaks[peakCounter].j = j;
					peakCounter++;
				}
			}
		}
		hafShit aux;
		for(int i=0;i<peakCounter-1;i++){
			for(int j=i+1;j<peakCounter;j++){
				if(peaks[i].val < peaks[j].val){
					aux = peaks[j];
					peaks[j]=peaks[i];
					peaks[i]=aux;
				}
			}
		}

		int k=10;

		CDC dc;
		dc.CreateCompatibleDC(0);
		CBitmap ddBitmap;
		HBITMAP hDDBitmap = CreateDIBitmap(::GetDC(0),
			&((LPBITMAPINFO)lpS)->bmiHeader, CBM_INIT, lpSrc,
			(LPBITMAPINFO) lpS, DIB_RGB_COLORS);

		ddBitmap.Attach(hDDBitmap);
		CBitmap* pTempBmp = dc.SelectObject(&ddBitmap);

		CPen pen(PS_SOLID, 1, RGB(255,0,0));
		CPen *pTempPen = dc.SelectObject(&pen);

		for (int i=k;i>0;i--){
			Point p = intersectsWith(peaks[i].i, peaks[i].j);

			dc.MoveTo(p.X,p.Y);
			dc.LineTo(peaks[i].i,peaks[i].j);

		}



		dc.SelectObject(pTempPen);
		dc.SelectObject(pTempBmp);

		GetDIBits(dc.m_hDC, ddBitmap, 0, dwHeight, lpDst,
			(LPBITMAPINFO)lpD, DIB_RGB_COLORS);



		END_PROCESSING("RANSAC Line");
	}



	int dt[3000][3000];
	void CDibView::OnLab4Dt()
	{
		BEGIN_PROCESSING();

		for (int i=0;i < dwHeight; i++){
			for (int j=0; j<dwWidth;j++){
				dt[i][j] = 0;
			}
		}
		for (int i=1;i < dwHeight-1; i++){
			for (int j=1; j<dwWidth-1;j++){
				if (lpSrc[i*w+j] == 0)
					dt[i][j] = 0;
				else 
					dt[i][j] = 255;
			}
		}

		int mask[3][3] = {{3,2,3},{2,0,2},{3,2,3}};


		for (int i = 1; i < dwHeight; i++)
		{
			for (int j = 1; j < dwWidth-1; j++){
				int val1 = dt[(i-1)][(j-1)] + mask[0][0];
				int val2 = dt[(i-1)][(j+0)] + mask[0][1];
				int val3 = dt[(i-1)][(j+1)] + mask[0][2];
				int val4 = dt[(i+0)][(j-1)] + mask[1][0];
				int min1 = val1 > val2 ? val2 : val1;
				int min2 = val3 > val4 ? val4 : val3;
				int min = min1 > min2 ? min2 : min1;
				if (lpSrc[i*w+j] > min)
					dt[i][j] = min;
			}
		}
		for (int i = dwHeight - 2; i >=0; i--){
			for (int j = dwWidth - 2; j >= 1; j--) {
				int val1 = dt[(i+1)][(j+1)] + mask[2][2];
				int val2 = dt[(i+1)][(j+0)] + mask[2][1];
				int val3 = dt[(i+1)][(j-1)] + mask[2][0];
				int val4 = dt[(i+0)][(j+1)] + mask[1][2];
				int min1 = val1 > val2 ? val2 : val1;
				int min2 = val3 > val4 ? val4 : val3;
				int min = min1 > min2 ? min2 : min1;
				if (min < dt[i][j])
					dt[i][j] = min;
			}
		}


		for (int i = 0; i < dwHeight; i++)
		{
			for (int j = 0; j < dwWidth; j++){
				lpDst[i*w+j] = dt[i][j];
			}
		}

		END_PROCESSING("RANSAC Line");
	}

	int patternMatches[1000];
	int patternCounter = -1;
	void CDibView::OnLab4Patternmatching()
	{
		BEGIN_PROCESSING();

		for (int i=0;i < dwHeight; i++){
			for (int j=0; j<dwWidth;j++){
				if (lpSrc[i*w+j] == 0){
					patternMatches[patternCounter++] = dt[i][j];
				}
			}
		}

		int average = 0;
		int sum = 0;
		for (int i=0;i<patternCounter;i++){
			sum += patternMatches[i];
		}
		average = sum / patternCounter;

		CString str ;
		str.Format("%d",average);
		AfxMessageBox(_T(str));


		END_PROCESSING("RANSAC Line");
	}

	struct rgb{
		int r;
		int g;
		int b;
	};

	int getColorForAngle(double angle,RGBQUAD *bmiColorsDst){

		if (angle >= 0 && angle < 40){
			bmiColorsDst[0].rgbRed = 255;
			bmiColorsDst[0].rgbGreen = 0;
			bmiColorsDst[0].rgbBlue = 0;
			return 0;
		}
		if (angle >= 40 && angle < 80){
			bmiColorsDst[1].rgbRed = 255;
			bmiColorsDst[1].rgbGreen = 128;
			bmiColorsDst[1].rgbBlue = 0;
			return 1;
		}
		if (angle >= 80 && angle < 120){
			bmiColorsDst[2].rgbRed = 255;
			bmiColorsDst[2].rgbGreen = 255;
			bmiColorsDst[2].rgbBlue = 0;
			return 2;
		}
		if (angle >= 120 && angle < 160){
			bmiColorsDst[3].rgbRed = 0;
			bmiColorsDst[3].rgbGreen = 255;
			bmiColorsDst[3].rgbBlue = 0;
			return 3;
		}
		if (angle >= 160 && angle < 200){
			bmiColorsDst[4].rgbRed = 0;
			bmiColorsDst[4].rgbGreen = 255;
			bmiColorsDst[4].rgbBlue = 128;
			return 4;
		}
		if (angle >= 200 && angle < 240){
			bmiColorsDst[5].rgbRed = 0;
			bmiColorsDst[5].rgbGreen = 255;
			bmiColorsDst[5].rgbBlue = 255;
			return 5;
		}
		if (angle >= 240 && angle < 280){
			bmiColorsDst[6].rgbRed = 0;
			bmiColorsDst[6].rgbGreen = 0;
			bmiColorsDst[6].rgbBlue = 255;
			return 6;
		}
		if (angle >= 280 && angle < 320){
			bmiColorsDst[7].rgbRed = 128;
			bmiColorsDst[7].rgbGreen = 0;
			bmiColorsDst[7].rgbBlue = 255;
			return 7;
		}
		if (angle >= 320 && angle < 360){
			bmiColorsDst[8].rgbRed = 0;
			bmiColorsDst[8].rgbGreen = 0;
			bmiColorsDst[8].rgbBlue = 255;
			return 8;
		}
	}
	BYTE *InitialImage;
	int numberOfDivisions;
	int numberOfDivisions2;
	BYTE *CellDividedImage;

	void CDibView::OnLab5Magnitudeandgradient()
	{
		BEGIN_PROCESSING();
		//InitialImage = lpSrc;

		int dx[3] = {-1,0,1};
		int dy[3] = {1,0,1};
		int ix,iy;

		for(int i=1;i < dwHeight-1; i++){
			for (int j=2; j<dwWidth-2;j++){
				ix = abs((double)(-lpSrc[i*w+j-1] + lpSrc[i*w+j+1])/2);
				if (ix > 255)
					ix = 255;
				if (ix < 0)
					ix = 0;
				iy = abs((double)(-lpSrc[(i-1)*w+j] + lpSrc[(i+1)*w+j])/2);
				if (iy > 255)
					iy = 255;
				if (iy < 0)
					iy = 0;

				int gradient = sqrt (ix*ix + iy*iy);
				lpDst[i*w+j] = gradient;
				double theta = atan2(iy,ix);

				double alfa = theta * 180 / 3.1415;
				double alfasigned;
				if (alfa >= 0)
					alfasigned = alfa;
				else 
					alfasigned = alfa + 360;
			}
		}


		END_PROCESSING("RANSAC Line");
	}


	void CDibView::OnLab5Intervalcoloring()
	{
		BEGIN_PROCESSING();


		int dx[3] = {-1,0,1};
		int dy[3] = {1,0,1};
		int ix,iy;

		for(int i=1;i < dwHeight-1; i++){
			for (int j=2; j<dwWidth-2;j++){
				ix = ((double)(-lpSrc[i*w+j-1] + lpSrc[i*w+j+1]));

				iy = ((double)(-lpSrc[(i-1)*w+j] + lpSrc[(i+1)*w+j]));


				int gradient = sqrt (ix*ix + iy*iy);
				double theta = atan2(iy,ix);

				double alfa = theta * 180 / 3.1415;
				double alfasigned;
				if (alfa >= 0)
					alfasigned = alfa;
				else 
					alfasigned = alfa + 360;
				int color = getColorForAngle(alfasigned,bmiColorsDst);


				lpDst[i*w+j] = color;
			}
		}


		END_PROCESSING("RANSAC Line");
	}
	int cellWidth = 16;
	int cellHeight = 16;
	int numberOfRuns = 0;
	int nrCell1 = 0;
	int nrCell2 = 0;
	void CDibView::OnLab5Celldivision()
	{
		BEGIN_PROCESSING();

		int dx[3] = {-1,0,1};
		int dy[3] = {1,0,1};
		int ix,iy;

		int colors[500];
		for (int i=0;i<500;i++)
			colors[i]= 0;
		int count = 0;
		BYTE *aux;
		aux = lpDst;
		int hist[9];
		for (int i=0;i<9;i++)
			hist[i] = 0;

		for(int i=1;i < dwHeight-1; i++){
			for (int j=2; j<dwWidth-2;j++){
				ix = ((double)(-lpSrc[i*w+j-1] + lpSrc[i*w+j+1]));
				iy = ((double)(-lpSrc[(i-1)*w+j] + lpSrc[(i+1)*w+j]));

				int gradient = sqrt (ix*ix + iy*iy);
				lpDst[i*w+j] = gradient;
				double theta = atan2(iy,ix);
				double alfa = theta * 180 / 3.1415;
				double alfasigned;
				if (alfa >= 0)
					alfasigned = alfa;
				else 
					alfasigned = alfa + 360;
				int color = getColorForAngle(alfasigned,bmiColorsDst);

				aux [i*w+j] = color;

			}
		}
		for(int i=0;i < dwHeight/cellHeight; i++){
			for (int j=0; j<dwWidth/cellWidth;j++){
				for (int i=0;i<9;i++)
					hist[i] = 0;
				for (int k = i*cellHeight; k < cellHeight + i*cellHeight; k++){
					for (int l = j*cellWidth; l < cellWidth + j*cellWidth; l++){
						int val = aux[k*w+l];
						hist[val]++;
					}
				}
				int max = 0;
				int index = 0;
				for (int i=0;i<9;i++){
					if (hist[i] > max) {
						max = hist[i];
						index = i;
					}
				}

				for (int k = i*cellHeight; k < cellHeight + i*cellHeight; k++){
					for (int l = j*cellWidth; l < cellWidth + j*cellWidth; l++){
						lpDst[k*w+l] = index;
					}
				}
			}
		}
		/*if (numberOfRuns == 0) {
		InitialImage = lpDst;
		numberOfRuns++;
		}
		else {
		CellDividedImage = lpDst;
		numberOfDivisions = dwHeight/cellHeight;
		}*/

		END_PROCESSING("RANSAC Line");
	}


	void CDibView::OnLab5Hog()
	{
		BEGIN_PROCESSING();
		InitialImage = lpSrc;
		numberOfDivisions = 32;
		END_PROCESSING("RANSAC Line");
	}

	void CDibView::OnLab5Load2()
	{
		BEGIN_PROCESSING();
		CellDividedImage = lpSrc;
		numberOfDivisions2 = 32;
		END_PROCESSING("RANSAC Line");
	}


	void CDibView::OnLab5Degreeofsimilarity()
	{
		BEGIN_PROCESSING();

		double similarity = 0;
		double upTerm = 0;
		double downTerm = 0;
		double downTerm1 = 0;
		double downTerm2 = 0;

		for(int i=0;i<numberOfDivisions;i++) {
			upTerm += InitialImage[i]*CellDividedImage[i];
		}

		for(int i=0;i<numberOfDivisions;i++) {

			downTerm1 += InitialImage[i]*InitialImage[i];
		}

		for(int i=0;i<numberOfDivisions;i++) {
			downTerm2 += CellDividedImage[i]*CellDividedImage[i];
		}

		downTerm = sqrt(downTerm1) * sqrt(downTerm2);

		similarity = upTerm / downTerm;

		CString info;

		info.Format("Similarity is %f", similarity);
		AfxMessageBox(info);

		END_PROCESSING("RANSAC Line");
	}

	int matrice[400][361];
	void CDibView::OnLab6Load400()
	{
		BEGIN_PROCESSING(); 

		BYTE *lpSI,*lpSrcI; 
		DWORD dwWidthI,dwHeightI,wI; 
		HDIB hBmpSrcI; 
		CFile fileIn; 
		CFileException fE; 
		FILE *f = fopen("matrix.txt","w+");

		AfxEnableControlContainer(); 
		char buffer[MAX_PATH]; 
		BROWSEINFO bi; 
		ZeroMemory(&bi, sizeof(bi)); 
		SHGetPathFromIDList(SHBrowseForFolder(&bi), buffer); 
		if (strcmp(buffer,"")==0) return; 

		char directoryPath[MAX_PATH]; 
		CFileFind fFind; 
		int nextFile; 
		CString msg; 
		strcpy(directoryPath,buffer); 
		strcat(directoryPath,"\\face*.bmp"); 
		nextFile=fFind.FindFile(directoryPath); 
		int nrImages=0; 

		while (nextFile) 
		{ 
			nrImages++; 
			nextFile=fFind.FindNextFile(); 
			CString fnIn=fFind.GetFilePath(); 
			fileIn.Open(fnIn, CFile::modeRead | CFile::shareDenyWrite, &fE); 
			hBmpSrcI = (HDIB)::ReadDIBFile(fileIn); 
			fileIn.Close(); 
			lpSI = (BYTE*)::GlobalLock((HGLOBAL)hBmpSrcI); 
			dwWidthI = ::DIBWidth((LPSTR)lpSI); 
			dwHeightI = ::DIBHeight((LPSTR)lpSI); 
			lpSrcI=(BYTE*)::FindDIBBits((LPSTR)lpSI); 
			DWORD wI=WIDTHBYTES(dwWidthI*8); 
			if (nrImages > 1){
				fprintf(f,"\n");
			}

			for (int i=0;i<dwHeightI;i++)
				for (int j=0;j<dwWidthI;j++){
					fprintf (f,"%d ",lpSrcI[i*wI+j]);
				}


				for(int i=0; i < 361; i++){
					matrice[nrImages-1][i] = lpSrcI[i];
				}


				///////////// DO THE PROCESSING HERE 

				::GlobalUnlock((HGLOBAL)hBmpSrcI); 
		} 
		//msg.Format("Found and processed %d images",nrImages); 
		//AfxMessageBox(msg); 
		fclose(f);
		END_PROCESSING("Covariance");
	}

	double meanValues[400];
	double standardDeviation[400];
	double covarianceMatrix[400][361];
	double correlationCoefficient[400][361];

	void computeMean(){
		for(int i=0;i<361;i++){
			for(int j=0; j < 400; j++){
				meanValues[i] += matrice[j][i];
			}
		}
		for(int i=0;i<400;i++){
			meanValues[i] /= 400;
		}
	}

	void computeStandardDeviation(){
		for(int j=0; j < 361; j++){
			for(int i=0;i<400;i++){
				standardDeviation[i] = pow(matrice[i][j] - meanValues[i], 2); 
			}
		}
		for(int i=0;i<400;i++){
			standardDeviation[i] /= 400;
			standardDeviation[i] = sqrt(standardDeviation[i]);
		}
	}

	void computeCovarianceMatrix(){
		for(int j=0; j < 361; j++){
			for(int i=0;i<400;i++){
				covarianceMatrix[i][j] = (matrice[i][j] - meanValues[j]) * (matrice[j][i] - meanValues[i]); 
			}
		}

		for(int j=0; j < 361; j++){
			for(int i=0;i<400;i++){
				covarianceMatrix[i][j] /= 400; 
			}
		}
	}

	void computeCorrelationCoefficient(){
		for(int j=0; j < 361; j++){
			for(int i=0;i<400;i++){

				correlationCoefficient[i][j] = covarianceMatrix[i][j] / (standardDeviation[i] * standardDeviation[j]); 
			}
		}
	}
	int smth[361][400];
	void CDibView::OnLab6Corelationcoefficients()
	{
		BEGIN_PROCESSING();

		computeMean();
		computeStandardDeviation();
		computeCovarianceMatrix();
		computeCorrelationCoefficient();


		for (int i=0;i<400;i++){
			for (int j=0;j<361;j++){
				smth[i][j] = (int) correlationCoefficient[i][j];
			}
		}

		double score = correlationCoefficient[13*19+4][13*19+14];
		/*for (int i=0;i<400;i++){
		for (int j=0;j<361;j++){
		int img = matrice[i][j];
		lpDst[matrice[i][j]*w + smth[i][j]] = 0;
		}
		}*/

		END_PROCESSING("lab6");


	}




	void CDibView::OnLab6Core()
	{
		// TODO: Add your command handler code here
	}

	int facesTrainArray[400][400];
	int nonFacesTrainArray[400][400];

	void loadImages (bool isTrainSet){

		BYTE *lpSI,*lpSrcI; 
		DWORD dwWidthI,dwHeightI,wI; 
		HDIB hBmpSrcI; 
		CFile fileIn; 
		CFileException fE; 

		AfxEnableControlContainer(); 
		char buffer[MAX_PATH]; 
		BROWSEINFO bi; 
		ZeroMemory(&bi, sizeof(bi)); 
		SHGetPathFromIDList(SHBrowseForFolder(&bi), buffer); 
		if (strcmp(buffer,"")==0) return; 

		char directoryPath[MAX_PATH]; 
		CFileFind fFind; 
		int nextFile; 
		CString msg; 
		//strcpy(directoryPath,"C:\\Users\\Vlad\\Dropbox\\school\\PRS\\Lab7\\train_imgs");
		strcpy(directoryPath,buffer); 
		if (isTrainSet)
			strcat(directoryPath,"\\trainFace_*.bmp"); 
		else
			strcat(directoryPath,"\\trainNonFace_*.bmp"); 
		nextFile=fFind.FindFile(directoryPath); 
		int nrImages=0; 

		while (nextFile) 
		{ 
			nrImages++; 
			nextFile=fFind.FindNextFile(); 
			CString fnIn=fFind.GetFilePath(); 
			fileIn.Open(fnIn, CFile::modeRead | CFile::shareDenyWrite, &fE); 
			hBmpSrcI = (HDIB)::ReadDIBFile(fileIn); 
			fileIn.Close(); 
			lpSI = (BYTE*)::GlobalLock((HGLOBAL)hBmpSrcI); 
			dwWidthI = ::DIBWidth((LPSTR)lpSI); 
			dwHeightI = ::DIBHeight((LPSTR)lpSI); 
			lpSrcI=(BYTE*)::FindDIBBits((LPSTR)lpSI); 
			DWORD wI=WIDTHBYTES(dwWidthI*8); 

			if (isTrainSet){
				for(int i=0; i < 400; i++){
					facesTrainArray[nrImages-1][i] = lpSrcI[i];
				}
			}
			else {
				for(int i=0; i < 400; i++){
					nonFacesTrainArray[nrImages-1][i] = lpSrcI[i];
				}
			}
			///////////// DO THE PROCESSING HERE 

			::GlobalUnlock((HGLOBAL)hBmpSrcI); 
		} 
		msg.Format("Found and processed %d images",nrImages); 
		AfxMessageBox(msg); 
	}


	void CDibView::OnLab7Ass1()
	{
		loadImages(true);
	}

	void CDibView::OnLab7Loadtrainnonface()
	{
		loadImages(false);
	}

	int mahanttanFaces[160];
	int mahanttanNonFaces[160];
	int selected[100];
	void CDibView::OnLab7Normaldistr()
	{
		BEGIN_PROCESSING();
		CKnn dlg;
		dlg.m_K = 5;
		dlg.DoModal();


		int kapa = dlg.m_K;

		loadImages(true);
		loadImages(false);
		int nbFaces =0 ;
		int nbNonFaces=0;

		for (int n = 0; n < 160; n++)
		{
			for (int i = 0; i < 20; i++)
			{
				for (int j =0; j < 20 ; j++)
				{
					mahanttanFaces[n] += abs (lpSrc[i*w+j] - facesTrainArray[n][i*w+j]);	
				}
			}
		}

		for (int n = 0; n < 160; n++)
		{
			for (int i = 0; i < 20; i++)
			{
				for (int j =0; j < 20 ; j++)
				{
					mahanttanNonFaces[n] += abs (lpSrc[i*w+j] - nonFacesTrainArray[n][i*w+j]);	
				}
			}
		}

		for (int i=0;i<kapa;i++)
		{
			int minFaces=0;
			int minNonFaces=0;
			int posFaces=0,posNonFaces=0;
			for (int j=0;j<160;j++)
			{
				if (minFaces < mahanttanFaces [j]){
					minFaces = mahanttanFaces [j];
					posFaces = j;
				}
			}
			for (int j=0;j<160;j++)
			{
				if (minNonFaces < mahanttanNonFaces [j]){
					minNonFaces = mahanttanNonFaces [j];
					posNonFaces = j;
				}
			}
			if (minFaces < minNonFaces ) {
				mahanttanFaces[posFaces] = -102000;
				nbFaces++;
			}
			else{
				mahanttanNonFaces[posNonFaces] = -102000;
				nbNonFaces++;
			}
		}

		CString msg;
		if (nbFaces > nbNonFaces){
			msg.Format("The selected image is a face"); 
		}
		else
			msg.Format("The selected image is not a face"); 
		AfxMessageBox(msg); 

		END_PROCESSING("Knn manhattan");
	}

	int mahanttanFaces2[160];
	int mahanttanNonFaces2[160];
	void CDibView::OnLab7Histogrampoints()
	{
		BEGIN_PROCESSING();
		CKnn dlg;
		dlg.m_K = 5;
		dlg.DoModal();


		int kapa2 = dlg.m_K;

		loadImages(true);
		loadImages(false);
		int nbFaces =0 ;
		int nbNonFaces=0;

		for (int n = 0; n < 160; n++)
		{
			for (int i = 0; i < 20; i++)
			{
				for (int j =0; j < 20 ; j++)
				{
					mahanttanFaces2[n] += abs (lpSrc[i*w+j] - facesTrainArray[n][i*w+j]) * abs (lpSrc[i*w+j] - facesTrainArray[n][i*w+j]);	
				}
			}
		}

		for (int n = 0; n < 160; n++)
		{
			for (int i = 0; i < 20; i++)
			{
				for (int j =0; j < 20 ; j++)
				{
					mahanttanNonFaces2[n] += abs (lpSrc[i*w+j] - nonFacesTrainArray[n][i*w+j]) * abs (lpSrc[i*w+j] - facesTrainArray[n][i*w+j]);	
				}
			}
		}

		for (int i=0;i<kapa2;i++)
		{
			int minFaces=0;
			int minNonFaces=0;
			int posFaces=0,posNonFaces=0;
			for (int j=0;j<160;j++)
			{
				if (minFaces < mahanttanFaces2 [j]){
					minFaces = mahanttanFaces2 [j];
					posFaces = j;
				}
			}
			for (int j=0;j<160;j++)
			{
				if (minNonFaces < mahanttanNonFaces2 [j]){
					minNonFaces = mahanttanNonFaces2 [j];
					posNonFaces = j;
				}
			}
			if (minFaces < minNonFaces ) {
				mahanttanFaces2[posFaces] = -102000;
				nbFaces++;
			}
			else{
				mahanttanNonFaces2[posNonFaces] = -102000;
				nbNonFaces++;
			}
		}

		CString msg;
		if (nbFaces > nbNonFaces){
			msg.Format("The selected image is a face"); 
		}
		else
			msg.Format("The selected image is not a face"); 
		AfxMessageBox(msg); 

		END_PROCESSING("Knn euclidean");
	}


	void CDibView::OnLab7Bivariatedistribution()
	{
		// TODO: Add your command handler code here
	}
	double dataset[3000][3];
	int counter=0;

	void CDibView::OnLab9Perceptron()
	{
		BEGIN_PROCESSING();		


		double a[3] = {0.1,0.1,0.1};
		double niu = 1;
		double stopThreshold = 0.0001;
		double b[3] = {0,0,0};

		for (int i=0;i<dwHeight;i++)
		{
			for (int j=0;j<dwWidth;j++)
			{
				if (lpSrc[i*w+j] == 2) {
					dataset[counter][0] = 1;
					dataset[counter][1] = i;
					dataset[counter][2] = j;
					counter++;
				}
				else if (lpSrc[i*w+j] == 1) {
					dataset[counter][0] = -1;
					dataset[counter][1] = -i;
					dataset[counter][2] = -j;
					counter++;
				}
			}
		}
		double value = 0;
		int iteratii = 0;
		do
		{
			b[0] = 0;b[1] = 0;b[2] = 0;
			for (int i=0;i<counter;i++)
			{
				if (dataset[i][0] == 1)
				{
					double a1 = a[0] + dataset[i][1] * a[1] + a[2] * dataset[i][2];
					if (a1 < 0) {
						a[0] += niu * dataset[i][0];
						a[1] += niu * dataset[i][1];
						a[2] += niu * dataset[i][2];

						b[0] += niu * dataset[i][0];
						b[1] += niu * dataset[i][1];
						b[2] += niu * dataset[i][2];
					}

				}
				else 
				{
					double a1 = -1 * a[0] + dataset[i][1] * a[1] + a[2] * dataset[i][2];
					if (a1 < 0) {
						a[0] += niu * dataset[i][0];
						a[1] += niu * dataset[i][1];
						a[2] += niu * dataset[i][2];

						b[0] += niu * dataset[i][0];
						b[1] += niu * dataset[i][1];
						b[2] += niu * dataset[i][2];
					}
				}
			}
			iteratii++;
			value = sqrt(b[0]*b[0] + b[1]*b[1]+b[2]*b[2]);
		}while (value >= stopThreshold);

		bmiColorsDst[3].rgbRed = 0;
		bmiColorsDst[3].rgbGreen = 0;
		bmiColorsDst[3].rgbBlue = 64;

		bmiColorsDst[3].rgbRed = 64;
		bmiColorsDst[3].rgbGreen = 0;
		bmiColorsDst[3].rgbBlue = 0;


		for (int i=0;i<dwHeight;i++)
		{
			for (int j=0;j<dwWidth;j++)
			{
				if (((int)(a[0] + a[1] * i + a[2] * j)) < 0 && lpDst[i*w+j] != 2 && lpDst[i*w+j] != 1)
				{
					lpDst[i*w+j] = 3;
				}
				else if (((int)(a[0] + a[1] * i + a[2] * j)) > 0 && lpDst[i*w+j] != 1 && lpDst[i*w+j] != 2)
				{
					lpDst[i*w+j] = 4;
				}

			}
		}

		END_PROCESSING("Perceptron");
	}

	struct PointDouble
	{
		double X;
		double Y;
	};

	PointDouble RedPoints[2000],BluePoints[2000];
	double S1[2][2],S2[2][2],Sb[2][2],Sw[2][2],Swt[2][2];

	void CDibView::OnLab10Lda()
	{
		BEGIN_PROCESSING();		


		int indexRed = 0;
		int indexBlue = 0;
		for (int i=0;i<dwHeight;i++)
		{
			for (int j=0;j<dwWidth;j++)
			{
				if (lpSrc[i*w+j] == 1)
				{
					RedPoints[indexRed].X = i;
					RedPoints[indexRed].Y = j;
					indexRed++;
				}
				if (lpSrc[i*w+j] == 2)
				{
					BluePoints[indexBlue].X = i;
					BluePoints[indexBlue].Y = j;
					indexBlue++;
				}
			}
		}

		PointDouble meanRed,meanBlue;
		meanRed.X = 0;
		meanRed.Y = 0;
		meanBlue.X = 0;
		meanBlue.Y = 0;

		for (int i = 0 ; i < indexRed; i++)
		{
			meanRed.Y += RedPoints[i].X;
			meanRed.X += RedPoints[i].Y;
		}

		for (int i = 0 ; i < indexBlue; i++)
		{
			meanBlue.X += BluePoints[i].X;
			meanBlue.Y += BluePoints[i].Y;
		}

		meanRed.X = meanRed.X / indexRed;
		meanRed.Y = meanRed.Y / indexRed;
		meanBlue.X = meanBlue.X / indexBlue;
		meanBlue.Y = meanBlue.Y / indexBlue;

		for (int i = 0; i < indexRed; i++)
		{
			S1[1][1] += (RedPoints[i].X - meanRed.Y) * (RedPoints[i].X - meanRed.Y);
			S1[1][0] += (RedPoints[i].X - meanRed.Y) * (RedPoints[i].Y - meanRed.X);
			S1[0][1] += (RedPoints[i].Y - meanRed.X) * (RedPoints[i].X - meanRed.Y);
			S1[0][0] += (RedPoints[i].Y - meanRed.X) * (RedPoints[i].Y - meanRed.X);
		}

		for (int i = 0; i < indexBlue; i++)
		{
			S2[1][1] += (BluePoints[i].X - meanBlue.X) * (BluePoints[i].X - meanBlue.X);
			S2[1][0] += (BluePoints[i].X - meanBlue.X) * (BluePoints[i].Y - meanBlue.Y);
			S2[0][1] += (BluePoints[i].Y - meanBlue.Y) * (BluePoints[i].X - meanBlue.X);
			S2[0][0] += (BluePoints[i].Y - meanBlue.Y) * (BluePoints[i].Y - meanBlue.Y);
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				S1[i][j] /= indexRed;
				S2[i][j] /= indexBlue;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Sw[i][j] = S1[i][j] + S2[i][j];
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Swt[i][j] = Sw[1-j][1-i];
			}
		}

		Sb[1][1] = (meanRed.Y - meanBlue.X) * (meanRed.Y - meanBlue.X);
		Sb[1][0] = (meanRed.Y - meanBlue.X) * (meanRed.X - meanBlue.Y);
		Sb[0][1] = (meanRed.X - meanBlue.Y) * (meanRed.Y - meanBlue.X);
		Sb[0][0] = (meanRed.X - meanBlue.Y) * (meanRed.X - meanBlue.Y);

		double det =  Sw[0][0] * Sw[1][1] - Sw[0][1] * Sw[1][0];
		double  adj[2][2] = {{Sw[1][1], -Sw[0][1]},{-Sw[1][0], Sw[0][0]}};

		double invSw[2][2];

		for(int i=0; i<2; i++)
			for(int j=0; j<2; j++)
				invSw[i][j] = (1/det) * adj[i][j];


		double wStar[2];
		wStar[0] = invSw[0][0] * (meanRed.X - meanBlue.Y) + invSw[0][1] * (meanRed.Y - meanBlue.X);
		wStar[1] = invSw[1][0] * (meanRed.X - meanBlue.Y) + invSw[1][1] * (meanRed.Y - meanBlue.X);




		CDC dc;
		dc.CreateCompatibleDC(0);
		CBitmap ddBitmap;
		HBITMAP hDDBitmap = CreateDIBitmap(::GetDC(0),
			&((LPBITMAPINFO)lpS)->bmiHeader, CBM_INIT, lpSrc,
			(LPBITMAPINFO) lpS, DIB_RGB_COLORS);

		ddBitmap.Attach(hDDBitmap);
		CBitmap* pTempBmp = dc.SelectObject(&ddBitmap);

		CPen pen(PS_SOLID, 1, RGB(255,0,0));
		CPen *pTempPen = dc.SelectObject(&pen);

		double x1 = 0;
		double x2 = (wStar[1] / wStar[0]) * (x1 - dwWidth/2) + dwHeight/2;

		dc.MoveTo(0,dwHeight - 1 - x2);

		x1 = dwWidth-1;
		x2 = (wStar[1] / wStar[0]) * (x1 - dwWidth/2) + dwHeight/2;
		dc.LineTo(x1,dwHeight - 1 - x2);

		dc.SelectObject(pTempPen);
		dc.SelectObject(pTempBmp);

		GetDIBits(dc.m_hDC, ddBitmap, 0, dwHeight, lpDst,
			(LPBITMAPINFO)lpD, DIB_RGB_COLORS);

		END_PROCESSING("LDA");
	}

	struct WeightedPoint{
		int x;
		int y;
		double w;
	};

	WeightedPoint D[1300];



	void CDibView::OnLab11Adaboost()
	{
		BEGIN_PROCESSING();

		int T = 5;
		double m = 0;
		int nbOfPoints = 0;

		for (int i=0;i<dwWidth;i++)
			for (int j=0;j<dwHeight;j++)
			{
				if (lpSrc[i*w+j] == 0 || lpSrc[i*w+j] == 2)
				{
					m++;
				}
			}

			for (int i=0;i<dwWidth;i++)
				for (int j=0;j<dwHeight;j++)
				{
					if (lpSrc[i*w+j] == 0 || lpSrc[i*w+j] == 2)
					{
						D[nbOfPoints].x = i;
						D[nbOfPoints].y = j;
						D[nbOfPoints].w = 1/m;
						nbOfPoints++;
					}

				}

				double sum = 0;double error1=0,error2=0,error3=0,error4 = 0;
				for (int t = 0; t < T; t++)
				{
					for (int i=0 ;i<nbOfPoints;i++)
					{
						sum += D[i].w;
					}
					for (int i = 0; i< nbOfPoints ;i++)
					{
						D[i].w = D[i].w / sum;
					}



					for (int i = 0 ; i < nbOfPoints; i++)
					{
						int th = D[i].x;
						for (int j = 0; j<nbOfPoints;j++)
						{
							if (D[j].x < th)
								error1+=D[j].w;
						}

					}
					for (int i = 0 ; i < nbOfPoints; i++)
					{
						int th = D[i].x;
						for (int j = 0; j<nbOfPoints;j++)
						{
							if (D[j].x > th)
								error2 += D[j].w;
						}

					}

					for (int i = 0 ; i < nbOfPoints; i++)
					{
						int th = D[i].y;
						for (int j = 0; j<nbOfPoints;j++)
						{
							if (D[j].y < th)
								error3 += D[j].w;
						}

					}

					for (int i = 0 ; i < nbOfPoints; i++)
					{
						int th = D[i].y;
						for (int j = 0; j<nbOfPoints;j++)
						{
							if (D[j].x < th)
								error4 += D[j].w;
						}

					}
				}

				double minError = error1 < error2 ? error1 : error2;
				int bestWeak = error1 < error2 ? 1 : 2;
				minError = minError < error3 ? minError : error3;
				bestWeak = minError < error3 ? bestWeak : 3;
				minError = minError < error4 ? minError : error4;
				bestWeak = minError < error4 ? bestWeak : 4;



				END_PROCESSING("ADA");
	}

	struct ArcT{
		int x1;
		int y1;
		int x2;
		int y2;
		double w;
		bool visited;
	};

#define RADIUS 10
	WeightedPoint unclassifiedPoints[1000],vNew[900];
	ArcT arcs[900],selectedArcs[900];
	int unclassfiedPointsCounter, arcsCounter, selectedArcsCounter,vNewCounter;

	double calculateDistance(WeightedPoint firstPoint, WeightedPoint secondPoint)
	{
		double aux1 = (firstPoint.x - secondPoint.x) * (firstPoint.x - secondPoint.x);
		double aux2 = (firstPoint.y - secondPoint.y) * (firstPoint.y - secondPoint.y);
		double distance = sqrt (aux1 + aux2);
		return distance;
	}

	void constructCompleteGraph()
	{
		for (int i = 0; i < unclassfiedPointsCounter - 1; i++)
		{
			for (int j = i + 1; j < unclassfiedPointsCounter; j++)
			{
				if (unclassifiedPoints[i].x != unclassifiedPoints[j].x
					&& unclassifiedPoints[i].y != unclassifiedPoints[j].y)
				{
					arcs[arcsCounter].x1 = unclassifiedPoints[i].x;
					arcs[arcsCounter].y1 = unclassifiedPoints[i].y;
					arcs[arcsCounter].x2 = unclassifiedPoints[j].x;
					arcs[arcsCounter].y2 = unclassifiedPoints[j].y;
					arcs[arcsCounter].w = calculateDistance(unclassifiedPoints[i],unclassifiedPoints[j]);
					arcs[arcsCounter].visited = false;
					arcsCounter++;
				}
			}
		}
	}

	void sortArcsByWeight()
	{
		for (int i = 0; i < arcsCounter; i++)
		{
			for (int j = i+1; j < arcsCounter; j++)
			{
				if (arcs[i].w > arcs[j].w)
				{
					ArcT aux = arcs[i];
					arcs[i] = arcs[j];
					arcs[j] = aux;
				}
			}
		}
	}

	bool existUnvisitedEdges()
	{
		for (int i = 0; i < arcsCounter; i++)
		{
			if (arcs[i].visited == false)
				return true;
		}
		return false;
	}

	bool isFirstEndContained(ArcT arc)
	{
		for (int i = 0; i < arcsCounter; i++)
		{
			if (vNew[i].x == arc.x1 && vNew[i].y == arc.y1)
				return true;
		}
		return false;
	}

	bool isSecondEndNotContained(ArcT arc)
	{
		for (int i = 0; i < arcsCounter; i++)
		{
			if (vNew[i].x == arc.x2 && vNew[i].y == arc.y2)
				return false;
		}
		return true;
	}

	void prim()
	{
		vNew[vNewCounter] = unclassifiedPoints[0];
		while (vNewCounter < unclassfiedPointsCounter)
		{
			ArcT minArc;
			minArc.w = INT_MAX;
			for (int i = 0; i < arcsCounter; i++)
			{
				if (minArc.w > arcs[i].w && isFirstEndContained(arcs[i]) && isSecondEndNotContained(arcs[i])) 
				{
					minArc.x1 = arcs[i].x1;
					minArc.y1 = arcs[i].y1;
					minArc.x2 = arcs[i].x2;
					minArc.y2 = arcs[i].y2;
					minArc.w = arcs[i].w;
				}
			}
			vNewCounter++;
			vNew[vNewCounter].x = minArc.x2;
			vNew[vNewCounter].y = minArc.y2;
			vNew[vNewCounter].w = minArc.w;

			selectedArcs[selectedArcsCounter] = minArc;
			selectedArcsCounter++;
		}
	}

	void trunkBigEdges()
	{
		for (int i = 0; i < selectedArcsCounter; i++)
		{
			if (selectedArcs[i].w > RADIUS)
				selectedArcs[i].w = -1;
		}
	}

	void CDibView::OnPrsProiect()
	{
		BEGIN_PROCESSING();		

		for (int i = 0; i < dwHeight ; i++)
		{
			for (int j=0; j < dwWidth; j++)
			{
				if (lpSrc[i*w+j] != 255)
				{
					unclassifiedPoints[unclassfiedPointsCounter].x = i;
					unclassifiedPoints[unclassfiedPointsCounter].y = j;
					unclassifiedPoints[unclassfiedPointsCounter].w = -1;
					unclassfiedPointsCounter++;
				}
			}
		}

		constructCompleteGraph();
		prim();
		trunkBigEdges();
		selectedArcs;

		END_PROCESSING("Project");
	}
