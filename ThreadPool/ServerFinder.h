#pragma once
#include "ThreadPool/MyThreadPool.h"
#include "ThreadPool/MyTask.h"
#include <time.h>
#include <process.h>

// ��ǰʱ���Ƿ�������
inline bool IS_NIGHT()
{
	int t = time(0) % 86400 + 28800 ;
	return t < 21600 || t > 64800; // t����[0-6) �� (18-24]
}

// FindServer �߳�״̬
enum FindState
{
	Is_Finding = 0,		// ����ִ��Find
	Not_Finding,		// ֹͣfind
	End_Finding,		// ����find
};

#define CLIENT_NUM 8

// Ѱ��ָ�����δ���ָ���˿ڵ��豸IP
class CServerFinder
{
private:
	bool m_bWait; // �Ƿ��ڵȴ�״̬
	FindState m_nState; // find״̬
	char m_strIp[64];
	int m_nPort;
	CSimpleSocket *m_pClient[CLIENT_NUM]; // Socket ��װ��Ķ�������
	CMyThreadPool *pool;
	static int m_nWaitTime;// �ȴ��������λ��s��Ĭ��10s��
	static void FindServer(void *param);

public:
	void Exit() { if (Is_Finding == m_nState) { m_nState = Not_Finding; while(End_Finding != m_nState) Sleep(10); } }

	void SetIpPort(const char *ip, int port) { m_bWait = false; strcpy_s(m_strIp, ip); m_nPort = port; }

	void SetThreadWait() { m_bWait = true; }

	bool IsReady() const { return !m_bWait; }

	static void SetWaitTime(int n) { m_nWaitTime = n; }

	CServerFinder(const char *ip, int port, int n);

	~CServerFinder();
};
