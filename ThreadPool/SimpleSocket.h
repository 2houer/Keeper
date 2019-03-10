#pragma once

#include <WinSock2.h>


class CSimpleSocket
{
public:

	inline CSimpleSocket() : m_Socket(INVALID_SOCKET), m_argp(1) { }

	inline ~CSimpleSocket() { Close(); }

	// ���ӷ���ˣ�������ģʽ��
	int connectServer(const char *pIp, int nPort);

	inline SOCKET getSocket() { SOCKET s = m_Socket; m_Socket = INVALID_SOCKET; return s; }

	inline SOCKET GetSocket() const { return m_Socket; }

	void Close(); // �ر�

	int sendData(const char *pData, int nSendLen);

	int recvData(char *pBuf, int nReadLen);

private:
	SOCKET m_Socket;/**< ��Ϊ�ͻ������ӵ�socket */
	unsigned long m_argp;/**< �Ƿ�����ģʽ 1-���� 0-������*/
};
