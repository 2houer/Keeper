
/** Copyright notice \n
* Copyright (c) 2016, BCECR
* All rights reserved.
* @file		BcecrSocket.h
* @brief BCECR SOCKET
* @version	1.0
* @date unknow
* @update	2017/3/10
* @author	BCECR
*/

#pragma once
#include <WinSock2.h>
#include <string>

/// �������ͻ�����
#define MAX_LISTEN 5
/// Ĭ�ϻ�������С
#define DEFAULT_BUFFER 256
/************************************************************************/
/* socket �����붨��                                                    */
/************************************************************************/
/// no error
#define _NO__ERROR 0
/// WSAStartup() error
#define ERROR_WSASTARTUP 1
/// socket() error
#define ERROR_SOCKET 2
/// connect() error
#define ERROR_CONNECT 3
/// bind() error
#define ERROR_BIND 4
/// listen() error
#define ERROR_LISTEN 5
/// ioctlsocket() error
#define ERROR_IOCTLSOCKET 6

/** 
* @class	CBcecrSocket 
* @brief	socketͨ���࣬����socketͨ�ŵķ�����/�ͻ��˳���
* @details	ʵ�ֻ�������/�����ݵĹ���
*/
class CBcecrSocket
{
public:
	/// ���캯��
	CBcecrSocket();
	/// ����
	~CBcecrSocket();
	/// ��ʼ��socket
	int init(const char *pIp, int nPort, int nType); //0:server, 1:client
	int init(SOCKET s, const char *ip); // ��s�����ͻ���socket
	/// ֻ���client�ˣ����ӷ����
	int Connect();
	/// socket�˳�ʱ����������
	void unInit();

	/// ��������
	int recvData(char *pBuf, int nReadLen, int nTimeOut = 1000); //nTimeOut��λ����
	/// ��������
	int sendData(const char *pData, int nSendLen);
	/// ������Ϣ
	std::string GetErrorMsg(const int nRet) const;

	bool IsConnected() const { return m_bConneted; }

	bool IsRegistered() const { return m_bRegistered; } // �Ƿ�ע��

	void Register(bool result) { m_bRegistered = result; } // ע���Ƿ�ɹ�

	SOCKET getSocket() const { return m_Socket; }

private:
	bool m_bInit;			/**< �Ƿ��Ѿ���ʼ�� */
	bool m_bConneted;		/**< �Ƿ������ӳɹ� */
	bool m_bRegistered;		/**< �Ƿ�ע��ɹ� */
	int m_nType;			/**< socket���ͣ�0:server, 1:client */

	SOCKET m_Socket;		/**< ��Ϊ�ͻ������ӵ�socket */
	sockaddr_in m_in;

public:
	char m_chToIp[32];				/**< �Է���IP */
	int  m_chToport;				/**< �Է��Ķ˿� */
	char m_chLocalIp[32];			/**< ����IP */
};
