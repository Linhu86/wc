#ifndef __thread_h__
#define __thread_h__

#include <pthread.h>

class Thread
{
  public:
    Thread();
    ~Thread();
    int create();
    int join();
    int detach();
    int set_priority(int sched_prio);
    int set_sched(int sched_pol);
    pthread_t self();
    void display_sched_attr();
    virtual void *run() = 0;

  private:
    pthread_t m_tid;
    int m_running;
    int m_detached;
    int m_policy;
    int m_prio;
};

#endif
