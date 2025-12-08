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
    double volume_traité;
} Usine;

typedef struct NoeudAVL
{
    Usine *usine;
    int hauteur;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
} NoeudAVL;

Usine *creerUsine(char *id, long long capacite);
void libererUsine(Usine *u);

NoeudAVL *creerNoeud(Usine *data);
NoeudAVL *insererNoeud(NoeudAVL *noeud, char *id, long long capacite, double ajoute_volume);
int hauteur(NoeudAVL *n);
int max(int a, int b);
int facteurEquilibre(NoeudAVL *n);
NoeudAVL *rotationDroite(NoeudAVL *y);
NoeudAVL *rotationGauche(NoeudAVL *x);
void libererAVL(NoeudAVL *racine);
void parcoursInfixe(NoeudAVL *racine);

#endif