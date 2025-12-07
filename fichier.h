#include <stdio.h>
#include <stdlib.h>

typedef struct usines{
    char *identifiant;
    int capacite_max;
    int volume_capte;
    int volume_traite;
} Usines;

typedef struct AVL {
    Usines valeur;
    int hauteur;     // HAUTEUR(droit) - HAUTEUR(gauche)
    struct AVL *gauche;
    struct AVL *droit;
} AVL;


AVL* initAVL(Usines u);
int hauteur(AVL* a);
void mettreAJourHauteur(AVL* a);
int facteurEquilibre(AVL* a);

AVL* rotationGauche(AVL* A);
AVL* rotationDroite(AVL* A);
AVL* doubleRotationDroite(AVL* A);
AVL* doubleRotationGauche(AVL* A);

AVL* insererAVL(AVL* a, Usines u);
Usines* rechercherAVL(AVL* a, char* id);
void afficherAVL(AVL* a);

void freeAVL(AVL* a);


