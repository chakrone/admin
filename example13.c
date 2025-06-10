#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Structure pour le chronométrage
typedef struct {
    struct timeval start_time;
    struct timeval end_time;
    const char* operation_name;
} TimingInfo;

// Variables globales pour les mesures
TimingInfo timing_read = {0};
TimingInfo timing_dec = {0};
TimingInfo timing_hex = {0};
TimingInfo timing_oct = {0};
TimingInfo timing_total = {0};

// Fonctions utilitaires de chronométrage
void start_timer(TimingInfo* timer, const char* name) {
    gettimeofday(&timer->start_time, NULL);
    timer->operation_name = name;
}

void stop_timer(TimingInfo* timer) {
    gettimeofday(&timer->end_time, NULL);
}

void print_elapsed_time(TimingInfo* timer) {
    double elapsed = (timer->end_time.tv_sec - timer->start_time.tv_sec) * 1000.0;
    elapsed += (timer->end_time.tv_usec - timer->start_time.tv_usec) / 1000.0;
    printf("%s: %.3f ms\n", timer->operation_name, elapsed);
}

// Fonctions d'affichage avec chronométrage
void *afficherDecimal(void *arg) {
    start_timer(&timing_dec, "Affichage decimal");
    int valeur = *(int *)arg;
    printf("Decimal : %d\n", valeur);
    stop_timer(&timing_dec);
    pthread_exit(NULL);
}

void *afficherHexadecimal(void *arg) {
    start_timer(&timing_hex, "Affichage hexadecimal");
    int valeur = *(int *)arg;
    printf("Hexadecimal : 0x%x\n", valeur);
    stop_timer(&timing_hex);
    pthread_exit(NULL);
}

void *afficherOctal(void *arg) {
    start_timer(&timing_oct, "Affichage octal");
    int valeur = *(int *)arg;
    printf("Octal: 0%o\n", valeur);
    stop_timer(&timing_oct);
    pthread_exit(NULL);
}

void *saisirValeur(void *arg) {
    start_timer(&timing_read, "Lecture valeur");
    int *ptr = malloc(sizeof(int));
    
    printf("Bienvenue ! Veuillez entrer un entier : ");
    scanf("%d", ptr);
    
    stop_timer(&timing_read);
    pthread_exit((void *)ptr);
}

int main() {
    start_timer(&timing_total, "Temps total execution");
    
    pthread_t id_saisie;
    pthread_t id_dec, id_hex, id_oct;
    void *val_retour;
    
    // Exécution séquentielle des phases
    pthread_create(&id_saisie, NULL, saisirValeur, NULL);
    pthread_join(id_saisie, &val_retour);
    int valeur = *(int *)val_retour;
    
    // Exécution parallèle des affichages
    pthread_create(&id_dec, NULL, afficherDecimal, (void *)&valeur);
    pthread_create(&id_hex, NULL, afficherHexadecimal, (void *)&valeur);
    pthread_create(&id_oct, NULL, afficherOctal, (void *)&valeur);
    
    pthread_join(id_dec, NULL);
    pthread_join(id_hex, NULL);
    pthread_join(id_oct, NULL);
    
    stop_timer(&timing_total);
    
    // Rapport de performance
    printf("\n=== RAPPORT DE PERFORMANCE ===\n");
    print_elapsed_time(&timing_read);
    print_elapsed_time(&timing_dec);
    print_elapsed_time(&timing_hex);
    print_elapsed_time(&timing_oct);
    print_elapsed_time(&timing_total);
    
    free(val_retour);
    return 0;
}