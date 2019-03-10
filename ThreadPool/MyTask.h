/** 
* @file MyTask.h
* @brief ����ʵ��
*/

#pragma once
#include "task.h"
#include "SimpleSocket.h"

/**
* @class CMyTask 
* @brief ��ĿThreadPool������ʵ��
*/
class CMyTask : public CTask
{
protected:
	static SOCKET m_Socket; // find a socket
	static char m_strIp[64];// find a ip

public:
	char m_curIp[64];		// IP

	int m_nPort;			// Port

	CSimpleSocket *m_pClient;// Socket

	CMyTask(const char *ip, int port, CSimpleSocket *pClient, int id);

	static SOCKET GetSocket() { return m_Socket; }

	static const char* GetIp() { return m_strIp; }

	static void SetSocket(SOCKET s) { m_Socket = s; }

	static bool isFind() { return INVALID_SOCKET != m_Socket; }

	static void Reset() { m_Socket = INVALID_SOCKET; }

protected:
	~CMyTask(void);
public:
	_inline virtual void Destroy();
	virtual void taskProc();
};
