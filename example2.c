#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_DEPENSE 1000
#define CREDIT_AMOUNT 200
#define MAX_THREADS 6

// Structure pour partager les données entre threads
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int solde;
    int depense_totale;
    int stop;
} compte_bancaire;

// Thread de la banque (crédits automatiques)
void* banque(void* arg) {
    compte_bancaire* compte = (compte_bancaire*)arg;
    
    while (!compte->stop) {
        pthread_mutex_lock(&compte->mutex);
        
        // Crédit automatique
        compte->solde += CREDIT_AMOUNT;
        printf("BANQUE: Crédit de %d DH effectué. Nouveau solde: %d DH\n", 
               CREDIT_AMOUNT, compte->solde);
        
        // Notification de tous les threads en attente
        pthread_cond_broadcast(&compte->cond);
        
        pthread_mutex_unlock(&compte->mutex);
        
        // Intervalle entre les crédits
        sleep(3);
    }
    
    printf(" BANQUE: Service terminé.\n");
    return NULL;
}

// Threads de dépenses
void* depense(void* arg) {
    compte_bancaire* compte = (compte_bancaire*)arg;
    int id = rand() % 1000;
    
    while (!compte->stop) {
        pthread_mutex_lock(&compte->mutex);
        
        // Génération du montant de dépense
        int montant = (rand() % 50) + 1;
        
        // Vérification des conditions de dépense
        while (compte->solde < montant || 
               (compte->depense_totale + montant) > MAX_DEPENSE) {
            
            if (compte->stop) {
                pthread_mutex_unlock(&compte->mutex);
                printf( CLIENT %d: Fin des opérations.\n", id);
                return NULL;
            }
            
            printf(" CLIENT %d: Attente - Solde insuffisant (%d DH) "
                   "ou plafond atteint (%d/%d DH)\n", 
                   id, compte->solde, compte->depense_totale, MAX_DEPENSE);
            
            // Mise en attente conditionnelle
            pthread_cond_wait(&compte->cond, &compte->mutex);
        }
        
        // Exécution de la dépense
        compte->solde -= montant;
        compte->depense_totale += montant;
        
        printf(" CLIENT %d: Dépense de %d DH effectuée. "
               "Solde: %d DH. Total dépensé: %d/%d DH\n", 
               id, montant, compte->solde, compte->depense_totale, MAX_DEPENSE);
        
        pthread_mutex_unlock(&compte->mutex);
        
        // Pause aléatoire entre les dépenses
        usleep((rand() % 1000000) + 500000);
    }
    
    printf(" CLIENT %d: Fin des opérations.\n", id);
    return NULL;
}

int main() {
    pthread_t threads[MAX_THREADS];
    compte_bancaire compte = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
        .solde = 0,
        .depense_totale = 0,
        .stop = 0
    };
    
    srand(time(NULL));
    
    printf(" DÉMARRAGE DU SYSTÈME BANCAIRE\n");
    printf("=====================================\n");
    
    // Création des threads
    pthread_create(&threads[0], NULL, banque, &compte);
    for (int i = 1; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, depense, &compte);
    }
    
    // Simulation pendant 20 secondes
    sleep(20);
    
    // Arrêt coordonné du système
    printf("\n ARRÊT DU SYSTÈME DEMANDÉ\n");
    pthread_mutex_lock(&compte.mutex);
    compte.stop = 1;
    pthread_cond_broadcast(&compte.cond);
    pthread_mutex_unlock(&compte.mutex);
    
    // Attente de la terminaison de tous les threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n BILAN FINAL\n");
    printf("=====================================\n");
    printf("Solde final: %d DH\n", compte.solde);
    printf("Total dépensé: %d/%d DH\n", compte.depense_totale, MAX_DEPENSE);
    printf("Système arrêté avec succès.\n");
    
    return 0;
}