#pragma once
#include "afxwin.h"


// Afx_MessageBox �Ի���
// �öԻ�����Զ��ر�
class Afx_MessageBox : public CDialog
{
	DECLARE_DYNAMIC(Afx_MessageBox)

public:
	Afx_MessageBox(LPCTSTR lpszPrompt, int nElapse = 5000, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Afx_MessageBox();

	// �Ի�������
	enum { IDD = IDD_AFXMESSAGE_BOX };

	int m_nElapse;

	int m_nTopMost;

	bool m_bIsNotice; // �Ƿ�Ϊ��ʾ��Ϣ

	CString m_strPrompt;

	CString m_strTitle; // ���ڱ��⣨��ѡ��

	void SetTopMost() { m_nTopMost = 1; }

	void SetNotice() { m_bIsNotice = true; }

	void SetTitle(const char *title) { m_strTitle = CString(title); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Infomation;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
