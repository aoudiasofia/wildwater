#include <stdio.h>
#include <stdlib.h>

typedef struct usines{
    int identifiant;
    int capacite_max;
    int volume_capte;
    int volume_traite;
} Usines;

typedef struct AVL {
    Usines valeur;
    int equilibre;     // HAUTEUR(droit) - HAUTEUR(gauche)
    struct AVL *gauche;
    struct AVL *droit;
} AVL;



