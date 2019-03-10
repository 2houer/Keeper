#include "stdafx.h"
#include "MyThreadPool.h"
#include "Task.h"
#include<cassert>
#include<iostream>


/**
* @brief ����һ������ȷ�����̳߳�
* @param[in] nDefaultSize �̳߳�Ĭ������
*/
CMyThreadPool::CMyThreadPool(int nDefaultSize)
{
	m_nThreadNum = nDefaultSize;
	m_bIsExit = false;
	for(int i = 0; i < nDefaultSize; i++)
	{
		CMyThread *p = new CMyThread(this);
		m_IdleThreadStack.push(p);
		p->startThread();
	}
}

/// Ĭ����������
CMyThreadPool::~CMyThreadPool(void)
{
}

/**
* @brief �ı��̳߳صĴ�С
* @return nSize �̳߳ش�С
*/
void CMyThreadPool::ChangeSize(int nSize)
{
	m_mutex.Lock();
	if (nSize > m_nThreadNum)
	{
		while (nSize > m_nThreadNum) AddThread();
	}
	else if (nSize < m_nThreadNum)
	{
		while (nSize < m_nThreadNum) SubtractThread();
	}
	m_mutex.Unlock();
}

/** 
* @brief ���̴߳ӻ����ȡ������������߳�ջ��.��ȡ֮ǰ�жϴ�ʱ��������Ƿ�������.
* ���������Ϊ��ʱ�Ź���,������������ȡ�������ִ��.
*/
bool CMyThreadPool::SwitchActiveThread( CMyThread *pThread)
{
	CTask *pTask = NULL;
	m_mutex.Lock();
	if(pTask = m_TaskQueue.pop())//������в�Ϊ��,����ȡ����ִ��
	{
		pThread->assignTask(pTask);
		pThread->startTask();
	}
	else//�������Ϊ�գ����̹߳���
	{
		m_ActiveThreadList.removeThread(pThread);
		m_IdleThreadStack.push(pThread);
	}
	m_mutex.Unlock();
	return true;
}

/**
* @brief �������
* @param[in] *t ����(ָ��)
* @param[in] priority ���ȼ�,�����ȼ������񽫱����뵽����.
*/
void CMyThreadPool::addTask( CTask *t, PRIORITY priority )
{
	assert(t);
	if(!t || m_bIsExit)
		return;
	CTask *task = NULL;
	m_mutex.Lock();
	if(priority == PRIORITY::NORMAL)
	{
		m_TaskQueue.push(t);//�����������
	}
	else if(PRIORITY::HIGH)
	{
		m_TaskQueue.pushFront(t);//�����ȼ�����
	}

	if(!m_IdleThreadStack.isEmpty())//���ڿ����߳�,���ÿ����̴߳�������
	{
		if(task = m_TaskQueue.pop())//ȡ����ͷ����
		{
			CMyThread *pThread = m_IdleThreadStack.pop();
			m_ActiveThreadList.addThread(pThread);
			pThread->assignTask(task);
			pThread->startTask();
		}
	}
	m_mutex.Unlock();
}

/**
* @brief �����̳߳�
* @details clear m_TaskQueue��m_ActiveThreadList��m_IdleThreadStack
*/
void CMyThreadPool::destroyThreadPool()
{
	m_mutex.Lock();
	m_bIsExit = true;
	m_TaskQueue.clear();
	m_ActiveThreadList.clear();
	m_IdleThreadStack.clear();
	m_mutex.Unlock();
}
