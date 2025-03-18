#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "queue.h"
#include "wait.h"
#include "sem.h"

extern int tswitch();
PROC proc[NPROC], *freeList, *sleepList, *readyQueue, *running;

#define NBUF 4
#define N 8
int buf[NBUF], head, tail;
SEMAPHORE full, empty, mutex;

void producer(void);  // Forward declarations
void consumer(void);
void task1(void);
int create(void (*f)(), void *parm);

int do_create() {
    return create(producer, (void *)(long)running->pid);
}

void producer() {
    int i;
    printf("producer %d start\n", running->pid);
    for (i=0; i<N; i++) {
        P(&empty);
        P(&mutex);
        buf[head++] = i+1;
        printf("producer %d: item = %d\n", running->pid, i+1);
        head %= NBUF;
        V(&mutex);
        V(&full);
    }
    printf("producer %d exit\n", running->pid);
    texit(0);
}

void consumer() {
    int i, c;
    printf("consumer %d start\n", running->pid);
    for (i=0; i<N; i++) {
        P(&full);
        P(&mutex);
        c = buf[tail++];
        tail %= NBUF;
        printf("consumer %d: got item = %d\n", running->pid, c);
        V(&mutex);
        V(&empty);
    }
    printf("consumer %d exit\n", running->pid);
    texit(0);
}

int init() {
    int i;
    PROC *p;
    for (i=0; i<NPROC; i++) {
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;
        p->next = p+1;
    }
    proc[NPROC-1].next = 0;
    freeList = &proc[0];
    readyQueue = sleepList = 0;
    
    p = running = (PROC *)dequeue(&freeList);
    p->status = READY;
    p->priority = 0;
    
    head = tail = 0;
    full.value = 0; full.queue = 0;
    empty.value = NBUF; empty.queue = 0;
    mutex.value = 1; mutex.queue = 0;
    
    printf("init complete\n");
    return 0;
}

int scheduler() {
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = (PROC *)dequeue(&readyQueue);
    return 0;
}

int create(void (*f)(), void *parm) {
    int i;
    PROC *p = (PROC *)dequeue(&freeList);
    if (!p) return -1;
    
    p->status = READY;
    p->priority = 1;
    
    for (i=1; i<13; i++)
        p->stack[SSIZE-i] = 0;
    p->stack[SSIZE-1] = (int)parm;
    p->stack[SSIZE-2] = (int)texit;
    p->stack[SSIZE-3] = (int)f;
    p->ksp = (int)&p->stack[SSIZE-12];
    
    enqueue(&readyQueue, p);
    return p->pid;
}

int main() {
    printf("Welcome to the MT User-Level Threads System\n");
    init();
    create((void *)producer, 0);
    create((void *)consumer, 0);
    while(1) {
        if (readyQueue)
            tswitch();
    }
    return 0;
}
