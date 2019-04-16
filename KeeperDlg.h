
// KeeperDlg.h : ͷ�ļ�
//

#pragma once
#include <string>
#include "BcecrSocket.h"
#include "AppInfo.h"
#include "CPUusage.h"
#include "tick.h"
#include "ThreadPool\ServerFinder.h"
#include "Resource.h"
using namespace std;

class CKeeperDlg;
extern CKeeperDlg *g_KeeperDlg;

// Ctrl+���� �������öԻ���
#define MY_HOTKEY 1

// ������c����ʱ�ȴ�s��
#define WAIT(c, s) for(int n = max(100*(s), 2); --n && (c); Sleep(10))

/// ������Ϣ
#define WM_TRAY_MSG (WM_USER+100)

#define MAX_NAMES 5

// һ�ν���������
#define SOCKET_BUF 256

// Win7�汾��
#define WINDOWS_7_VER 61

// stop application
#define StopApp ReStart

enum { S_RUN = 0, S_STOP, S_PAUSE };

// 64λ������ʹ��64λ�ػ����������ƥ�䷵��true��
inline bool CheckWowProcess(HANDLE process)
{
	BOOL b32 = FALSE, b = IsWow64Process(process, &b32);
	return b32 ? false : sizeof(INT_PTR) != 8;
}

// Notice�̼߳�����
struct NoticeNum
{
	int num;
	CRITICAL_SECTION cs;
	NoticeNum() : num(0) { ::InitializeCriticalSection(&cs); }
	~NoticeNum() { while(num) Sleep(10); ::DeleteCriticalSection(&cs); }
	void AddNoticeNum(int n) { EnterCriticalSection(&cs); num += n; LeaveCriticalSection(&cs); }
};

// CKeeperDlg �Ի���
class CKeeperDlg : public CDialog
{
// ����
public:
	CKeeperDlg(const string& Icon = "", CWnd* pParent = NULL);	// ��׼���캯��

	~CKeeperDlg();

	// ��ȡAPP����
	const char* GetAppName() const { return m_moduleName; }
	// ��ȡAPPΨһID
	const char* GetAppId() const { return m_moduleId; }
	// ��ȡAPP������
	const char* GetAppPwd() const { return m_password; }
	// ��ȡԶ��������������IP
	const char* GetRemoteIp() const { return m_strUpServer[0] ? m_strUpServer : m_pSocket->m_chToIp; }
	// ��ȡKeeper·��
	const char* GetModulePath() const { return m_pKeeperPath; }

	friend void UpdateThread(void *param);

	friend void RecoverThread(void *param);

private:
	enum { IDD = IDD_Keeper_DIALOG };// �Ի�������

	//////////////////////////////////////////////////////////////////////////
	char m_moduleName[32];			// ģ������Сд���������á�
	char m_moduleId[32];			// ģ����롾�������á�
	char m_password[64];			// ����
	char m_modulePath[MAX_PATH];	// ģ��·��Сд
	char m_pKeeperPath[MAX_PATH];	// ������·��
	int m_bIsStoped;				// ���ػ�����״̬:0-����,1-ֹͣ,2-��ͣ
	bool m_bUpdate;					// �Ƿ���������
	char m_strUpServer[64];			// ����������
	string m_strConf;				// �����ļ�·��
	char m_FileDescription[MAX_PATH];// ���ػ���������

	//////////////////////////////////////////////////////////////////////////
	// settings
	int m_nWatchTime;				// �۲�ʱ��[2]
	bool m_bAutoRun;				// ��������[3]
	int m_nVisible;					// ���ػ������Ƿ�ɼ�[4]
	int m_nAffinityCpu;				// �󶨵�CPU[5]
	char m_sTitle[MAX_PATH];		// ��������(MFC����������)[6]
	CString m_strIcon;				// ����ͼ��[7]
	char m_Ip[64];					// Զ��Ip[8]
	int m_nPort;					// Զ�˶˿�[9]
	int m_nExitCode;				// ���ػ����������˳�����[10]

	CBcecrSocket *m_pSocket;		// ͨѶsocket
	CString m_sWindowNames[MAX_NAMES];// ���ػ��ĳ��������ʾ�Ĵ�����
	char m_sLogDir[MAX_PATH];		// ��־Ŀ¼

	BOOL m_bExit;					// �������˳�
	BOOL m_bKeeeperExit;			// �ػ��߳��˳�
	BOOL m_bCheckExit;				// �����߳��˳�
	BOOL m_bSocketExit;				// Socket�߳��˳�
	DWORD m_ThreadId;				// ���ػ����̵�ID
	HANDLE m_handle;				// ���ػ����̵ľ��
	int m_nAliveTime;				// �������ڣ��룩
	int m_nRunTimes;				// ���ػ�������������
	char m_sRunLog[64];				// ���ػ�����������־
	HANDLE m_event;					// keeper�˳��¼�

	BOOL m_bTray;					// ���̱��
	HICON m_hIcon;					// ͼ����
	Gdiplus::Bitmap *pBmp;			// ͼ��
	CMenu m_trayPopupMenu;			// �����Ҽ��˵�
	BOOL SetTray(BOOL bTray);		// ��������Ч��

	CPUusage m_cpu;					// CPUʹ����
	tick_s	mytick;					// tick
	char m_strCreateTime[64];		// ��������
	char m_strModeTime[64];			// �޸�����
	char m_strVersion[64];			// �ļ��汾
	float m_fFileSize;				// �ļ���С(Mb)
	char m_strKeeperVer[64];		// �ػ�����汾
	CRITICAL_SECTION m_cs;

	void GetFileTimeInfo(const char *file); // ��ȡ�ļ���Ϣ

	void GetFileInfo(); // ��ȡ���ػ������ļ���Ϣ

	void GetDiskInfo(char *info); // ��ȡ�������ڴ��̵���Ϣ

	// �����ļ�
	bool DownloadFile(const char *name, const char *postfix = ".exe", BOOL isKeeper = true, const char *type = "exe");

	void DownloadFilelist(const char *dst, char *p);

	// ���ڴ��ͷ�������"updater.exe"
	BOOL ReleaseRes(const char *strFileName, WORD wResID, const CString &strFileType);

	CServerFinder *m_finder;

	const char *ControlIp() const { return (m_finder && m_finder->IsReady()) ? CMyTask::GetIp() : m_Ip; }

	void Lock() { EnterCriticalSection(&m_cs); }

	void Unlock() { LeaveCriticalSection(&m_cs); }

public:

	//////////////////////////////////////////////////////////////////////////
	// �ػ��߳�
	static DWORD WINAPI keepProc(LPVOID pParam);

	// ������
	static DWORD WINAPI checkProc(LPVOID pParam);

	// ִ��Զ������
	static DWORD WINAPI socketProc(LPVOID pParam);
	//////////////////////////////////////////////////////////////////////////

	// ��ȡ����IP
	std::string GetLocalHost() const;

	// ���ñ�����������
	BOOL SetSelfStart(const char *sPath, const char *sNmae, bool bEnable = true) const;

	// �ҵ����ػ�����ľ��
	HWND FindKeepWindow() const;

	// ��ȡ���ػ�����Ľ��̾��
	static HANDLE GetProcessHandle(const CString &processName, const CString &strFullPath, BOOL &op);

	// �Ƿ�����ʱ�ɼ�
	bool IsVisible() const { return 1 == m_nVisible; }

	// Ϊ���ػ��������ͼ��
	HICON GetIcon() const { return m_hIcon; }

	// ��ȡ�ػ�����·��
	CString GetKeeperPath() const { return CString(m_pKeeperPath); }

	// �˳��ػ�����
	void ExitKeeper(bool bExitDst = false);

	// �����ػ�����
	static void ReStartSelf(const CString &Path);

	// ����ffmepg������Ļ
	void ffmepgStart(int nPort);

	// ֹͣffmpeg������Ļ
	void ffmpegStop(const char *app);

	// ���ض˷�����Ϣ
	void SendInfo(const char *info, const char *details, int code = 0);

// ʵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	afx_msg LRESULT OnTrayCallbackMsg(WPARAM wp, LPARAM lp);// ���̴�����

	// ���ص�����
	BOOL HideToTray();

	// ֹͣ���򲢹ػ�
	void Shutdown();

	// ���������
	void ReBoot();

	// ��������
	bool ReStart(bool bForce = true);

	// ˢ�³���
	void Refresh();

	// ֹͣ����
	void Stop(bool bAll=false);

	// ��������
	void Start();

	// ��������
	void Update(const std::string &arg);

	// ����ʱ��
	void SetTime(const std::string &arg);

	// ��ѯ������Ϣ
	bool QueryAppInfo(AppInfo &info);

	// �����������ʱ�䲢��¼
	const char* log_runtime(bool bLog2File = false);

	// ��¼Զ�˼�ز�����־
	void log_command(const time_t *date, const char *ip, const char *cmd, const char *arg) const;

	void log_command(const time_t *date, const char *content) const;

	void InitRemoteIp(const char *ip, int port);

	// ��ʼ����������(������ݸ�����Ѱ�Ҵ��ھ��)
	void InitTitles(const char *t);

	// ���öԻ���ͼ��
	void SetDlgIcon(const CString &icon);

	// ��������
	void Notice(const char *notice, int tm=8000);

	// ���������̣߳��Թ���ض˼��ӳ���״̬
	void Watch(int bWatch);

	// �˳��ػ�
	afx_msg void OnExitMenu();

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonShowconsole();
	afx_msg void OnAppAbout();
	afx_msg void OnSelfStart();
	afx_msg void OnUpdateSelfStart(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
public:
	afx_msg void OnSettings();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};
