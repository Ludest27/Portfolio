#include <unistd.h>
#include <errno.h>

int _write(int file, char *ptr, int len) {
    return len;
}
int _close(int file) {
    return -1;
}
int _read(int file, char *ptr, int len) {
    return 0;
}
int _lseek(int file, int ptr, int dir) {
    return 0;
}
int _isatty(int file) {
    return 1;
}
void _exit(int status) {
    while (1) {}  // Hang system on exit
}
int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}
int _getpid(void) {
    return 1;
}
