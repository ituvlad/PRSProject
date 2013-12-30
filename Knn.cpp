// Knn.cpp : implementation file
//

#include "stdafx.h"
#include "diblook.h"
#include "Knn.h"
#include "afxdialogex.h"


// CKnn dialog

IMPLEMENT_DYNAMIC(CKnn, CDialog)

CKnn::CKnn(CWnd* pParent /*=NULL*/)
	: CDialog(CKnn::IDD, pParent)
	, m_K(0)
{

}

CKnn::~CKnn()
{
}

void CKnn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_K);
}


BEGIN_MESSAGE_MAP(CKnn, CDialog)
END_MESSAGE_MAP()


// CKnn message handlers
