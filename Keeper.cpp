
// Keeper.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Keeper.h"
#include "KeeperDlg.h"
#include "confReader.h"
#include "ChooseExecutable.h"
#include "Afx_MessageBox.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����WinExec��������
#define WIN_EXEC 1

// CKeeperApp

BEGIN_MESSAGE_MAP(CKeeperApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKeeperApp ����

CKeeperApp::CKeeperApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_bUnique = true;
	m_bReStart = false;
	m_nParentId = 0;
	m_hMutex = INVALID_HANDLE_VALUE;
}


// Ψһ��һ�� CKeeperApp ����

CKeeperApp theApp;


// CKeeperApp ��ʼ��

// Ѱ�ҿ�ִ���ļ�����Ϊ���ػ�����
string CKeeperApp::FindExecutable(const char *sDirect)
{
	CString filter = CString(sDirect) + _T("\\*.EXE");
	CFileFind ff;
	BOOL bFind = ff.FindFile(filter);
	vector<CString> result;
	USES_CONVERSION;
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (ff.IsDots() || ff.IsDirectory())
			continue;
		CString ret = ff.GetFileTitle();
		char *cur = W2A(ret);
		const char *lwr = _strlwr(cur);
		if (strcmp(lwr, m_strTitle))
		{
			result.push_back(ret);
		}
	}
	ff.Close();
	if (1 == result.size())
	{
		return W2A(result.at(0));
	}
	else if (result.size())
	{
		CChooseExecutable dlg(result);
		dlg.DoModal();
		return W2A(dlg.GetExecutable());
	}
	return "";
}

BOOL CKeeperApp::Prepare(string &temp)
{
	// ��ȡ��ǰ�û�����������ָ����ݣ�Ĭ��administrator����¼ϵͳ
	DWORD nLen = MAX_PATH;
	char strUser[MAX_PATH];
	if (!GetUserNameA(strUser, &nLen))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	USES_CONVERSION;
	WCHAR wPath[MAX_PATH]; 	// ��������·��
	GetModuleFileName(NULL, wPath, MAX_PATH);
	CString csPath(wPath);
	int pos = csPath.ReverseFind('\\');
	CString sDir = csPath.Left(pos);
	const char * strModuleDir = W2A(sDir);
	CString sName = csPath.Right(csPath.GetLength() - pos - 1);
	pos = sName.ReverseFind('.');
	CString sTitle = sName.Left(pos);
	strcpy_s(m_strTitle, W2A(sTitle));

	//////////////////////////////////////////////////////////////////////////
	const char *pTitle = _strlwr(m_strTitle);
	m_strConf = (0 == strcmp(m_strTitle, "keeper")) ? 
		string(strModuleDir) + "\\Keep.conf" : 
		string(strModuleDir) + string("\\") + string(m_strTitle) + string(".conf");
	const string &file = m_strConf;
	// �����ļ���ȡ
	confReader ini(file);

	// ��ȡģ��ID
	ini.setSection("module");
	temp = ini.readStr("id", "");
	if(temp.empty())
	{
		// IDΪ��ʱ��ȡ����
		temp = ini.readStr("name", "");
		temp = temp.empty() ? FindExecutable(strModuleDir) : temp;
		if (temp.empty())
		{
			AfxMessageBox(_T("���ػ����򲻴��ڻ�δ��ѡ��! �ػ������޷��������С�"));
			return FALSE;
		}
		WritePrivateProfileStringA("module", "name", temp.c_str(), file.c_str());
		WritePrivateProfileStringA("module", "id", temp.c_str(), file.c_str());
	}

	ini.setSection("settings");
	// [14] �Ƿ�ֻ����Ψһ��ʵ�����У�Ĭ��Ϊ1��
	m_bUnique = ini.readInt("unique", 1);

	// ��ģ���Ψһ���봴���ź���
	m_hMutex = m_bUnique ? ::CreateMutex(NULL, TRUE, CString(temp.c_str())) : INVALID_HANDLE_VALUE;
	if ( m_bUnique && (GetLastError() == ERROR_ALREADY_EXISTS) )
	{
		if (IDCANCEL == AfxMessageBox(_T("�ڸ��豸�Ѿ�������һ���ػ�! ID = ") + CString(temp.c_str()) 
			+ _T(", \r\n�Ƿ�����ػ�? �⽫�ظ����б��ػ�����, �������Ԥ��!\r\n������ȷ����2������һ����"), MY_AfxMsgBox))
			return FALSE;
		int t = time(NULL) % 86400;//1��ڼ���
		char t_str[64];
		sprintf_s(t_str, "_%d", t);
		temp = temp + string(t_str);
		WritePrivateProfileStringA("module", "id", temp.c_str(), file.c_str());
	}

	// [1]��ȡ����ϵͳ�û�
	const char *dst = _strlwr(strUser);
	temp = ini.readStr("sys_user", "");
	if (temp.empty())
	{
		WritePrivateProfileStringA("settings", "sys_user", dst, file.c_str());
		temp = dst;
	}
	else if (strcmp(temp.c_str(), dst) && m_bUnique)
	{
		Afx_MessageBox box(_T("����ϵͳ�û��������ļ���ƥ��! �Ƿ�������������?"));
		if (IDOK == box.DoModal())
		{
			WritePrivateProfileStringA("settings", "sys_user", dst, file.c_str());
			m_bReStart = true;
		}
		return FALSE;
	}

	// [7]��ȡͼ���ļ�
	Gdiplus::GdiplusStartupInput StartupInput;
	if (Gdiplus::GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL))
		return FALSE;
	temp = ini.readStr("icon", "");
	temp = temp.empty() ? string(strModuleDir) + "\\Keeper.png" : temp;
	// Icon����ڵ�ǰĿ¼
	const char *p = temp.c_str();
	if (p + 1 && ':' != p[1])
		temp = string(strModuleDir) + "\\" + temp;
	return TRUE;
}


BOOL CKeeperApp::InitInstance()
{
	float time_span = 6000.F;
	USES_CONVERSION;
	const char *id = W2A(m_lpCmdLine);
	m_nParentId = atoi(id);

	string temp;
	if (FALSE == Prepare(temp))
		return FALSE;

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// [11]��������״̬�������Զ����ɣ�
	const char is_run[4] = { "1" };
	int isRun = GetPrivateProfileIntA("settings", "is_run", 0, m_strConf.c_str());
	if (1 == isRun && m_bUnique)// ��һ���ػ�������ܷ������ر�
	{
		Afx_MessageBox box(_T("��⵽��Ӧ�ó������һ���˳��ǲ������ġ��Ƿ����ԭ��?"), time_span);
		box.DoModal();
	}else
		WritePrivateProfileStringA("settings", "is_run", is_run, m_strConf.c_str());

	AfxEnableControlContainer();

#ifndef _AFX_NO_MFC_CONTROLS_IN_DIALOGS
	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
#endif

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("TQEH"));

	CKeeperDlg dlg(temp);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

#ifndef _AFX_NO_MFC_CONTROLS_IN_DIALOGS
	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CKeeperApp::ExitInstance()
{
	// �ر��ź������
	if (INVALID_HANDLE_VALUE != m_hMutex)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		m_hMutex = INVALID_HANDLE_VALUE;
	}

	// GDI +
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	const char is_run[4] = { "0" };
	WritePrivateProfileStringA("settings", "is_run", is_run, m_strConf.c_str());

	// ��������
	if (m_bReStart)
	{
		char buf[_MAX_PATH];
		::GetModuleFileNameA(NULL, buf, sizeof(buf));
		CString path(buf);
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile = path;
		ShExecInfo.lpParameters = m_lpCmdLine;
		ShExecInfo.nShow = SW_SHOW;
#if WIN_EXEC
		if (WinExec(buf, SW_SHOW) <= 31)
#else
		if (FALSE == ShellExecuteEx(&ShExecInfo))
#endif
			OutputDebugStringA("======> ����\"Keeper\"ʧ��\n");
		else
			OutputDebugStringA("======> ����\"Keeper\"�ɹ�\n");
	}
	OutputDebugStringA("======> Keeper �˳� <======\n");

	return CWinApp::ExitInstance();
}

// AfxMessageBox���ض�����Ϊ
int CKeeperApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	if (MY_AfxMsgBox == nType)
	{
		Afx_MessageBox box(lpszPrompt, 8000);

		return box.DoModal();
	}

	return CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
}
