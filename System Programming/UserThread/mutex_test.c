#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "queue.h"
#include "wait.h"
#include "mutex.h"

extern int tswitch();
PROC proc[NPROC], *freeList, *sleepList, *readyQueue, *running;

#define N 4
int A[N][N];
MUTEX *mp;
int total;

void task1(void *parm);  // Forward declaration
void func(void *arg);    // Forward declaration
int create(void (*f)(), void *parm);

int do_create() {
    return create(func, (void *)(long)running->pid);
}

void func(void *arg) {
    int i, row, s;
    int me = running->pid;
    row = (int)(long)arg;
    printf("task %d computes sum of row %d\n", me, row);
    s = 0;
    for (i=0; i < N; i++) {
        s += A[row][i];
    }
    printf("task %d update total with %d\n", me, s);
    mutex_lock(mp);
    total += s;
    printf("[total = %d] ", total);
    mutex_unlock(mp);
    texit(0);
}

void task1(void *parm) {
    int pid[N], i, status;
    int me = running->pid;
    printf("task %d: create working tasks : ", me);
    for(i=0; i < N; i++) {
        pid[i] = create(func, (void *)(long)i);
        printf("%d ", pid[i]);
    }
    printf(" to compute matrix row sums\n");
    
    for(i=0; i<N; i++) {
        printf("task %d tries to join with task %d\n", running->pid, pid[i]);
        join(pid[i], &status);
    }
    printf("task %d : total = %d\n", me, total);
    texit(0);
}

int init() {
    int i, j;
    PROC *p;
    for (i=0; i<NPROC; i++) {
        p = &proc[i];
        p->pid = i;
        p->priority = 0;
        p->status = FREE;
        p->next = p+1;
    }
    proc[NPROC-1].next = 0;
    freeList = &proc[0];
    readyQueue = sleepList = 0;
    
    p = running = (PROC *)dequeue(&freeList);
    p->status = READY;
    p->priority = 0;
    
    printf("P0: initialize A matrix\n");
    for(i=0; i<N; i++) {
        for(j=0; j<N; j++) {
            A[i][j] = i*N + j + 1;
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }
    
    mp = mutex_create();
    total = 0;
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
    create((void *)task1, 0);
    while(1) {
        if (readyQueue)
            tswitch();
    }
    return 0;
}
