#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

// Nombre de processus fils
#define NUM_CHILDREN 4

// Nom du sémaphore
#define SEM_NAME "/sync_semaphore"

// PID des processus fils
extern pid_t children[NUM_CHILDREN];

// Sémaphore
extern sem_t *sem;

// Gestionnaire de signal pour les processus fils
void child_signal_handler(int sig);

// Gestionnaire de signal pour le processus père
void parent_signal_handler(int sig);

#endif
