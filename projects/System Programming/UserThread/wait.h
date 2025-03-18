#ifndef WAIT_H
#define WAIT_H
int tsleep(int event);
int twakeup(int event);
int texit(int value);
int join(int targetPid, int *status);
#endif
