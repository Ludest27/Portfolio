int enqueue(PROC **queue, PROC *p) {
    PROC *q = *queue;
    if (q == 0 || p->priority > q->priority) {
        *queue = p;
        p->next = q;
        return 1;
    }
    while (q->next && p->priority <= q->next->priority)
        q = q->next;
    p->next = q->next;
    q->next = p;
    return 1;
}

PROC *dequeue(PROC **queue) {
    PROC *p = *queue;
    if (p)
        *queue = p->next;
    return p;
}

void printList(char *name, PROC *p) {
    printf("%s = ", name);
    while(p) {
        printf("[%d %d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL\n");
}
