all: mutex_test sem_test

mutex_test: mutex_test.c ts.s queue.c wait.c mutex.c queue.h wait.h mutex.h
	gcc -m32 mutex_test.c ts.s queue.c wait.c mutex.c -o mutex_test

sem_test: sem_test.c ts.s queue.c wait.c sem.c queue.h wait.h
	gcc -m32 sem_test.c ts.s queue.c wait.c sem.c -o sem_test

clean:
	rm -f mutex_test sem_test *.o
