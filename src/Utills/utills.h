#ifndef UTILLS_H
#define UTILLS_H

#include <condition_variable>
#include <atomic>

#include <thread>
#include <chrono>
#include <queue>

/*
 * Что-то вроде SDK из всяких маленьких штук, из-за которых как-то неловко тащить большие библиотеки
 */

typedef unsigned long long int uint64;

//Парсинг времени в формате 2017-05-04 03:02:01
static time_t stringToTime(const char* str)
{
    time_t result = 0;
    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;

    if (sscanf(str, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec) != 6)
    {
        return 0;
    }

    struct tm breakdown = {0};
    breakdown.tm_year = year - 1900;
    breakdown.tm_mon = month - 1;
    breakdown.tm_mday = day;
    breakdown.tm_hour = hour;
    breakdown.tm_min = min;

    result = mktime(&breakdown);
    return result;
}

//текущее время формате 2017-05-04 03:02:01
static const char*  currentDateTimeStr()
{
    time_t     now = time(0);
    struct tm  tstruct;
    static char  timeBuffer[80];
    tstruct = *localtime(&now);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %X", &tstruct);
    return timeBuffer;
}

class Semaphore
{
    unsigned int            count_;
    std::mutex              mutex_;
    std::condition_variable condition_;

public:
     Semaphore(unsigned int initial_count)
            : count_(initial_count),
              mutex_(),
              condition_()
    {
    }

    void signal()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ == 0)
        {
            condition_.wait(lock);
        }
        --count_;
    }
};

//Абстрактный класс для создания модулей с потоком (чем-то похож на одноименный класс из ACE, создавался как замена ему)
class Task
{
    std::thread* thread;
protected:
    virtual void svc(void) = 0;

    void activate()
    {
        thread = new std::thread(&Task::svc, this);
    }
public:
    Task():thread(NULL){}
    virtual ~Task()
    {
        thread->detach();
        delete thread;
    }
};

class Event
{
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> ready;

public:
    Event():ready(false)
    {

    }
    void reset()
    {
        std::unique_lock<std::mutex> lck(mtx);
        ready = false;
    }
    void wait()
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!ready) cv.wait(lck);
    }
    void signal()
    {
        std::unique_lock<std::mutex> lck(mtx);
        ready = true;
        cv.notify_all();
    }
};

//текущее время в миллисекундах
static long nowMSec()
{
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    return  ms.count();
}

//Потокобезопасная std::queue
template <class T> class SafeQueue
{
public:

private:
    std::queue<T> q;
    std::mutex mutex;
    bool isActiveFlag;
public:

    SafeQueue():isActiveFlag(false)
    {

    };
    void start()
    {
        std::lock_guard<std::mutex> lock(mutex);
        isActiveFlag = true;
    };

    void stop()
    {
        std::lock_guard<std::mutex> lock(mutex);
        isActiveFlag = false;
    };

    void enqueue(T msg)
    {
        std::lock_guard<std::mutex> lock(mutex);
        q.push(msg);
    };

    T dequeue()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (q.size() && isActive())
        {
            T msg = q.front();
            q.pop();
            return msg;
        }
        else
            return NULL;
    };

    bool isActive()
    {
        return isActiveFlag;
    }
};

//Пул объектов. Заполняется извне. Предполагается использовать, чтобы избежать частых выделений памяти.
template<class T> class ObjectPool
{
    Semaphore           sema;
    std::mutex          mutex;
    std::queue<T *>     pool;
public:
    ObjectPool(): sema(0)
    {
    };
    ~ObjectPool()
    {
        while(!pool.empty())
        {
            T *v = pool.front();
            pool.pop();
            delete v;
        }
    };

    T* get()
    {
        sema.wait();
        T *cs;
        {
            std::lock_guard<std::mutex> lock(mutex);
            cs = pool.front();
            pool.pop();
        }
        return cs;
    };

    void push(T *obj)
    {
        std::lock_guard<std::mutex> lock(mutex);
        pool.push(obj);
        sema.signal();
    };

    size_t size()
    {
        return pool.size();
    }
};

#endif //UTILLS_H
