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
    int set_sched(int sched_pol, int sched_prio);
    pthread_t self();
    void display_sched_attr();
    virtual void *run() = 0;

  private:
    pthread_t m_tid;
    pthread_attr_t m_attr;
    struct sched_param m_param;
    int m_running;
    int m_detached;
    int m_policy;
    int m_prio;
};

#endif
