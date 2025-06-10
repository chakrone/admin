#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Fonctions d'affichage spécialisées
void *afficherDecimal(void *arg) {
    int valeur = *(int *)arg;
    printf("Decimal : %d\n", valeur);
    pthread_exit(NULL);
}

void *afficherHexadecimal(void *arg) {
    int valeur = *(int *)arg;
    printf("Hexadecimal  : 0x%x\n", valeur);
    pthread_exit(NULL);
}

void *afficherOctal(void *arg) {
    int valeur = *(int *)arg;
    printf("Octal        : 0%o\n", valeur);
    pthread_exit(NULL);
}

// Fonction de saisie
void *saisirValeur(void *arg) {
    int *ptr = malloc(sizeof(int));
    
    printf("Bienvenue ! Veuillez entrer un entier : ");
    scanf("%d", ptr);
    
    pthread_exit((void *)ptr);
}

int main() {
    pthread_t id_saisie;
    pthread_t id_dec, id_hex, id_oct;
    void *val_retour;
    
    // Phase de saisie
    pthread_create(&id_saisie, NULL, saisirValeur, NULL);
    pthread_join(id_saisie, &val_retour);
    
    int valeur = *(int *)val_retour;
    
    // Phase d'affichage parallèle
    pthread_create(&id_dec, NULL, afficherDecimal, (void *)&valeur);
    pthread_create(&id_hex, NULL, afficherHexadecimal, (void *)&valeur);
    pthread_create(&id_oct, NULL, afficherOctal, (void *)&valeur);
    
    // Synchronisation des threads d'affichage
    pthread_join(id_dec, NULL);
    pthread_join(id_hex, NULL);
    pthread_join(id_oct, NULL);
    
    free(val_retour);
    return 0;
}