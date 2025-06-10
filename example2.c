#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_DEPENSE 1000
#define CREDIT_AMOUNT 200
#define MAX_THREADS 6 // 5 pour les dépenses + 1 pour la banque

// Structure pour partager les données entre threads
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int solde;
    int depense_totale;
    int stop;
} compte_bancaire;

// Fonction pour le thread de la banque (crédit)
void* banque(void* arg) {
    compte_bancaire* compte = (compte_bancaire*)arg;
    
    while (!compte->stop) {
        pthread_mutex_lock(&compte->mutex);
        
        // Créditer le compte de 200 dirhams
        compte->solde += CREDIT_AMOUNT;
        printf("Banque: Compte crédité de %d DH. Nouveau solde: %d DH\n", 
               CREDIT_AMOUNT, compte->solde);
        
        // Réveiller tous les threads en attente
        pthread_cond_broadcast(&compte->cond);
        
        pthread_mutex_unlock(&compte->mutex);
        
        // Attendre un peu avant le prochain crédit
        sleep(3);
    }
    
    printf("Banque: Fin du service.\n");
    return NULL;
}

// Fonction pour les threads de dépenses
void* depense(void* arg) {
    compte_bancaire* compte = (compte_bancaire*)arg;
    int id = rand() % 1000; // Identifiant unique pour le thread
    
    while (!compte->stop) {
        pthread_mutex_lock(&compte->mutex);
        
        // Générer un montant aléatoire entre 1 et 50
        int montant = (rand() % 50) + 1;
        
        // Vérifier si la dépense est possible
        while (compte->solde < montant || 
               (compte->depense_totale + montant) > MAX_DEPENSE) {
            if (compte->stop) {
                pthread_mutex_unlock(&compte->mutex);
                printf("Dépense %d: Fin des opérations.\n", id);
                return NULL;
            }
            
            printf("Dépense %d: Attente - Solde insuffisant (%d DH) ou plafond atteint (%d/%d DH)\n", 
                   id, compte->solde, compte->depense_totale, MAX_DEPENSE);
            
            // Attendre un crédit ou un changement de situation
            pthread_cond_wait(&compte->cond, &compte->mutex);
        }
        
        // Effectuer la dépense
        compte->solde -= montant;
        compte->depense_totale += montant;
        printf("Dépense %d: %d DH dépensés. Solde: %d DH. Total dépensé: %d/%d DH\n", 
               id, montant, compte->solde, compte->depense_totale, MAX_DEPENSE);
        
        pthread_mutex_unlock(&compte->mutex);
        
        // Attendre un peu avant la prochaine dépense
        usleep((rand() % 1000000) + 500000); // Entre 0.5 et 1.5 secondes
    }
    
    printf("Dépense %d: Fin des opérations.\n", id);
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
    
    // Créer le thread de la banque
    pthread_create(&threads[0], NULL, banque, &compte);
    
    // Créer les threads de dépenses
    for (int i = 1; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, depense, &compte);
    }
    
    // Laissez tourner le programme pendant un certain temps (20 secondes)
    sleep(20);
    
    // Demander l'arrêt des threads
    pthread_mutex_lock(&compte.mutex);
    compte.stop = 1;
    pthread_cond_broadcast(&compte.cond);
    pthread_mutex_unlock(&compte.mutex);
    
    // Attendre la fin des threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Programme terminé. Solde final: %d DH. Total dépensé: %d/%d DH\n", 
           compte.solde, compte.depense_totale, MAX_DEPENSE);
    
    return 0;
}