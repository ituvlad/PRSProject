// dibview.h : interface of the CDibView class
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

class CDibView : public CScrollView
{
protected: // create from serialization only
	CDibView();
	DECLARE_DYNCREATE(CDibView)

// Attributes
public:
	CDibDoc* GetDocument()
		{
			ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDibDoc)));
			return (CDibDoc*) m_pDocument;
		}

// Operations
public:

// Implementation
public:
	virtual ~CDibView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);

	// Printing support
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

// Generated message map functions
protected:
	//{{AFX_MSG(CDibView)
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDoRealize(WPARAM wParam, LPARAM lParam);  // user message
	afx_msg void OnProcessingParcurgereSimpla();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLab1Ransac();
	afx_msg void OnLab1Ransac32776();
	afx_msg void OnLab2Ransac();
	afx_msg void OnLab3Hough();
	afx_msg void OnLab3Drawhough();
	afx_msg void OnLab4Dt();
	afx_msg void OnLab4Patternmatching();
	afx_msg void OnLab5Magnitudeandgradient();
	afx_msg void OnLab5Intervalcoloring();
	afx_msg void OnLab5Celldivision();
	afx_msg void OnLab5Hog();
	afx_msg void OnLab5Degreeofsimilarity();
	afx_msg void OnLab6Load400();
	afx_msg void OnLab6Corelationcoefficients();
	afx_msg void OnLab6Core();
	afx_msg void OnLab7Ass1();
	afx_msg void OnLab7Normaldistr();
	afx_msg void OnLab7Histogrampoints();
	afx_msg void OnLab7Bivariatedistribution();
	afx_msg void OnLab5Load2();
	afx_msg void OnLab7Loadtrainnonface();
	afx_msg void OnLab9Perceptron();
	afx_msg void OnLab10Lda();
	afx_msg void OnLab11Adaboost();
	afx_msg void OnPrsProiect();
	afx_msg void OnPrsProiect2();
};

/////////////////////////////////////////////////////////////////////////////
