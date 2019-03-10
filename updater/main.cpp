#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <io.h>
#include <direct.h>

#define POSTFIX "_update.exe" //����������ĺ�׺����

// �˳�ʱ��ӡ��Ϣ
#ifdef _DEBUG
#define MY_LOG(pFile, pSrc, n) { OutputDebugStringA("======> "); OutputDebugStringA(pSrc); \
	if (pFile){ fwrite(pSrc, strlen(pSrc), 1, pFile); fclose(pFile); } return n; }
#else
#define MY_LOG(pFile, pSrc, n) { if (pFile){ fwrite(pSrc, strlen(pSrc), 1, pFile); fclose(pFile); } return n; }
#endif

#define LOG(pFile, pSrc) MY_LOG(pFile, pSrc, -1)
#define LOG_OK(pFile, pSrc) MY_LOG(pFile, pSrc, 0)
#define WAIT_TIME 10 //�ȴ�ʱ�䣨���ӣ�

// Ŀ¼������[��Ա�ױ��]
struct Folder
{
	char buf[_MAX_PATH], *pos;
	Folder()
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
};

// argv[1]Ϊ�������������(������׺)
// ��⵱ǰĿ¼�Ƿ����argv[1]_update.exe���ļ�
// ������ڣ���ɾ��argv[1].exe���������ļ��滻
int main(int argc, char ** argv)
{
	Folder folder; // ��ǰĿ¼����ʼ��
	FILE *f = fopen(folder.get("updater.log"), "a+");
	if (f)
	{
		time_t timep = time(NULL);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
		char log[600];
		sprintf_s(log, "%s ����%d��: argv[0] = %s, argv[1] = %s, arg[2] = %s\n", 
			tmp, argc, argv[0], argc>1 ? argv[1]:"null", argc>2 ? argv[2]:"null");
		fwrite(log, strlen(log), 1, f);
	}else 
		return 0xDEAD;
	if (1 == argc)
		LOG(f, "����ֻ��һ��.\n");

	char id[128];// ��ȡKeeper.conf "id" �ֶ�
	GetPrivateProfileStringA("module", "id", "", id, sizeof(id), folder.get("keep.conf"));
	if (id[0] == 0)
		LOG(f, "��ȡ�����ļ�ʧ��.\n");

	const char *name = argv[1];// �����ļ�
	char update[128];// ������ʱ�ļ�
	sprintf_s(update, "%s%s", name, POSTFIX);
	if (-1 == _access(folder.get(update), 0))
		LOG(f, "�����ļ�������.\n");
	char buf[128], exe[_MAX_PATH];
	sprintf(buf, "%s.exe", name);
	strcpy_s(exe, folder.get(buf));

	bool isKeeper = 0 == strcmp(name, "Keeper");// �Ƿ������ػ�����
	if (isKeeper)
	{
		// �������������ȴ��ػ�����
		HANDLE m = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, id);
		if (m && WAIT_TIMEOUT == WaitForSingleObject(m, WAIT_TIME*60000))
			LOG(f, "�ɰ��������ʹ����.\n");
		CloseHandle(m);
	}
	if (-1 == _access(folder.get(".old"), 0))
		_mkdir(folder.get(".old"));
	if (0 == _access(folder.get(".old", buf), 0))
		DeleteFileA(folder.get(".old", buf));
	CopyFileA(exe, folder.get(".old", buf), FALSE);
	for (int k = WAIT_TIME*60; FALSE == DeleteFileA(exe) && k; --k)
		Sleep(1000);

	if ( 0 == rename(folder.get(update), exe) )
	{
		if (isKeeper)
		{
			SHELLEXECUTEINFO ShExecInfo = { 0 };
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.lpFile = exe;
			ShExecInfo.lpParameters = argc > 2 ? argv[2] : NULL;
			if (ShellExecuteEx(&ShExecInfo))
				LOG_OK(f, "�����ػ�����ɹ�.\n");
			LOG(f, "�����ػ�����ʧ��.\n");
		}else 
			LOG_OK(f, "��������ɹ�.\n");
	}
	else
		LOG(f, "����������ռ��.\n");
}
