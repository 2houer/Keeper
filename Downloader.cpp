#include "Downloader.h"
#include "md5driver.hpp"
#include <io.h>
#include <direct.h>

// ����ָ��Ĺ̶�����
#define SIZE_1 100

// req					| resp
// (1) size:file		| size:bytes,md5
// (2) down:file		| ......

// ƴ������ָ��
#define make_cmd(buf, cmd, arg) {memset(buf,0,SIZE_1); sprintf_s(buf,"%s:%s",cmd,arg);}

// ����ָ��
#define parse_cmd(buf, arg) {arg=buf; while(*arg && ':'!=*arg)++arg; if(*arg)*arg++=0;}

// ��ָ���ļ�׼��д��
FILE* getFileId(const char *path)
{
	if (0 == _access(path, 0) && remove(path))
		return NULL;
	return fopen(path, "wb+");
}

// ����ļ��Ƿ��Ѿ����ع�
bool Downloader::check(const char *file, int size, const char *MD5)
{
	int n = 0;// �ļ���С
	FILE* f = fopen(file, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		n = ftell(f);
		fclose(f);
	}
	if (n != size)
		return false;
	// �Ƚ�Md5��
	char m[32+4] = {0};
	MDFile(file, m);
	return 0 == strcmp(m, MD5);
}

Downloader::Downloader(void) : m_bConnect(false), m_bCleanup(false), m_pSocket(new CBcecrSocket())
{
	memset(m_strApp, 0, sizeof(m_strApp));
}


Downloader::~Downloader(void)
{
	m_pSocket->unInit();
	delete m_pSocket;
	m_pSocket = NULL;
}


void Downloader::SetUpdateApp(const char *name)
{
	strcpy_s(m_strApp, name);
	m_bCleanup = false;
}


void Downloader::Connect(const char *ip, int port)
{
	if (false == m_bConnect && port)
	{
		m_bConnect = 0 == m_pSocket->init(ip, port, 1);
	}
}

void Downloader::Disconnect()
{
	m_bConnect = false;
	m_pSocket->unInit();
}

// ����ָ��Ŀ¼�µ������ļ�������"Keeper.exe"��
void ClearDir(const char *dir)
{
	std::string logDir(dir);
	//�ļ����
	intptr_t hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	std::string s;
	try
	{
		if ((hFile = _findfirst(s.assign(logDir).append("\\*.*").c_str(), &fileinfo)) != -1)
		{
			do{
				_strlwr(fileinfo.name);
				if (IS_DIR != fileinfo.attrib 
					&& strcmp(fileinfo.name, ".") && strcmp(fileinfo.name, "..")
					&& strcmp(fileinfo.name, "keeper.exe"))
				{
					std::string cur = s.assign(logDir).append("\\").append(fileinfo.name);
					DeleteFileA(cur.c_str());
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}catch (std::exception e){ if(hFile) _findclose(hFile); }
}

// ����Ŀ��Ŀ¼������м�Ŀ¼������Ҳ��������
void MakeDir(const char *path)
{
	char buf[_MAX_PATH], *p = buf;
	strcpy_s(buf, path);
	for (;;)
	{
		while(*p != '\\' && *p) ++p;
		if (*p == 0)
			break;
		*p = 0;
		if (-1 == _access(buf, 0))
			_mkdir(buf);
		*p++ = '\\';
	}
}

bool Downloader::DownloadFile(const char *name, const char *postfix, BOOL isCommon, const char *type)
{
	char file[_MAX_PATH]; // Զ���ļ�
	sprintf_s(file , "%s.%s", name, type);
	if (m_bConnect)
	{
		char temp[_MAX_PATH]; // �����ļ�
		sprintf_s(temp, "%s%s", name, postfix);
		char path[_MAX_PATH]; // �����ļ�����·��
		strcpy_s(path, m_path.get(temp));
		char buf[SIZE_1]; // Զ���ļ�����·��
		const bool b32bit = 4 == sizeof(INT_PTR);
		sprintf_s(buf, "%s\\%s", m_strApp, file);
		char CMD[SIZE_1]; // ����
		make_cmd(CMD, "size", buf);
		m_pSocket->sendData(CMD, SIZE_1);// ���ļ�����
		m_pSocket->recvData(CMD, SIZE_1);// ���ļ���С
		char *arg = 0;
		parse_cmd(CMD, arg);
		int size = atoi(arg);
		if (0 == size) // ������û�и��ļ�
		{
			make_cmd(CMD, "down", "?");
			m_pSocket->sendData(CMD, SIZE_1);// ������
			char szLog[256];
			sprintf_s(szLog, "===> �������������ļ�: %s\n", buf);
			OutputDebugStringA(szLog);
			return false;
		}
		const char *Md5 = arg; //md5��
		while (*Md5 && ',' != *Md5)++Md5;
		if (*Md5) ++Md5;
		bool bNotExist = (-1 == _access(path, 0));// �����ļ�������
		if (bNotExist || false == check(path, size, Md5))
		{
			const char *backup = m_path.get(".old"); // ����".old"Ŀ¼��žɰ汾�ļ�
			if (!m_bCleanup) { ClearDir(backup); m_bCleanup = true; }
			if(-1==isCommon && !bNotExist)
			{
				if (-1 == _access(backup, 0))
					_mkdir(backup);
				SetFileAttributesA(backup, FILE_ATTRIBUTE_HIDDEN);
				const char *cur = m_path.get(".old", temp);
				if (0 == _access(cur, 0)) // ɾ���ѱ��ݵľ��ļ�
					DeleteFileA(cur);
				else MakeDir(cur);
				if (FALSE == CopyFileA(path, cur, FALSE))// �滻ǰ�ȱ���
				{
					char info[300];
					sprintf_s(info, "===> �����ļ�ǰ����\"%s\"ʧ��.\n", path);
					OutputDebugStringA(info);
				}
			}else if(bNotExist) MakeDir(path);
			FILE *fid = getFileId(path);
			if (NULL == fid) // �ļ���ռ��
			{
				make_cmd(CMD, "down", "?");
				m_pSocket->sendData(CMD, SIZE_1);// ������
				char szLog[300];
				sprintf_s(szLog, "===> �����ļ���ռ��: %s\n", path);
				DeleteFileA(m_path.get(".old", temp));
				OutputDebugStringA(szLog);
				return false;
			}
			make_cmd(CMD, "down", buf);
			m_pSocket->sendData(CMD, SIZE_1); // ����
			int total = size;
			const int SIZE_2 = 128*1024;// 128K
			char *buf = new char[SIZE_2];// recv buf
			int s = 0; // ��ʱ����
			do // ���ļ�
			{
				int recv = m_pSocket->recvData(buf, SIZE_2-1);
				if (recv > 0)
				{
					s = 0;
					buf[recv] = 0;
					total -= recv;
					fwrite(buf, recv, 1, fid);
				}
				else if (recv < 0)
				{
					OutputDebugStringA("===> Socket�Ͽ��������ļ�ʧ��.\n");
					break;
				}
				else if (10 == ++s)
				{
					OutputDebugStringA("===> Socket��ʱ�������ļ�ʧ��.\n");
					break;
				}
			} while (total > 0);
			delete [] buf;
			fclose(fid);
			if (-1==isCommon && total)// ʧ�ܺ�ԭ�����ļ�
			{
				int k = 10;
				while (FALSE == DeleteFileA(path) && --k) Sleep(500);
				const char *cur = m_path.get(".old", temp);
				if (0 == _access(cur, 0) && FALSE == CopyFileA(cur, path, FALSE))
				{
					char info[300];
					sprintf_s(info, "===> ��ԭ�����ļ�\"%s\"ʧ��.\n", path);
					OutputDebugStringA(info);
				}
			}
			return 0 == total;
		}else{
			make_cmd(CMD, "down", "?");
			m_pSocket->sendData(CMD, SIZE_1);// ������
			char szLog[300];
			sprintf_s(szLog, "===> �����ļ�����Ҫ����: %s\n", path);
			OutputDebugStringA(szLog);
			return TRUE==isCommon ? true : false;
		}
	}
	return false;
}
