#ifndef SEM_H
#define SEM_H
typedef struct semaphore {
    int value;
    PROC *queue;
} SEMAPHORE;

int P(SEMAPHORE *s);
int V(SEMAPHORE *s);
#endif
