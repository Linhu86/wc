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

pthread_mutex_t m_mutex;
pthread_cond_t m_cond;

thread_producer_state_t producer_state = THREAD_PRODUCER_START;
thread_consumer_state_t consumer_state = THREAD_CONSUMER_START;

 
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

class ConsumerThread : public Thread
{
  public:
    ConsumerThread(wqueue<WorkItem*>& queue) : m_queue(queue) {}
 
    void* run() {
       while(consumer_state != THREAD_CONSUMER_STOP){

         printf("Consumer thread %lu: is running.\n", (long unsigned int)self()); 

         int send_signal = 0;
         
         if(m_queue.is_empty())
         {
           pthread_mutex_lock(&m_mutex);
           printf("Consumer thread %lu: Empty queue, waiting some one to produce.\n", (long unsigned int)self());
           pthread_cond_wait(&m_cond,&m_mutex);
           pthread_mutex_unlock(&m_mutex);
         }

         if(m_queue.is_full())
         {
            send_signal = 1;
            printf("Consumer thread %lu: Full queue, consume one and send signal for someone to produce.\n", (long unsigned int)self());
         }

         printf("Consumer thread %lu: - waiting for item...\n", 
               (long unsigned int)self());
         WorkItem* item = m_queue.remove();
         printf("Consumer thread %lu: - got one item\n", 
               (long unsigned int)self());
         printf("Consumer thread %lu: - item: message - %s, number - %d\n", 
               (long unsigned int)self(), item->getMessage(), 
                item->getNumber());
         delete item;

         if(send_signal == 1)
         {
             pthread_mutex_lock(&m_mutex);
             pthread_cond_signal(&m_cond);
             pthread_mutex_unlock(&m_mutex);

         }

         printf("Consumer thread %lu: resource available %d\n", (long unsigned int)self(), m_queue.size());
         sleep(2);
       }
    }

  private:
    wqueue<WorkItem*>& m_queue;

};

class ProducerThread : public Thread
{
  public:
    ProducerThread(wqueue<WorkItem *> &queue) : m_queue(queue){}

    void* run() {

      while(producer_state != THREAD_PRODUCER_STOP){

        printf("Producer thread %lu: is running.\n", (long unsigned int)self());   

        int send_signal = 0;
            
        if(m_queue.is_full())
        {
          pthread_mutex_lock(&m_mutex);
          printf("Producer thread %lu: queue is full, waiting for someone to consumer it.\n", (long unsigned int)self());
          pthread_cond_wait(&m_cond, &m_mutex);
          pthread_mutex_unlock(&m_mutex);
        }

        if(m_queue.is_empty())
        {
          send_signal = 1;
          printf("Producer thread %lu: queue is empty, produce one and send signal for someone to consumer it.\n", (long unsigned int)self());
        }

        WorkItem *item = new WorkItem("product:", (long unsigned int)self());
        m_queue.add(item);

        if(send_signal == 1)
        {
          pthread_mutex_lock(&m_mutex);
          pthread_cond_signal(&m_cond);
          pthread_mutex_unlock(&m_mutex);      
        }

        printf("Producer thread %lu: resource available %d\n", (long unsigned int)self(), m_queue.size());
        sleep(1);
       
     }
   }

  private:
    wqueue<WorkItem *>& m_queue;
};

int main()
{
    // Create the queue and consumer (worker) threads
    wqueue<WorkItem*>  queue(1024);

    ConsumerThread* consumer_thread1 = new ConsumerThread(queue);
    ConsumerThread* consumer_thread2 = new ConsumerThread(queue);

    consumer_thread1->set_sched(SCHED_RR, 10);
    consumer_thread2->set_sched(SCHED_RR, 10);



    printf("Create 2 Consumer threads.\n");
    consumer_thread1->create();
    consumer_thread2->create();

    sleep(2);

    ProducerThread* producer_thread1 = new ProducerThread(queue);
    ProducerThread* producer_thread2 = new ProducerThread(queue);

    producer_thread1->set_sched(SCHED_RR, 10);
    producer_thread2->set_sched(SCHED_RR, 10);

    printf("Create 2 Producer threads.\n");

    producer_thread1->create();
    producer_thread2->create();

    sleep(10);

    ConsumerThread* consumer_thread3 = new ConsumerThread(queue);
    ConsumerThread* consumer_thread4 = new ConsumerThread(queue);

    consumer_thread3->set_sched(SCHED_RR, 10);
    consumer_thread4->set_sched(SCHED_RR, 10);

    printf("!!!!!!!!!!!!!!!!!! Create 2 more Consumer threads !!!!!!!!!!!!!\n");
        
    consumer_thread3->create();
    consumer_thread4->create();



    // Wait for the queue to be empty
    while (1)
    {
    }
    printf("done\n");
    return 0;
}



