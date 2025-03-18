#include "type.h"
#include <stdio.h>

extern PROC proc[NPROC];
extern PROC *running;
extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC *freeList;

int tsleep(int event) {
    running->event = event;
    running->status = SLEEP;
    enqueue(&sleepList, running);
    tswitch();
    return 0;
}

int twakeup(int event) {
    PROC *p, *q = 0;
    while((p = dequeue(&sleepList)) != 0) {
        if (p->event == event) {
            p->status = READY;
            enqueue(&readyQueue, p);
        }
        else
            enqueue(&q, p);
    }
    sleepList = q;
    return 0;
}

int texit(int exitValue) {
    running->exitStatus = exitValue;
    running->status = ZOMBIE;
    twakeup(running->pid);
    tswitch();
    return 0;
}

int join(int targetPid, int *status) {
    PROC *p;
    while(1) {
        for(p = &proc[1]; p < &proc[NPROC]; p++) {
            if (p->pid == targetPid) {
                if (p->status == ZOMBIE) {
                    *status = p->exitStatus;
                    p->status = FREE;
                    enqueue(&freeList, p);
                    return p->pid;
                }
                running->joinPid = targetPid;
                running->joinPtr = p;
                tsleep(targetPid);
                break;
            }
        }
        if (p == &proc[NPROC])
            return -1;
    }
}
