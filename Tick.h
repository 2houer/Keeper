#pragma once

#include <time.h>

/** 
* @class tick
* @brief ������ʱ��[8 bytes]
*/
class tick
{
protected:
	/// ��ʼʱ��
	clock_t start;
	/// ��ǰʱ��
	inline clock_t now() const { return clock(); }
public:
	/// ���첢��ʱ
	tick() : start(now()){ }
	/// ��ʼ��ʱ
	inline void begin() { start = now(); }
	/// ���¼�ʱ
	inline void reset() { start = now(); }
	/// ��ʱ��ms��
	inline clock_t time() const { return (now() - start); }
};

/** 
* @class tick_s
* @brief ����ʱ��[8 bytes]
*/
class tick_s
{
protected:
	/// ��ʼʱ��
	time_t start;
	/// ��ǰʱ��
	inline time_t now() const { return ::time(NULL); }
public:
	/// ���첢��ʱ
	tick_s() : start(now()){ }
	/// ��ʼ��ʱ
	inline void begin() { start = now(); }
	/// ���¼�ʱ
	inline void reset() { start = now(); }
	/// ��������ʱ��
	inline void set_beginTime(int nBegin) { start = nBegin; }
	/// ��ʱ��s��
	inline time_t time() const { return (now() - start); }
};
