#ifndef __thread_h__
#define __thread_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "thread.hpp"
#include "wqueue.hpp"
    
typedef enum thread_producer_state {
  THREAD_PRODUCER_START,
  THREAD_PRODUCER_STOP
} thread_producer_state_t;

typedef enum thread_consumer_state {
  THREAD_CONSUMER_START,
  THREAD_CONSUMER_STOP
} thread_consumer_state_t;

static pthread_mutex_t thread_mutex;
static pthread_cond_t thread_cond;

static thread_producer_state_t producer_state = THREAD_PRODUCER_START;
static thread_consumer_state_t consumer_state = THREAD_CONSUMER_START;

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

class WorkItem
{
    string m_message;
    int    m_number;
 
  public:
    WorkItem(const char* message, int number) 
          : m_message(message), m_number(number) {}
    ~WorkItem() {}
 
    const char* getMessage() { return m_message.c_str(); }
    int getNumber() { return m_number; }
};

template<class T>
class ConsumerThread : public Thread
{
  public:
    ConsumerThread(wqueue<T>& queue) : m_queue(queue) {}
 
    void* run() {
       while(consumer_state != THREAD_CONSUMER_STOP){

         printf("Consumer thread %lu: is running.\n", (long unsigned int)self()); 

         int send_signal = 0;
         
         if(m_queue.is_empty())
         {
           pthread_mutex_lock(&thread_mutex);
           printf("Consumer thread %lu: Empty queue, waiting some one to produce.\n", (long unsigned int)self());
           pthread_cond_wait(&thread_cond,&thread_mutex);
           pthread_mutex_unlock(&thread_mutex);
         }

         if(m_queue.is_full())
         {
            send_signal = 1;
            printf("Consumer thread %lu: Full queue, consume one and send signal for someone to produce.\n", (long unsigned int)self());
         }

         printf("Consumer thread %lu: - waiting for item...\n", 
               (long unsigned int)self());
         T item = m_queue.remove();
         printf("Consumer thread %lu: - got one item\n", 
               (long unsigned int)self());
         printf("Consumer thread %lu: - item: message - %s, number - %d\n", 
               (long unsigned int)self(), item->getMessage(), 
                item->getNumber());
         delete item;

         if(send_signal == 1)
         {
             pthread_mutex_lock(&thread_mutex);
             pthread_cond_signal(&thread_cond);
             pthread_mutex_unlock(&thread_mutex);

         }

         printf("Consumer thread %lu: resource available %d\n", (long unsigned int)self(), m_queue.size());
         sleep(2);

         return NULL;
       }
    }

  private:
    wqueue<T>& m_queue;

};

template<class T>
class ProducerThread : public Thread
{
  public:
    ProducerThread(wqueue<T> &queue) : m_queue(queue){}

    void* run() {

      while(producer_state != THREAD_PRODUCER_STOP){

        printf("Producer thread %lu: is running.\n", (long unsigned int)self());   

        int send_signal = 0;
            
        if(m_queue.is_full())
        {
          pthread_mutex_lock(&thread_mutex);
          printf("Producer thread %lu: queue is full, waiting for someone to consumer it.\n", (long unsigned int)self());
          pthread_cond_wait(&thread_cond, &thread_mutex);
          pthread_mutex_unlock(&thread_mutex);
        }

        if(m_queue.is_empty())
        {
          send_signal = 1;
          printf("Producer thread %lu: queue is empty, produce one and send signal for someone to consumer it.\n", (long unsigned int)self());
        }

        T item = new WorkItem("product:", (long unsigned int)self());
        m_queue.add(item);

        if(send_signal == 1)
        {
          pthread_mutex_lock(&thread_mutex);
          pthread_cond_signal(&thread_cond);
          pthread_mutex_unlock(&thread_mutex);      
        }

        printf("Producer thread %lu: resource available %d\n", (long unsigned int)self(), m_queue.size());
        sleep(1);
     }
     return NULL;
   }

  private:
    wqueue<T>& m_queue;
};


#endif
