#include <stdio.h>
#include "type.h"

PROC proc[NPROC];        // NPROC PROCs
PROC *freeList;          // freeList of PROCs
PROC *readyQueue;        // priority queue of READY procs
PROC *running;           // current running proc pointer
PROC *sleepList;         // list of SLEEP procs

// Function declarations
int do_kfork();
int do_switch();
int do_exit();
int do_wait();
int tswitch();
int scheduler();
PROC *dequeue(PROC **queue);
int enqueue(PROC **queue, PROC *p);
int printList(char *name, PROC *p);
void print_tree(PROC *p, int level);
int add_child(PROC *parent, PROC *child);
int ksleep(int event);
int kwakeup(int event);
int kwait(int *status);

int body()
{
    int c;
    printf("proc %d starts from body()\n", running->pid);
    while(1){
        printf("***************************************\n");
        printf("proc %d running: parent=%d\n", running->pid,running->ppid);
        printf("enter a key [f|s|q|p|w] : ");
        c = getchar(); getchar();
        switch(c){
            case 'f': do_kfork(); break;
            case 's': do_switch(); break;
            case 'q': do_exit(); break;
            case 'p': print_tree(&proc[0], 0); break;
            case 'w': do_wait(); break;
        }
    }
}

int add_child(PROC *parent, PROC *child)
{
    if (!parent || !child) {
        printf("add_child error: null pointer\n");
        return -1;
    }

    child->parent = parent;
    child->ppid = parent->pid;
    child->sibling = 0;

    if (parent->child == NULL) {
        parent->child = child;
    }
    else {
        PROC *p = parent->child;
        while (p->sibling) {
            p = p->sibling;
        }
        p->sibling = child;
    }
    return 0;
}

int kfork()
{
    int i;
    PROC *p = dequeue(&freeList);
    if (!p){
        printf("no more proc\n");
        return(-1);
    }
    
    p->status = READY;
    p->priority = 1;     
    p->ppid = running->pid;
    p->parent = running;  
    p->child = 0;        
    p->sibling = 0;      
    p->event = 0;        
    p->exitCode = 0;     
    
    for (i=1; i<10; i++)
        p->kstack[SSIZE - i] = 0;
        
    p->kstack[SSIZE-1] = (int)body;
    p->ksp = &(p->kstack[SSIZE - 9]);
    
    add_child(running, p);
    enqueue(&readyQueue, p);
    return p->pid;
}

int init()
{
    int i;
    PROC *p;
    for (i=0; i<NPROC; i++){
        p = &proc[i];
        p->pid = i;             
        p->status = FREE;
        p->priority = 0;
        p->next = p+1;
        p->child = 0;           
        p->sibling = 0;         
        p->parent = 0;          
        p->event = 0;           
        p->exitCode = 0;        
    }
    proc[NPROC-1].next = 0;
    freeList = &proc[0];        
    readyQueue = 0;             
    sleepList = 0;              

    p = running = dequeue(&freeList);
    p->status = READY;
    p->ppid = 0;                
    p->parent = p;              // P0 is its own parent
    
    printList("freeList", freeList);
    printf("init complete: P0 running\n");
    return 0;
}

void print_tree(PROC *p, int level)
{
    if (!p || level > NPROC) {
        return;
    }
    
    int i;
    for (i = 0; i < level; i++)
        printf("    ");
        
    char *status;
    switch(p->status) {
        case FREE: status = "FREE"; break;
        case READY: status = "READY"; break;
        case SLEEP: status = "SLEEP"; break;
        case ZOMBIE: status = "ZOMBIE"; break;
        default: status = "UNKNOWN"; break;
    }
    
    printf("-%d(%s)", p->pid, status);
    
    if (p->child) {
        printf("->[");
        print_tree(p->child, level+1);
        
        PROC *c = p->child->sibling;
        while (c) {
            print_tree(c, level+1);
            c = c->sibling;
        }
        printf("]");
    }
    printf("\n");
}

int scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    if (running == 0)
        running = &proc[0];
    printf("next running = %d\n", running->pid);
    return 0;
}

int do_exit()
{
    
    if (running->pid <= 1) {
        printf("P0 and P1 cannot exit\n");
        return -1;
    }

    // Mark as zombie but keep the child list
    running->status = ZOMBIE;
    running->exitCode = 123;

    // Wake up parent
    kwakeup((int)running->parent);

    tswitch();
    return 0;
}

int do_kfork()
{
    int child = kfork();
    if (child < 0)
        printf("kfork failed\n");
    else{
        printf("proc %d kforked a child = %d\n", running->pid, child);
        printList("readyQueue", readyQueue);
    }
    return child;
}

int do_switch()
{
    tswitch();
    return 0;
}

int ksleep(int event)
{
    running->event = event;
    running->status = SLEEP;
    running->next = sleepList;
    sleepList = running;
    tswitch();
    return 0;
}

int kwakeup(int event)
{
    PROC *p = sleepList, *q = 0;
    
    while (p) {
        PROC *next = p->next;
        if (p->event == event) {
            if (q)
                q->next = p->next;
            else
                sleepList = p->next;
            p->status = READY;
            enqueue(&readyQueue, p);
        } else {
            q = p;
        }
        p = next;
    }
    return 0;
}

int do_wait()
{
    int pid, status;
    printf("proc %d trying to wait for child\n", running->pid);
    
    pid = kwait(&status);
    
    if (pid > 0)
        printf("proc %d waited for child %d exitCode=%d\n", 
               running->pid, pid, status);
    else
        printf("proc %d wait error: no child\n", running->pid);
    
    return pid;
}

int kwait(int *status)
{
    PROC *p;
    int pid;
    
    // Return error if no children
    if (!running->child)
        return -1;
        
    while(1) {
        // Search for a ZOMBIE child
        p = running->child;
        while(p) {
            if (p->status == ZOMBIE) {
                pid = p->pid;
                *status = p->exitCode;
                
                // Remove zombie from child list
                if (p == running->child) {
                    // First child is zombie
                    running->child = p->sibling;
                } else {
                    // Find zombie in sibling list
                    PROC *q = running->child;
                    while (q->sibling != p)
                        q = q->sibling;
                    q->sibling = p->sibling;
                }
                
                // Move zombie's children to P1 if any exist
                if (p->child) {
                    PROC *childList = p->child;
                    // Update each child's parent to P1
                    PROC *temp = childList;
                    while (temp) {
                        temp->parent = &proc[1];
                        temp->ppid = 1;
                        temp = temp->sibling;
                    }
                    
                    // Add children to P1's child list
                    if (proc[1].child) {
                        // Find end of P1's child list
                        PROC *end = proc[1].child;
                        while (end->sibling)
                            end = end->sibling;
                        end->sibling = childList;
                    } else {
                        proc[1].child = childList;
                    }
                }
                
                // Free the proc
                p->status = FREE;
                p->priority = 0;
                p->parent = 0;
                p->child = 0;
                p->sibling = 0;
                enqueue(&freeList, p);
                
                return pid;
            }
            p = p->sibling;
        }
        // No zombie child found, go to sleep
        ksleep((int)running);
    }
}

int main()
{
    printf("Welcome to the MT Multitasking System\n");
    init();
    kfork();
    while(1){
        if (readyQueue){
            printf("P0: switch process\n");
            tswitch();
        }
    }
}
