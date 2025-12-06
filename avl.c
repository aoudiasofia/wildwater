#include <stdio.h>
#include <stdlib.h>
#include "fichier.h"


//Creation d'un noeud AVL

AVL* initAVL(Usines u, int eq) {
    AVL* nouveau = malloc(sizeof(AVL));
    if (nouveau == NULL) {
        exit(1);
    }
    // Copie des données
    nouveau->valeur.identifiant = strdup(u.identifiant);
    nouveau->valeur.capacite_max = u.capacite_max;
    nouveau->valeur.volume_capte = u.volume_capte;
    nouveau->valeur.volume_traite = u.volume_traite;

    nouveau->equilibre = eq;
    nouveau->gauche = NULL;
    nouveau->droit = NULL;
    return nouveau;
}

//Hauteur et equilibre

int hauteur(AVL* a) {
    if (a == NULL) return 0;
    int hg = hauteur(a->gauche);
    int hd = hauteur(a->droit);
    if(hg > hd){
        return hg + 1;
    } else {
        return hd + 1;
    }
}

int facteurEquilibre(AVL* a) {
    if (a == NULL) return 0;
    return hauteur(a->droit) - hauteur(a->gauche);
}

//rotations

//rotation gauche sous arbre A avec son fils droit

AVL* rotationGauche(AVL* A){
   AVL* B = A->droit;
    A->droit = B->gauche;
    B->gauche = A;

    return B;
}
//rotation droite sous arbre A avec son fils gauche

AVL* rotationDroite(AVL* A){
    AVL* B = A->gauche;
    A->gauche = B->droit;
    B->droit = A;
   
    return B;
}

AVL* DoubleRotationDroite(AVL* A){
    A->droit = rotationDroite(A->droit); // mettre à jour le fils droit
    return rotationGauche(A);            // puis rotation gauche sur A
}

AVL* DoubleRotationGauche(AVL* A){
    A->gauche = rotationGauche(A->gauche); // mettre à jour le fils gauche
    return rotationDroite(A);              // puis rotation droite sur A
}

//insertion dans un AVL
AVL* insererAVL(AVL* a, Usines u) {
    if (a == NULL){
        return creerArbre(u, 0);
    } 

    if (strcmp(u.identifiant, a->valeur.identifiant) < 0) {
        a->gauche = insererAVL(a->gauche, u);
    } else if (strcmp(u.identifiant, a->valeur.identifiant) > 0) {
        a->droit = insererAVL(a->droit, u);
    } else {
        // déjà présent → mise à jour des volumes
        a->valeur.volume_capte += u.volume_capte;
        a->valeur.volume_traite += u.volume_traite;
        return a;
    }

    // équilibrage
    int eq = facteurEquilibre(a);
    if (eq < -1) {
        if (strcmp(u.identifiant, a->gauche->valeur.identifiant) < 0)
            return rotationDroite(a);
        else
            return doubleRotationGauche(a);
    }
    if (eq > 1) {
        if (strcmp(u.identifiant, a->droit->valeur.identifiant) > 0)
            return rotationGauche(a);
        else
            return doubleRotationDroite(a);
    }

    return a;
}


//Recherche dans un AVL

Usines* rechercherAVL(AVL* a, const char* id) {
    if (a == NULL) return NULL;
    if (strcmp(id, a->valeur.identifiant) == 0){
       return &a->valeur; 
    } 
    if (strcmp(id, a->valeur.identifiant) < 0){
        return rechercherAVL(a->gauche, id);
    } 
    return rechercherAVL(a->droit, id);
}


//affichage (PARCOURS INFIXE)
void afficherAVL(AVL* a) {
    if (a != NULL) {
        afficherAVL(a->gauche);
        printf("Usine: %s | Capacité: %d | Capté: %d | Traité: %d\n",
               a->valeur.identifiant,
               a->valeur.capacite_max,
               a->valeur.volume_capte,
               a->valeur.volume_traite);
        afficherAVL(a->droit);
    }
}



