/** 
* @file MyThreadPool.h
* @brief �ҵ��̳߳�
*/

#pragma once

#include "MyStack.h"
#include "MyList.h"
#include"MyQueue.h"
#include "MyThread.h"
#include "MyMutex.h"

class CTask;

/** 
* @enum PRIORITY 
* @brief �������ȼ� 
*/
enum PRIORITY
{
	NORMAL,			/**< �������ȼ� */
	HIGH			/**< �����ȼ� */
};

/** 
* @class CBaseThreadPool 
* @brief �̳߳ػ���
*/
class CBaseThreadPool
{
public:
	/// ���麯�����л���߳�
	virtual bool SwitchActiveThread(CMyThread *ptd) = 0;
};

/**
* @class CMyThreadPool 
* @brief �̳߳�
* @details �̳߳ذ�������Ҫ�أ������߳�ջ����߳������������
*/
class CMyThreadPool : public CBaseThreadPool
{
public:
	CMyThreadPool(int nDefaultSize);
	~CMyThreadPool(void);
	// �ı��̳߳صĴ�С
	void ChangeSize(int nSize);

private:
	/// �л���߳�
	virtual bool SwitchActiveThread(CMyThread* pThread);

public:
	/// ��������̳߳�
	void addTask(CTask *t, PRIORITY priority = PRIORITY::NORMAL);
	/// ��ʼ����
	bool start() { return /* ��ǰ������ */ 0; }
	/// �����̳߳�
	void destroyThreadPool();
	/// ��ȡ��̸߳���
	inline int GetActiveThreadNum() { m_mutex.Lock(); int s = m_ActiveThreadList.getSize(); m_mutex.Unlock(); return s; }
	/// ��ȡ�̳߳�����
	inline int GetThreadNum() { m_mutex.Lock(); int n = m_nThreadNum; m_mutex.Unlock(); return n; }
	/// �����Ƿ�ȫ��ִ�����
	inline bool IsTaskDone() { m_mutex.Lock(); bool b = m_TaskQueue.isEmpty(); m_mutex.Unlock(); return b; }
	/// �ȴ����л�߳��������[����Ϊmm]
	inline void Wait(int mm) { while (GetActiveThreadNum()) Sleep(mm); }

private:
	/// �̳߳�����
	int m_nThreadNum;
	/// �˳��ı��
	bool m_bIsExit;

	/// ������(�����������л�����ʱ)
	CMyMutex m_mutex;

	/// �����߳�ջ(���new�������߳�ָ��)
	CMyStack m_IdleThreadStack;
	/// ��߳��б�(���new�������߳�ָ��)
	CMyList m_ActiveThreadList;
	/// �������(���new����������ָ��)
	CMyQueue m_TaskQueue;

	/// ���̳߳������һ���̣߳����ص�ǰ�߳�����
	inline int SubtractThread()
	{
		Wait(10);
		CMyThread *pThread = m_IdleThreadStack.pop();
		if (pThread)
		{
			pThread->Exit();
			-- m_nThreadNum;
		}
		return m_nThreadNum;
	}

	/// ���̳߳������һ���̣߳����ص�ǰ�߳�����
	inline int AddThread()
	{
		Wait(10);
		CMyThread *pThread = new CMyThread(this);
		m_IdleThreadStack.push(pThread);
		pThread->startThread();
		++ m_nThreadNum;
		return m_nThreadNum;
	}
};
