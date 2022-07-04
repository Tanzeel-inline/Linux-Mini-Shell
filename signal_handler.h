#ifndef SIGNAL_HANDLER_H_
#define SIGNAL_HANDLER_H_

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>


struct send_pgid
{
        pid_t group_pid;
};
/*This will contain the id of child-processes that we will make during our execution*/
pid_t child;

/*CHILD PROCESS GROUP ID*/
pid_t childpgid;
/*Handle the signal based on signal*/
void signalhandler(int num);

#endif