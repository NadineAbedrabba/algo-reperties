#include "common.h"
#include <stdio.h>
#include <stdlib.h>

// Déclaration de la variable globale sem
sem_t *sem;

// Gestionnaire de signal pour les processus fils
void child_signal_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Processus fils %d a reçu SIGUSR1\n", getpid());
        // Attendre que tous les fils soient prêts
        sem_wait(sem);
        sem_post(sem);

        // Simuler une tâche complexe
        printf("Processus fils %d commence sa tâche\n", getpid());
        sleep(2);  // Simuler le temps de la tâche
        printf("Processus fils %d a terminé sa tâche\n", getpid());

        // Envoyer un signal de confirmation au père
        printf("Processus fils %d envoie SIGUSR2 au père\n", getpid());
        kill(getppid(), SIGUSR2);
    }
}

int main() {
    // Ouvrir le sémaphore
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Installer le gestionnaire de signal
    struct sigaction sa;
    sa.sa_handler = child_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Attendre un signal
    while (1) {
        pause();  // Attendre un signal
    }

    // Fermer le sémaphore
    sem_close(sem);

    return 0;
}
