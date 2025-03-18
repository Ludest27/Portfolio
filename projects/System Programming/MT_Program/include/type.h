#ifndef TYPE_H
#define TYPE_H

#define NPROC 9          // number of PROCs
#define SSIZE 1024       // stack size = 4KB

// PROC status
#define FREE     0
#define READY    1
#define SLEEP    2
#define ZOMBIE   3

typedef struct proc{
    struct proc *next;    // next proc pointer
    int *ksp;            // saved stack pointer
    int pid;             // pid = 0 to NPROC-1
    int ppid;            // parent pid
    int status;          // PROC status
    int priority;        // scheduling priority
    int kstack[SSIZE];   // process stack
    struct proc *child;   // point to first child
    struct proc *sibling; // point to next sibling
	struct proc *parent;
    int event;           // event to sleep on
    int exitCode;        // exit code on termination
} PROC;

#endif
