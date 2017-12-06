#ifndef __TIMERMANAGER_H__
#define __TIMERMANAGER_H__

#include <queue>
#include <list>
#include <set>
#include <Utills/utills.h>


namespace CoreDiam
{
typedef enum { COMMON, DPA } timer_type;
struct TimerData
{
	TimerData(const timer_type &type = COMMON)
		: m_TimeValue(0), m_type(type)
	{
	}
	long m_TimeValue;
	timer_type     m_type;
};


struct TimerDataCmpr
{
	bool operator () (const TimerData *_First, const TimerData *_Second) const
	{
		return (_First->m_TimeValue < _Second->m_TimeValue); // > 0;
	}
};


class TimerDataFactory
{
public:
	virtual ~TimerDataFactory() {}

	virtual TimerData * acquire(timer_type ttype) = 0;
	virtual void        release(TimerData *td) = 0;
};


class ITimerCallback
{
public:
	virtual ~ITimerCallback() {}
	virtual bool OnTimer(TimerData* _Timer) = 0;
	virtual bool OnDPATimer(TimerData* _Timer) = 0;
	virtual void Flush() = 0;
};


class ITimerManagerLock
{
public:
	virtual ~ITimerManagerLock() {}
	virtual void enter() = 0;
	virtual void leave() = 0;
};


class AutoTimerManagerLock
{
private:
	ITimerManagerLock &lock_;
public:
	AutoTimerManagerLock(ITimerManagerLock &lock): lock_(lock) {
		lock_.enter();
	}
	~AutoTimerManagerLock() {
		lock_.leave();
	}
};


class CTimerManager : public Task
{
private:
	typedef std::multiset<TimerData*, TimerDataCmpr> TIMERMAP;

	bool                       m_bExit;

	//ACE_Recursive_Thread_Mutex m_Mutex;
	//ACE_RW_Mutex               m_Mutex;
	//ACE_Thread_Mutex           m_Mutex;

	//ACE_Semaphore              m_TimerSemaphore;
	//ACE_Thread_Mutex           m_conditionMutex;
	//ACE_Condition_Thread_Mutex m_condition;

	size_t                     m_Count;
	size_t                     m_CreatedCount;
	size_t                     m_CancelledCount;
	size_t                     m_TriggeredCount;
	//size_t                     m_MaxPoint;
	//size_t                     m_TimePoint;

	//inline TimerData      * TakeTimer();
	/*inline */int              SignalTimers(int count);
	/*inline */long * CalculateTimeout(long &_RefSrc);

	void                relax(unsigned int msecs);
	void                updateTimePoint();

	/*inline */TimerData * acquireTimer(timer_type ttype);
	/*inline */void        releaseTimer(TimerData *td);

protected:
	Event       m_StartEvent;
	TIMERMAP        m_TimersMap;
	ITimerCallback *m_pCallback;
	ITimerManagerLock &m_Lock_;
	TimerDataFactory  *m_tdFactory_;

	virtual ~CTimerManager();

	virtual void svc();

public:
	typedef TIMERMAP::iterator  TimerReference;

	CTimerManager(ITimerCallback *_pCallback, ITimerManagerLock &lock, TimerDataFactory *tdFactory = NULL);

	virtual TimerReference SetTimer(int _MilliSeconds, timer_type type = COMMON);
	virtual int            CancelTimer(const TimerReference &_Timer);
	virtual int            UpdateTimer(TimerReference &_Timer, int _MilliSeconds);
	virtual void           Release();

	TimerReference         nullTimer();

	size_t                 GetCount();
	size_t                 GetCreatedCount();
	size_t                 GetCancelledCount();
	size_t                 GetTriggeredCount();
	size_t                 GetTimerMinPoint();
	size_t                 GetTimerMaxPoint();
	size_t                 GetPoint();
};

}

#endif //__TIMERMANAGER_H__
