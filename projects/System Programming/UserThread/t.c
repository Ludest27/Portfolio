#include <stdio.h>
#include "type.h"

PROC proc[NPROC];
PROC *freeList;
PROC *readyQueue;
PROC *sleepList;
PROC *running;

#include "queue.c"

// Forward declarations
void func(void *parm);
int scheduler();
int do_exit();
int tswitch();
int texit(int value);

int init() {
    int i;
    PROC *p;
    for (i=0; i<NPROC; i++) {
        p = &proc[i];
        p->pid = i;
        p->priority = 0;
        p->status = FREE;
        p->event = 0;
        p->joinPid = 0;
        p->joinPtr = 0;
        p->next = p+1;
    }
    proc[NPROC-1].next = 0;
    freeList = &proc[0];
    readyQueue = 0;
    sleepList = 0;
    
    running = p = dequeue(&freeList);
    p->status = READY;
    p->priority = 0;
    printList("freeList", freeList);
    printf("init complete: P0 running\n");
    return 0;
}

int scheduler() {
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    printf("next running = %d\n", running->pid);
    return 0;
}

int texit(int value) {
    printf("task %d in texit value=%d\n", running->pid, value);
    running->status = FREE;
    running->priority = 0;
    enqueue(&freeList, running);
    printList("freeList", freeList);
    tswitch();
    return 0;
}

int do_exit() {
    texit(running->pid);
    return 0;
}

int create(void (*f)(), void *parm) {
    int i;
    PROC *p = dequeue(&freeList);
    if (!p) {
        printf("create failed\n");
        return -1;
    }
    p->status = READY;
    p->priority = 1;
    p->joinPid = 0;
    p->joinPtr = 0;

    for (i=1; i<13; i++)
        p->stack[SSIZE-i] = 0;
    p->stack[SSIZE-1] = (int)parm;
    p->stack[SSIZE-2] = (int)do_exit;  // Now do_exit is declared before this
    p->stack[SSIZE-3] = (int)f;
    p->ksp = (int)&p->stack[SSIZE-12];
    enqueue(&readyQueue, p);
    printList("readyQueue", readyQueue);
    printf("task %d created a new task %d\n", running->pid, p->pid);
    return p->pid;
}

int do_create() {
    int pid = create(func, (void *)(long)running->pid);  // Fixed cast
    return pid;
}

int do_switch() {
    tswitch();
    return 0;
}

void func(void *parm) {
    int c;
    printf("task %d start: parm = %d\n", running->pid, (int)(long)parm);  // Fixed cast
    while(1) {
        printf("task %d running\n", running->pid);
        printList("readyQueue", readyQueue);
        printf("enter a key [c|s|q] : ");
        c = getchar(); getchar();  // kill \r
        switch(c) {
            case 'c': do_create(); break;
            case 's': do_switch(); break;
            case 'q': do_exit(); break;
        }
    }
}

int main() {
    printf("Welcome to the MT User-Level Threads System\n");
    init();
    create((void *)func, 0);
    printf("P0 switch to P1\n");
    while(1) {
        if (readyQueue)
            tswitch();
    }
    return 0;
}
