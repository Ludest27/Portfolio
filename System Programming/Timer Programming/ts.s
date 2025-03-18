.global tswitch, scheduler, running

tswitch:
        pushal
        pushfl
        movl running, %ebx
        movl %esp, 4(%ebx)
        call scheduler
        movl running, %ebx
        movl 4(%ebx), %esp
        popfl
        popal
        ret
