#include "thread.hpp"
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


static void * runThread(void *arg)
{
  return ((Thread *)arg)->run();
}

Thread :: Thread() : m_tid(0), m_running(0), m_detached(0) 
{
    int ret;
    ret = pthread_attr_init(&m_attr);
    if (ret != 0)
    {
      printf("pthread [%lu] pthread_attr_init error: %s\n", (long unsigned int)m_tid, strerror(errno));
    }

}

Thread :: ~Thread()
{

  if(m_running == 1 && m_detached == 0)
  {
    pthread_detach(m_tid);
  }
  if(m_running == 1)
  {
    pthread_cancel(m_tid);
  }

  pthread_attr_destroy(&m_attr);
}

int Thread :: create()
{
  int result = 0;
  result = pthread_create(&m_tid, NULL, runThread, this);
  if(result == 0)
  {
    m_running = 1;
  }
  return m_running;
}

int Thread :: join()
{
  int result = 0;

  if(m_running == 1)
  {
    result = pthread_join(m_tid, NULL);
    if(result == 0)
    {
      m_detached = 0;
    }
  }
  return result;
}

int Thread :: detach()
{
  int result = 0;

  if(m_running == 1 && m_detached == 0)
  {
    result = pthread_detach(m_tid);
    if(result == 0)
    {
      m_detached = 1;
    }
  }
  return result;
}

pthread_t Thread :: self()
{
  return m_tid;
}

int Thread :: set_sched(int sched_pol, int sched_prio)
{
  m_policy = sched_pol;
  m_prio = sched_prio;

  int ret = 0;

  pthread_attr_setschedpolicy(&m_attr, m_policy);
  if (ret != 0){
    printf("pthread_attr_setschedpolicy error: %s\n", strerror(errno));
    return 1;
  }

  m_param.sched_priority = m_prio;

  ret = pthread_attr_setschedparam(&m_attr, &m_param);
  if (ret != 0){
    printf("pthread_attr_setschedparam error: %s\n", strerror(errno));
    return 1;
  }
  
  display_sched_attr();

  return 0;
}

void Thread :: display_sched_attr()
{
  int ret;
  int policy;

  ret = pthread_attr_getschedparam(&m_attr, &m_param);
  if (ret != 0)
  {
    printf("pthread [%lu] pthread_attr_getschedparam error: %s\n", (long unsigned int)m_tid, strerror(errno));
  }

  ret = pthread_attr_getschedpolicy(&m_attr, &policy);
  if (ret != 0)
  {
    printf("pthread [%lu] pthread_attr_getschedpolicy error: %s\n", (long unsigned int)m_tid, strerror(errno));
  }

  printf("pthread [%lu] properties: policy=%s, priority=%d\n", (long unsigned int)m_tid,
        (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
        (policy == SCHED_RR)    ? "SCHED_RR" :
        (policy == SCHED_OTHER) ? "SCHED_OTHER" :
        "???",
        m_param.sched_priority);

} 




