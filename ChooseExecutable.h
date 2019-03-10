#pragma once
#include "afxwin.h"
#include <vector>
using namespace std;

// CChooseExecutable �Ի���

/**
* @class CChooseExecutable
* @brief ѡ��Ի���ѡ���ػ�����
* 2018-2-22
*/
class CChooseExecutable : public CDialog
{
	DECLARE_DYNAMIC(CChooseExecutable)

public:
	CChooseExecutable(const vector<CString> &files, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CChooseExecutable();

	vector<CString> m_Files;

	CString GetExecutable() const;
	
// �Ի�������
	enum { IDD = IDD_EXECUTABLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ComboExecutable;
	CString m_StrExecutable;
	virtual BOOL OnInitDialog();
};
