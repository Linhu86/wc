#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "thread.hpp"
#include "wqueue.hpp"


int main()
{
    // Create the queue and consumer (worker) threads
    wqueue<WorkItem*>  queue(1024);

    ConsumerThread<WorkItem*> * consumer_thread1 = new ConsumerThread<WorkItem*>(queue);
    ConsumerThread<WorkItem*> * consumer_thread2 = new ConsumerThread<WorkItem*>(queue);

    consumer_thread1->set_sched(SCHED_RR, 10);
    consumer_thread2->set_sched(SCHED_RR, 10);



    printf("Create 2 Consumer threads.\n");
    consumer_thread1->create();
    consumer_thread2->create();

    sleep(2);

    ProducerThread <WorkItem*> * producer_thread1 = new ProducerThread <WorkItem*> (queue);
    ProducerThread <WorkItem*> * producer_thread2 = new ProducerThread <WorkItem*> (queue);

    producer_thread1->set_sched(SCHED_RR, 10);
    producer_thread2->set_sched(SCHED_RR, 10);

    printf("Create 2 Producer threads.\n");

    producer_thread1->create();
    producer_thread2->create();

    sleep(10);

    ConsumerThread <WorkItem*> * consumer_thread3 = new ConsumerThread <WorkItem*> (queue);
    ConsumerThread <WorkItem*> * consumer_thread4 = new ConsumerThread <WorkItem*> (queue);

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



