#include "stdafx.h"
#include "BcecrSocket.h"   
#include <stdio.h>
#include <iostream>

#ifndef USING_LOG
#define MY_LOG(pStr)	std::cout << pStr << std::endl
#else 
#include "../log.h"
#endif
#include <assert.h>


/**
* @brief Ĭ�ϵĹ��캯��
*/
CBcecrSocket::CBcecrSocket()
{
	m_bInit = false;
	m_bConneted = false;
	m_bRegistered = false;
	m_nType = 0;
	m_chToport = 0;

	m_Socket = INVALID_SOCKET;
	
	memset(m_chLocalIp, 0, sizeof(m_chLocalIp));
	memset(m_chToIp, 0, sizeof(m_chToIp));
	memset(&m_in, 0, sizeof(m_in));
}

/**
* @brief ~CBcecrSocket
*/
CBcecrSocket::~CBcecrSocket()
{
	unInit();
}

/** 
* @brief ��ʼ��һ�� socket
* @param[in] *pIp	�����IP
* @param[in] nPort	ͨ�Ŷ˿�
* @param[in] nType	socket���ͣ�0��server 1:client
* @retval	 int
* @return	 ������,��ͨ��GetErrorMsg()��ȡ������Ϣ  
*/
int CBcecrSocket::init(const char *pIp, int nPort, int nType)
{
	int nRet = _NO__ERROR;

	/// ����socket������������
	do 
	{
		m_nType = nType;
		
		assert(nType == 1);
		{
			if (!m_bInit)
			{
				m_Socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
				if (INVALID_SOCKET == m_Socket)
				{
					nRet = ERROR_SOCKET;
					break;
				}
				/// ���ͻ�����
				const int nSendBuf = 1024 * 1024 * 2;
				::setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
				/// ���ջ�����
				const int nRecvBuf = 1024 * 1024 * 2;
				::setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
				bool bConditionalAccept = true;
				::setsockopt(m_Socket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (const char *)&bConditionalAccept, sizeof(bool));

				m_chToport = nPort;
				memcpy(m_chToIp, pIp, strlen(pIp));

				m_in.sin_family = AF_INET;
				m_in.sin_port = htons(nPort);
				m_in.sin_addr.s_addr = inet_addr(pIp);
			}
			if (_NO__ERROR == nRet)
				m_bInit = true;
			clock_t t = clock();
			nRet = Connect();
			TRACE("======> Connect use time = %d\n", clock() - t);
		}

	} while (false);

	return nRet;

}


int CBcecrSocket::init(SOCKET s, const char *ip)
{
	if (INVALID_SOCKET == s)
		return -1;
	m_bInit = true;
	m_bConneted = true;
	m_bRegistered = true;
	m_Socket = s;
	strcpy_s(m_chToIp, ip);
	return _NO__ERROR;
}


int CBcecrSocket::Connect()
{
	int nRet = _NO__ERROR;
	do 
	{
		if (0 == m_nType || !m_bInit)
			break;
		/// �ͷ���˽�������
		nRet = ::connect(m_Socket, (const sockaddr *)&m_in, sizeof(sockaddr_in));
		if (SOCKET_ERROR == nRet)
		{
			nRet = ERROR_CONNECT;
			break;
		}

		//socketת��Ϊ������ģʽ,����ĳ����Ƶ�����������ж�ʱ,��ֹ��������
		ULONG ul = 1;   
		if (SOCKET_ERROR == ioctlsocket(m_Socket, FIONBIO, &ul))
		{
			nRet = ERROR_IOCTLSOCKET;
			break;
		}
		m_bConneted = true;
	} while (false);
	return nRet;
}


void CBcecrSocket::unInit()
{
	m_bInit = false;
	m_bConneted = false;
	m_bRegistered = false;
	if (INVALID_SOCKET != m_Socket)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

/**
* @brief ��ȡ����
* @param[in] *pBuf			������
* @param[in] nReadLen		���ݳ���
* @param[in] nTimeOut		��ʱʱ��
* @return ���ز�������(���ڵ���0Ϊ�ɹ�)
* @retval int
*/
int CBcecrSocket::recvData(char *pBuf, int nReadLen, int nTimeOut)
{
	if ((INVALID_SOCKET == m_Socket) || (NULL == pBuf) || (0 == nReadLen))
	{
		return -1;
	}

	const struct timeval time = { nTimeOut/1000, (nTimeOut%1000) * 1000 };

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(m_Socket, &fd);

	int ret = ::select(m_Socket+1, &fd, NULL, NULL, &time);
	if ( ret )
	{
		if ( FD_ISSET(m_Socket, &fd) )
		{
			ret = ::recv(m_Socket, pBuf, nReadLen, 0);
			ret = (ret <= 0) ? -1 : ret;
		}
	}
	else if(ret < 0)
	{
		ret = -1;
	}

	return ret;
}

/**
* @brief ��ȡ����
* @param[in] *pData			������
* @param[in] nSendLen		���ݳ���
* @return ���ز�������
* @retval int
*/
int CBcecrSocket::sendData(const char *pData, int nSendLen)
{
	if ((INVALID_SOCKET == m_Socket) || (NULL == pData) || (0 == nSendLen))
	{
		return -1;
	}

	int nRet = 0;
	int ret = 0;

	const struct timeval time = { 2, 0 };

	fd_set fdSend;
	int nLen = nSendLen;
	const char *pTmp = pData;

	while (nLen > 0)
	{
		FD_ZERO(&fdSend);
		FD_SET(m_Socket, &fdSend);

		ret = ::select(m_Socket+1, NULL, &fdSend, NULL, &time);
		if ( 1== ret )
		{
			if ( FD_ISSET(m_Socket, &fdSend) )
			{
				ret = ::send(m_Socket, pTmp, nLen, 0);
				if (ret <= 0)
				{
					nRet = -1;
					break;
				}

				nLen -= ret;
				pTmp += ret;

			}
		}
		else if ( ret < 0)
		{
			nRet = ret;
			break;
		}
		else if (0 == ret)
		{
			nRet = 0;
			break;
		}
	}

	return nRet;
}

/** ���socket������Ϣ
* @brief ���ݴ����룬��ȡsocket������Ϣ
* @param[in] nRet ������
* @return ���ش�����Ϣ�ַ���
* @retval string
*/
std::string CBcecrSocket::GetErrorMsg(const int nRet) const
{
	char Msg[100];
	memset(Msg, 0, 100);
	switch (nRet)
	{
	case _NO__ERROR:
		sprintf(Msg, "Success!\n");
		break;
	case ERROR_WSASTARTUP:
		sprintf(Msg, "WSAStartup() error:%d.\n", WSAGetLastError());
		break;
	case ERROR_SOCKET:
		sprintf(Msg, "socket() error:%d.\n", WSAGetLastError());
		break;
	case ERROR_CONNECT:
		sprintf(Msg, "connect() error:%d.\n", WSAGetLastError());
		break;
	case ERROR_BIND:
		sprintf(Msg, "bind() error:%d.\n", WSAGetLastError());
		break;
	case ERROR_LISTEN:
		sprintf(Msg, "listen() error:%d.\n", WSAGetLastError());
		break;
	case ERROR_IOCTLSOCKET:
		sprintf(Msg, "ioctlsocket() error:%d.\n", WSAGetLastError());
		break;
	default:
		sprintf(Msg, "Unknow error!\n");
		break;
	}
	return Msg;
}
