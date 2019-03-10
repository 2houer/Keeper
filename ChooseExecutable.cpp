// ChooseExecutable.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Keeper.h"
#include "ChooseExecutable.h"
#include "afxdialogex.h"


// CChooseExecutable �Ի���

IMPLEMENT_DYNAMIC(CChooseExecutable, CDialog)

CChooseExecutable::CChooseExecutable(const vector<CString> &files, CWnd* pParent)
	: CDialog(CChooseExecutable::IDD, pParent)
	, m_StrExecutable(_T(""))
{
	m_Files.assign(files.begin(), files.end());
}

CChooseExecutable::~CChooseExecutable()
{
}

void CChooseExecutable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXECUTABLE_COMBO, m_ComboExecutable);
	DDX_CBString(pDX, IDC_EXECUTABLE_COMBO, m_StrExecutable);
}


BEGIN_MESSAGE_MAP(CChooseExecutable, CDialog)
END_MESSAGE_MAP()


// CChooseExecutable ��Ϣ�������

CString CChooseExecutable::GetExecutable() const
{
	return m_StrExecutable;
}


BOOL CChooseExecutable::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i = 0;
	for (vector<CString>::const_iterator pos = m_Files.begin(); pos != m_Files.end(); ++pos)
	{
		m_ComboExecutable.InsertString(i++, *pos);
	}
	m_ComboExecutable.SetCurSel(0);

	return TRUE;
}
