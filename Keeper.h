
// Keeper.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include <string>
using namespace std;

// �Զ��رյĵ���������
#define MY_AfxMsgBox 6666

// CKeeperApp:
// �йش����ʵ�֣������ Keeper.cpp
//

class CKeeperApp : public CWinApp
{
public:
	bool m_bUnique;	// ���ػ������Ƿ�������

	int m_nParentId;// �ػ�����ĸ�����ID

	bool m_bReStart; // �Ƿ��˳�������

	bool is_debug;	 // ����ģʽ(������򽵼�)

	char m_strTitle[64]; // Keeper����(Сд)

	string m_strConf;	// �����ļ�·��

	// �����˳�����
	void SetReStart() { m_bReStart = true; }

	CKeeperApp();

	// �����á�Ԥ����
	BOOL Prepare(string &icon);

	// �ҵ���ִ���ļ�
	string FindExecutable(const char *sDirect);

// ��д
public:
	HANDLE m_hMutex;

	virtual BOOL InitInstance();

	virtual int ExitInstance();

	ULONG_PTR m_gdiplusToken;

	// ʵ��
	DECLARE_MESSAGE_MAP()
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
};

extern CKeeperApp theApp;
