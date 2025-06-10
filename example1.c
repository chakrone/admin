#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction de saisie exécutée dans un thread séparé
void *function(void* arg) {
    int* ptr = malloc(sizeof(int));
    
    printf("Bienvenue ! Veuillez entrer un entier : ");
    scanf("%d", ptr);
    
    pthread_exit((void *)ptr);
    return NULL;
}

int main() {
    pthread_t id;
    void *val;
    
    // Création du thread de saisie
    pthread_create(&id, NULL, function, NULL);
    
    // Attente et récupération de la valeur
    pthread_join(id, &val);
    
    printf("Le nombre saisi est : %d\n", *(int *)val);
    
    // Libération de la mémoire allouée
    free(val);
    
    return 0;
}