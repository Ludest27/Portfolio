#include "type.h"

extern PROC *running;
extern PROC *readyQueue;

typedef struct semaphore {
    int value;
    PROC *queue;
} SEMAPHORE;

int P(SEMAPHORE *s) {
    s->value--;
    if (s->value < 0) {
        running->status = BLOCK;
        enqueue(&s->queue, running);
        tswitch();
    }
    return 0;
}

int V(SEMAPHORE *s) {
    s->value++;
    if (s->value <= 0) {
        PROC *p = dequeue(&s->queue);
        p->status = READY;
        enqueue(&readyQueue, p);
    }
    return 0;
}
