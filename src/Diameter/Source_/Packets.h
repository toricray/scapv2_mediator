#ifndef __PACKETS_H__
#define __PACKETS_H__

#include <string.h>

#include <list>
#include <vector>
#include <set>


namespace CoreDiam {

//#define SENDQUEUESIZE 1000

class Packet
{
protected:
	Packet        *m_Next;
	unsigned char *m_Data;
	size_t         m_Size;
public:
	Packet(const unsigned char * _Data, size_t _Size)
		: m_Next(0), m_Size(_Size)
	{
		m_Data = new(std::nothrow) unsigned char[_Size];
		if(!m_Data)
			m_Size = 0;
		else
			memcpy(m_Data, _Data, _Size);
	}

	~Packet()
	{
		delete [] m_Data;
	}

public:
	inline const unsigned char * data()
	{
		return m_Data;
	}

	inline size_t size()
	{
		return m_Size;
	}

	bool assign(const unsigned char * _Data, size_t _Size)
	{
		delete[] m_Data;
		m_Size = 0;
		m_Data = new(std::nothrow) unsigned char[_Size];
		if(!m_Data)
			return false;
		m_Size = _Size;
		memcpy(m_Data, _Data, _Size);
		return true;
	}

	inline void next(T *item)
	{
		m_Next = item;
	}

	inline T * next()
	{
		return m_Next;
	}
};


template<typename T> class LazyQueue
{
private:
	T             *head_;
	//uint08         pad1_[CACHE_LINE_SIZE - sizeof(T *)];
	T             *tail_;
	//uint08         pad2_[CACHE_LINE_SIZE - sizeof(T *)];
	//bool           isActive_;
	//uint08         pad3_[CACHE_LINE_SIZE - sizeof(bool)];

public:
	LazyQueue()
		: head_(0)
		, tail_(0)
		//, isActive_(true)
	{
		//head_ = tail_ = new T();
		//head_->next(0);
	}

	~LazyQueue()
	{
		//if(head_)
		//	delete head_;
	}

	void enqueue(T *item)
	{
		item->next(0);
		// atomic {
		if(head_)
			tail_->next(item);
		else
			head_ = item;
		// } // atomic
		tail_ = item;
		return true;
	}

	bool dequeue(T **item)
	{
		if(head_)
		{
			*item = head_;
			head_ = reinterpret_cast<T *>(head_->next());
			return true;
		}

		// queue is empty
		*item = 0;
		return false;
	}

	//void start() {lock_.enter(); isActive_ = true;  lock_.leave();}
	//void stop()  {lock_.enter(); isActive_ = false; lock_.leave();}

	bool isEmpty()
	{
		// atomic {
		bool status = (0 == head_);
		// } // atomic
		return status;
	}
};

template<typename T>
class PacketsQueue
{
private:
	typedef LazyQueue<T> QueueType_;

	QueueType_  m_QueueNormal;
	QueueType_  m_QueueHigh;
	T          *m_Front;
	size_t      m_Count;
public:
	PacketsQueue(): m_Front(0), m_Count(0) {}
	~PacketsQueue() {}

	T      * dequeue() {
		if(m_Front)
		{
			m_Count--;
			T *item = m_Front;
			m_Front = 0;
			return item;
		}

		T *item = m_QueueHigh.dequeue();
		if(!item)
			item = m_QueueNormal.dequeue();

		if(item)
			m_Count--;

		return item;
	}

	size_t   enqueue(T *item, bool isHighPriority = false) {
		m_Count++;

		if(isHighPriority)
			m_QueueHigh.enqueue(item);
		else
			m_QueueNormal.enqueue(item);

		return m_Count;
	}

	T      * front() {
		if(m_Front)
			return m_Front;

		m_Front = m_QueueHigh.dequeue();
		if(!m_Front)
			m_Front = m_QueueNormal.dequeue();

		return m_Front;
	}

	size_t count() {
		return m_Count;
	}
};

}

#endif  // __PACKETS_H__
