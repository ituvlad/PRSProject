#pragma once


// CKnn dialog

class CKnn : public CDialog
{
	DECLARE_DYNAMIC(CKnn)

public:
	CKnn(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKnn();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_K;
};
