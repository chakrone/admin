#include <pthread.h>  // Bibliothèque pour la gestion des threads
#include <stdio.h>    // Bibliothèque standard pour les E/S

// Fonction exécutée par le thread secondaire
void *function(void* arg) {
    printf("Thread is Running!\n");
    
    // Conversion et traitement de l'argument
    int* x = (int *)arg;
    printf("Value from main thread: %d\n", *x);
    
    // Modification de la valeur
    *x = 100;
    
    // Terminaison du thread avec retour de valeur
    pthread_exit((void *)x);
    return NULL;
}

int main() {
    pthread_t id;        // Identifiant du thread
    void *retrieve;      // Pointeur pour la valeur de retour
    int val = 10;        // Valeur initiale
    
    // Création du thread
    if(pthread_create(&id, NULL, function, &val) != 0) {
        perror("Erreur lors de la creation du thread");
        return 1;
    }
    
    // Attente de la terminaison du thread
    pthread_join(id, &retrieve);
    
    // Affichage du résultat
    printf("Value retrieved : %d\n", *(int *)retrieve);
    printf("Execution termine avec succes!\n");
    
    return 0;
}