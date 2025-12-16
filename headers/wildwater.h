#ifndef H_WILDWATER
#define H_WILDWATER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//histogramme
typedef struct Usine
{
    char *id;
    long long capacite;
    double volume_source;
    double volume_traite;
} Usine;

typedef struct NoeudAVL
{
    Usine *usine;
    int hauteur;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
} NoeudAVL;

// API histo
Usine *creerUsine(char *id, long long capacite);
void libererUsine(Usine *u);
NoeudAVL *creerNoeud(Usine *data);
NoeudAVL *insererNoeud(NoeudAVL *noeud, char *id, long long capacite, double ajoute_volume_source, double ajoute_volume_reel);
int hauteur(NoeudAVL *n);
int max(int a, int b);
int facteurEquilibre(NoeudAVL *n);
NoeudAVL *rotationDroite(NoeudAVL *y);
NoeudAVL *rotationGauche(NoeudAVL *x);
void libererAVL(NoeudAVL *racine);
void parcoursInverse(NoeudAVL *racine, FILE *flux_sortie, int mode);

//leaks
struct Station;
typedef struct Liaison
{
    struct Station *enfant;
    double pourcentage_fuite;
    struct Liaison *suivant;
} Liaison;

typedef struct Station
{
    char *id;
    Liaison *liste_enfants;
} Station;

typedef struct NoeudIndex
{
    char *id;
    Station *station;
    int hauteur;
    struct NoeudIndex *gauche;
    struct NoeudIndex *droite;
} NoeudIndex;

Station *creerStation(char *id);
Liaison *ajouterLiaison(Station *parent, Station *enfant, double fuite);
NoeudIndex *insererIndex(NoeudIndex *noeud, char *id, Station *station);
Station *rechercherStation(NoeudIndex *racine, char *id);
void libererGraphe(NoeudIndex *indexRacine);
double calculerFuites(Station *depart, double volume_initial);

#endif


/*algorithme Récursif :
 *un volume d'eau arrive dans la station actuelle
 *compte le nombre d'enfants (liaisons sortantes)
 *le volume est divisé équitablement : Vol_Par_Tuyau = Volume / Nb_Enfants
 *pour chaque tuyau :
 * - Calcul Perte = Vol_Par_Tuyau * (Pourcentage / 100)
 * - Volume Arrivée = Vol_Par_Tuyau - Perte
 * - Total Fuites += Perte + Appeler Récursivement(Enfant, Volume Arrivée)
 */