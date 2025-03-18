int tsleep(int event) {
    running->event = event;
    running->status = SLEEP;
    enqueue(&sleepList, running);
    tswitch();
    return 1;
}

int wake_up(int event) {
    PROC *temp = 0;
    PROC *p = sleepList;
    while (p) {
        if (p->event == event) {
            p->status = READY;
            p->event = 0;
            temp = p;
            p = p->next;
            enqueue(&readyQueue, temp);
        }
        else {
            p = p->next;
        }
    }
    return 1;
}

int texit(int exitValue) {
    running->exitStatus = exitValue;
    running->status = ZOMBIE;
    wake_up(running->pid);
    tswitch();
    return 0;
}

int join(int targetPid) {
    PROC *p = &proc[targetPid];
    while(p->status != ZOMBIE) {
        running->joinPid = targetPid;
        tsleep(running->pid);
    }
    return p->exitStatus;
}
