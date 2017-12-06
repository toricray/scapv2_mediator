#include "TimerManager.h"

namespace CoreDiam {

const unsigned long  EMPTY_WAIT_DURATION_MSEC = 1000;


enum {
	TIMERS_BATCH_SIZE = 10
};

CTimerManager::CTimerManager(ITimerCallback *_pCallback, ITimerManagerLock &lock, TimerDataFactory *tdFactory)
	: m_bExit(false),
	m_Count(0),
	m_CreatedCount(0),
	m_CancelledCount(0),
	m_TriggeredCount(0),
	m_pCallback(_pCallback)
	, m_Lock_(lock)
	, m_tdFactory_(tdFactory)
{

	activate();
	m_StartEvent.wait();
}
//----------------------------------------------------------------------------
CTimerManager::~CTimerManager()
{

}
//----------------------------------------------------------------------------
void CTimerManager::Release()
{
	{
	m_bExit = true;
	}

	wait();
	delete this;
}
//----------------------------------------------------------------------------
void CTimerManager::svc()
{

	long  waitPoint;
	long *waitPointPtr = 0;

	m_StartEvent.signal();

	while(!m_bExit) // 0 == we have timeout to notify of
	{
		{
			AutoTimerManagerLock autoLock(m_Lock_);
			waitPointPtr = CalculateTimeout(waitPoint);
		}

		if(!m_bExit)
		{
			if(waitPointPtr)
				relax(500);
			else
			{
				while(!m_bExit)
				{
					if(TIMERS_BATCH_SIZE > SignalTimers(TIMERS_BATCH_SIZE))
						break;
					
					relax(1);
				}
			}
		}
	}

	return ;
}
//----------------------------------------------------------------------------
void CTimerManager::relax(unsigned int msecs)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}

//----------------------------------------------------------------------------
CTimerManager::TimerReference CTimerManager::SetTimer(int _MilliSeconds, timer_type type)
{

	if(_MilliSeconds > 0)
	{
		TimerData *oldTopTimer = 0;
		//uint64_t   oldTopMSec  = 0;

		TIMERMAP::iterator itBegin = m_TimersMap.begin();
		if(itBegin != m_TimersMap.end())
		{
			oldTopTimer = *itBegin;
			//oldTopMSec = (*itBegin)->m_TimeValue;
		}

		TimerData *Timer = acquireTimer(type); //new TimerData(type);
		long currentTime = nowMSec();
		Timer->m_TimeValue = currentTime + _MilliSeconds;

		TimerReference tr = m_TimersMap.insert(Timer);

		m_Count++;
		m_CreatedCount++;

		if(!oldTopTimer)
		{
		}
		else
		{
			// we've got more than one timers now
			itBegin = m_TimersMap.begin();
			if(itBegin != m_TimersMap.end())
			{
			}
		}

		return tr;
	}

	return m_TimersMap.end(); //NULL;
}
//----------------------------------------------------------------------------
int CTimerManager::CancelTimer(const TimerReference &_Timer)
{
	TimerData *td = *_Timer;
	m_TimersMap.erase(_Timer);

	releaseTimer(td);

	m_Count--;
	m_CancelledCount++;

	return 0;
}
//----------------------------------------------------------------------------
int CTimerManager::UpdateTimer(TimerReference &_Timer, int _MilliSeconds)
{
	TimerData *td = *_Timer;
	m_TimersMap.erase(_Timer);
	td->m_TimeValue = nowMSec() + _MilliSeconds;
	_Timer = m_TimersMap.insert(td);

	return 0;
}

//----------------------------------------------------------------------------
int CTimerManager::SignalTimers(int count)
{
	int             timersCount = 0;
	TimerData      *timer;
	long  currentTime(nowMSec());

	{
		AutoTimerManagerLock autoLock(m_Lock_);

		TIMERMAP::iterator it    = m_TimersMap.begin();

		while(timersCount < count && it != m_TimersMap.end())
		{
			timer = *it;
			if(currentTime < timer->m_TimeValue)
				break;

			timersCount++;

			m_Count--;
			m_TriggeredCount++;

			if(timer->m_type == COMMON)
				m_pCallback->OnTimer(timer);
			else
				m_pCallback->OnDPATimer(timer);

			m_TimersMap.erase(it++);

			releaseTimer(timer); //delete timer; // TODO: move out of the lock
		}
	}

	m_pCallback->Flush();

	return timersCount;
}
//----------------------------------------------------------------------------
/*inline */long * CTimerManager::CalculateTimeout(long &_RefSrc)
{
	long currentTime(nowMSec());

	TIMERMAP::iterator it = m_TimersMap.begin();
	if(it != m_TimersMap.end())
	{
		_RefSrc = (*it)->m_TimeValue;
		//m_Mutex.release();
		if(_RefSrc <= currentTime)
			return 0;

	}
	_RefSrc = currentTime;
	_RefSrc += 1000;
	return &_RefSrc;
}
//----------------------------------------------------------------------------
TimerData * CTimerManager::acquireTimer(timer_type ttype)
{
	if(m_tdFactory_)
		return m_tdFactory_->acquire(ttype);
	return new TimerData(ttype);
}
//----------------------------------------------------------------------------
void CTimerManager::releaseTimer(TimerData *td)
{
	if(m_tdFactory_)
	{
		m_tdFactory_->release(td);
		return;
	}
	delete td;
}

CTimerManager::TimerReference CTimerManager::nullTimer()
{
	return m_TimersMap.end();
}
//----------------------------------------------------------------------------
size_t CTimerManager::GetCount()
{
	AutoTimerManagerLock autoLock(m_Lock_);
	size_t count = m_TimersMap.size();
	return count;
}
//----------------------------------------------------------------------------
size_t CTimerManager::GetCreatedCount()
{
	AutoTimerManagerLock autoLock(m_Lock_);
	size_t count = m_CreatedCount;
	return count;
}
//----------------------------------------------------------------------------
size_t CTimerManager::GetCancelledCount()
{
	AutoTimerManagerLock autoLock(m_Lock_);
	size_t count = m_CancelledCount;
	return count;
}
//----------------------------------------------------------------------------
size_t CTimerManager::GetTriggeredCount()
{
	AutoTimerManagerLock autoLock(m_Lock_);
	size_t count = m_TriggeredCount;
	return count;
}


}
