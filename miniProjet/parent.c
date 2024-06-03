#include "common.h"

// Définition des variables globales
pid_t children[NUM_CHILDREN];
sem_t *sem;
int confirmations_received = 0; // Compteur pour les confirmations

// Gestionnaire de signal pour le processus père
void parent_signal_handler(int sig) {
    if (sig == SIGUSR2) {
        confirmations_received++;
        printf("Père a reçu une confirmation d'un fils (%d/%d)\n", confirmations_received, NUM_CHILDREN);
    }
}

// Fonction de nettoyage pour s'assurer que le sémaphore est supprimé
void cleanup() {
    sem_close(sem);
    sem_unlink(SEM_NAME);
    printf("Sémaphore nettoyé.\n");
}

int main() {
    struct sigaction sa;
    sigset_t block_mask, empty_mask;

    // Enregistrer la fonction de nettoyage
    atexit(cleanup);

    // Créer et initialiser le sémaphore
    sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Configurer le masque de signaux pour bloquer SIGUSR2
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &block_mask, NULL);

    // Configurer le masque vide pour `sigsuspend`
    sigemptyset(&empty_mask);

    // Installer les gestionnaires de signaux
    sa.sa_handler = parent_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // S'assurer que les appels sont relancés si interrompus

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Créer les processus fils
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if ((children[i] = fork()) == 0) {
            // Code du processus fils
            execl("./child", "./child", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else if (children[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Synchronisation initiale
    sleep(1);  // S'assurer que tous les fils sont créés et prêts

    // Envoyer des signaux aux fils pour commencer leurs tâches
    for (int i = 0; i < NUM_CHILDREN; i++) {
        printf("Père envoie SIGUSR1 au fils %d\n", children[i]);
        kill(children[i], SIGUSR1);
    }

    // Libérer tous les fils en une fois
    for (int i = 0; i < NUM_CHILDREN; i++) {
        sem_post(sem);
    }

    // Attendre que tous les fils terminent leurs tâches et envoient des confirmations
    while (confirmations_received < NUM_CHILDREN) {
        // Utiliser sigsuspend pour attendre les signaux
        sigsuspend(&empty_mask);
    }

    // Nettoyer
    cleanup();

    printf("Tous les processus fils ont terminé.\n");

    return 0;
}
