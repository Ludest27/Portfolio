#ifndef QUEUE_H
#define QUEUE_H
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
int printList(char *name, PROC *p);
#endif
