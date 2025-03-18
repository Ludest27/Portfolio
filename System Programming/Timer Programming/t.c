#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

extern int tswitch();

#define NPROC     9
#define SSIZE  1024

#define FREE      0
#define READY     1
#define SLEEP     2
#define BLOCK     3
#define PAUSE     4
#define ZOMBIE    5

typedef struct proc {
    struct proc *next;
    int    ksp;
    int    pid;
    int    ppid;
    int    priority;
    int    status;
    int    event;
    int    exitStatus;
    int    joinPid;
    struct proc *joinPtr;
    int    time;
    int    pause;
    int    stack[SSIZE];
} PROC;

typedef struct tqe {
    struct tqe *next;
    PROC *proc;
    int time;          // cumulative time
    void (*action)(PROC *); 
} TQE;

PROC proc[NPROC];
PROC *freeList;
PROC *readyQueue;
PROC *running;
PROC *sleepList;
PROC *pauseList;
TQE *timerQueue;
TQE tqes[NPROC];

int tick = 0;
int sec  = 0;
int min  = 0;
int hour = 0;

void timer_wakeup(PROC *p) {
    p->status = READY;
    enqueue(&readyQueue, p);
    printf("Task %d wake up\n", p->pid);
}

void check_timer_queue() {
    if (!timerQueue) return;

    timerQueue->time--;
    if (timerQueue->time <= 0) {
        TQE *tq = timerQueue;
        timerQueue = timerQueue->next;
        
        if (tq->action)
            tq->action(tq->proc);
        else
            timer_wakeup(tq->proc);
            
        tq->next = 0;
        tq->proc = 0;
        tq->time = 0;
    }
}

void timer_handler(int sig) {
    tick++;
    if (tick == 100) {  // 100 ticks = 1 second (10ms * 100)
        tick = 0;
        sec++;
        if (sec == 60) {
            sec = 0;
            min++;
            if (min == 60) {
                min = 0;
                hour++;
            }
        }
        check_timer_queue();
    }
}

void insert_timer(TQE *new_tq) {
    TQE *tq = timerQueue;
    TQE *prev = 0;
    int cumulative_time = new_tq->time;

    if (!tq || cumulative_time < tq->time) {
        new_tq->next = timerQueue;
        timerQueue = new_tq;
        return;
    }

    while (tq && cumulative_time >= tq->time) {
        cumulative_time -= tq->time;
        prev = tq;
        tq = tq->next;
    }

    new_tq->time = cumulative_time;
    new_tq->next = tq;
    prev->next = new_tq;

    if (tq) {
        tq->time -= cumulative_time;
    }
}

#include "queue.c"
#include "wait.c"

int menu() {
    printf("\n*********** menu ***********\n");
    printf("* create switch exit ps    *\n");
    printf("* pause t: pause for t sec *\n");
    printf("* timer t: timer for t sec *\n");
    printf("****************************\n");
    return 0;
}

int init() {
    int i;
    PROC *p;
    for (i = 0; i < NPROC; i++) {
        p = &proc[i];
        p->pid = i;
        p->priority = 1;
        p->status = FREE;
        p->event = 0;
        p->next = p + 1;
        tqes[i].next = 0;
        tqes[i].proc = 0;
        tqes[i].time = 0;
        tqes[i].action = 0;
    }
    proc[NPROC-1].next = 0;
    freeList = &proc[0];
    readyQueue = 0;
    sleepList = 0;
    pauseList = 0;
    timerQueue = 0;
    
    running = dequeue(&freeList);
    running->status = READY;
    running->priority = 0;
    printList("freeList", freeList);
    printf("init complete: P0 running\n");
    return 0;
}

int do_exit() {
    printf("task %d exit\n", running->pid);
    running->status = FREE;
    running->priority = 0;
    enqueue(&freeList, running);
    printList("freeList", freeList);
    tswitch();
    return 0;
}

void do_timer() {
    printf("Current time = %d:%d:%d\n", hour, min, sec);
}

void set_timer(int seconds) {
    if (seconds <= 0) {
        printf("timer: time must be > 0\n");
        return;
    }
    
    TQE *tq = &tqes[running->pid];
    tq->proc = running;
    tq->time = seconds;
    tq->action = timer_wakeup;
    
    insert_timer(tq);
    
    running->status = SLEEP;
    printf("task %d set timer for %d seconds\n", running->pid, seconds);
    tswitch();
}

void do_pause(int seconds) {
    if (seconds <= 0) {
        printf("pause: time must be > 0\n");
        return;
    }
    
    TQE *tq = &tqes[running->pid];
    tq->proc = running;
    tq->time = seconds;
    tq->action = 0;
    
    insert_timer(tq);
    
    running->status = PAUSE;
    printf("task %d pause for %d seconds\n", running->pid, seconds);
    tswitch();
}

int do_ps() {
    printf("--------- ps ----------\n");
    printList("readyQueue", readyQueue);
    printList("sleepList ", sleepList);
    
    TQE *tq = timerQueue;
    printf("timerQueue = ");
    while(tq) {
        printf("[%d,%d]->", tq->proc->pid, tq->time);
        tq = tq->next;
    }
    printf("NULL\n");

    printf("\nProcess Status:\n");
    for(int i=0; i<NPROC; i++) {
        if (proc[i].status != FREE) {
            printf("P%d: ", i);
            switch(proc[i].status) {
                case FREE:   printf("FREE"); break;
                case READY:  printf("READY"); break;
                case SLEEP:  printf("SLEEP"); break;
                case PAUSE:  printf("PAUSE"); break;
                case ZOMBIE: printf("ZOMBIE"); break;
                default:     printf("UNKNOWN");
            }
            printf("\n");
        }
    }
    printf("---------------------\n");
    return 0;
}

int create(void (*func)(), void *parm) {
    int i;
    PROC *p = dequeue(&freeList);
    if (!p) {
        printf("create failed\n");
        return -1;
    }
    p->ppid = running->pid;
    p->status = READY;
    p->priority = 1;
    
    for (i = 1; i < 12; i++)
        p->stack[SSIZE-i] = 0;
        
    p->stack[SSIZE-1] = (int)parm;
    p->stack[SSIZE-2] = (int)do_exit;
    p->stack[SSIZE-3] = (int)func;
    p->ksp = (int)(&p->stack[SSIZE-12]);
    
    enqueue(&readyQueue, p);
    printf("task %d created a new task %d\n", running->pid, p->pid);
    return p->pid;
}

void task(void *parm) {
    char line[128], cmd[16], *cp;
    int param = 0;
    
    printf("task %d start with parm %d\n", running->pid, (int)parm);
    
    while(1) {
        printf("task %d running\n", running->pid);
        menu();
        printf("enter command : ");
        fgets(line, 128, stdin);
        line[strlen(line)-1] = 0;
        
        cp = line;
        while(*cp == ' ') cp++;
        sscanf(cp, "%s %d", cmd, &param);
        
        if (strcmp(cmd, "create") == 0)
            create((void *)task, 0);
        else if (strcmp(cmd, "switch") == 0)
            tswitch();
        else if (strcmp(cmd, "exit") == 0)
            do_exit();
        else if (strcmp(cmd, "ps") == 0)
            do_ps();
        else if (strcmp(cmd, "time") == 0)
            do_timer();
        else if (strcmp(cmd, "pause") == 0)
            do_pause(param);
        else if (strcmp(cmd, "timer") == 0)
            set_timer(param);
        else
            printf("Invalid command. Try again.\n");
    }
}

int scheduler() {
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    printf("next running = %d\n", running->pid);
    return 0;
}

int main() {
    printf("Welcome to the MT multitasking system\n");
    
    signal(SIGALRM, timer_handler);
    
    struct itimerval timer;
    
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;
    
    setitimer(ITIMER_REAL, &timer, NULL);
    
    init();
    create((void *)task, 0);
    printf("P0 switch to P1\n");
    while(1) {
        if (readyQueue)
            tswitch();
    }
    return 0;
}
