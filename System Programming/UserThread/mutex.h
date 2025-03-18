#ifndef MUTEX_H
#define MUTEX_H
typedef struct mutex {
    int lock;
    PROC *owner;
    PROC *queue;
} MUTEX;

MUTEX *mutex_create();
void mutex_destroy(MUTEX *mp);
int mutex_lock(MUTEX *mp);
int mutex_unlock(MUTEX *mp);
#endif
