#include "type.h"
#include <stdlib.h>

extern PROC *running;
extern PROC *readyQueue;

typedef struct mutex {
    int lock;
    PROC *owner;
    PROC *queue;
} MUTEX;

MUTEX *mutex_create() {
    MUTEX *mp = (MUTEX *)malloc(sizeof(MUTEX));
    mp->lock = 0;
    mp->owner = 0;
    mp->queue = 0;
    return mp;
}

void mutex_destroy(MUTEX *mp) {
    free(mp);
}

int mutex_lock(MUTEX *mp) {
    if (mp->lock == 0) {
        mp->lock = 1;
        mp->owner = running;
    }
    else {
        running->status = BLOCK;
        enqueue(&mp->queue, running);
        tswitch();
    }
    return 0;
}

int mutex_unlock(MUTEX *mp) {
    if (mp->lock == 0 || mp->owner != running)
        return -1;
    if (mp->queue == 0) {
        mp->lock = 0;
        mp->owner = 0;
    }
    else {
        PROC *p = dequeue(&mp->queue);
        mp->owner = p;
        p->status = READY;
        enqueue(&readyQueue, p);
    }
    return 0;
}
