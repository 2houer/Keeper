#pragma once
#include "BcecrSocket.h"

// Ŀ¼
#define IS_DIR FILE_ATTRIBUTE_DIRECTORY

// Ŀ¼������[��Ա�ױ��]
typedef struct folder
{
	char buf[_MAX_PATH], *pos;
	folder()
	{
		// ��ȡ����ǰĿ¼
		GetModuleFileNameA(NULL, buf, sizeof(buf));
		pos = buf;
		while ('\0' != *pos) ++pos;
		while ('\\' != *pos) --pos;
		++pos;
	}
	// ��ȡ��ǰĿ¼�µ��ļ���Ŀ¼
	inline const char* get(const char *file)
	{
		strcpy(pos, file);
		return buf;
	}
	// ��ȡ��Ŀ¼�µ��ļ���Ŀ¼
	inline const char* get(const char *child, const char *file)
	{
		sprintf(pos, "%s\\%s", child, file);
		return buf;
	}
}folder;

/************************************************************************
* @class Downloader
* @brief ����SOCKET���ļ�������
* @details �����������£�2�����أ�
* 1�����������������
* 2������Ҫ���ص��ļ�����ʽ��size:file
* 3����ȡ�ļ�ͷ���ݣ���ʽ��size:bytes,md5
* 4����������ָ���ʽ��down:file
* 5����ȡ�ļ�
************************************************************************/
class Downloader
{
private:
	bool m_bConnect;				// �Ƿ����ӷ�����
	bool m_bCleanup;				// .old�Ƿ�������
	CBcecrSocket *m_pSocket;		// ͨѶ�׽���
	folder m_path;					// Ŀ¼������

	char m_strApp[64];				// ��������������

	bool check(const char *file, int size, const char *MD5); // �Ƿ�������

public:
	Downloader(void);
	~Downloader(void);
	void Connect(const char *ip, int port);
	void Disconnect();
	void SetUpdateApp(const char *name); // ������������

	/**
	* @brief �����ļ�
	* @param[in] name �ļ�����
	* @param[in] postfix �����ļ���׺
	* @param[in] isCommon �Ƿ����ļ����ػ���ȡ1 filelist��ȡ-1�����ػ���ȡ0��
	* @param[in] type �ļ�����
	*/
	bool DownloadFile(const char *name, const char *postfix = ".exe", BOOL isCommon = false, const char *type = "exe");
};
