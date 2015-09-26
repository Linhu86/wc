#ifndef __wqueue_h__
#define __wqueue_h__

#include <pthread.h>
#include <list>

using namespace std;

template <typename T>
class wqueue
{

  public:
    wqueue(int size) : max_list_size(size) {
      pthread_mutex_init(&m_mutex, NULL);
      pthread_cond_init(&m_condv, NULL);
    }
    ~wqueue() {
      pthread_mutex_destroy(&m_mutex);
      pthread_cond_destroy(&m_condv);
    }
    void add(T item) {
      pthread_mutex_lock(&m_mutex);
      m_queue.push_back(item);
      pthread_mutex_unlock(&m_mutex);
    }
    T remove() {
      pthread_mutex_lock(&m_mutex);
      T item = m_queue.front();
      m_queue.pop_front();
      pthread_mutex_unlock(&m_mutex);
      return item;
    }
    int size() {
      pthread_mutex_lock(&m_mutex);
      int size = m_queue.size();
      pthread_mutex_unlock(&m_mutex);
      return size;
    }

    int is_empty()
    {
      int ret = 0;
      if (size() == 0)
      {
        ret = 1;
      }
      return ret;
    }

    int is_full()
    {
      int ret = 0;
      if (size() == max_list_size)
      {
        ret = 1;
      }
      return ret;
    }

    pthread_mutex_t get_mutex()
    {
      return m_mutex;
    }

    pthread_cond_t get_cond()
    {
      return m_condv;
    }

  private:
    list<T>          m_queue;
    pthread_mutex_t  m_mutex;
    pthread_cond_t   m_condv;
    int              max_list_size;
};

#endif


