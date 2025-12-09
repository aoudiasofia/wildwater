#ifndef H_WILDWATER
#define H_WILDWATER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Usine
{
    char *id;
    long long capacite;
    double volume_source;
    double volume_traite; // Attention à l'accent "é", évité ici pour compatibilité
} Usine;

typedef struct NoeudAVL
{
    Usine *usine;
    int hauteur;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
} NoeudAVL;

// API
Usine *creerUsine(char *id, long long capacite);
void libererUsine(Usine *u);

NoeudAVL *creerNoeud(Usine *data);

// Mise à jour : on ajoute 'ajoute_volume_reel'
NoeudAVL *insererNoeud(NoeudAVL *noeud, char *id, long long capacite, double ajoute_volume_source, double ajoute_volume_reel);

int hauteur(NoeudAVL *n);
int max(int a, int b);
int facteurEquilibre(NoeudAVL *n);
NoeudAVL *rotationDroite(NoeudAVL *y);
NoeudAVL *rotationGauche(NoeudAVL *x);
void libererAVL(NoeudAVL *racine);

// Nouvelle fonction pour l'export fichier (ordre inverse)
void parcoursInverse(NoeudAVL *racine, FILE *flux_sortie, int mode);

#endif