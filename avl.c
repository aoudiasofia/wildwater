#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fichier.h"


//Creation d'un noeud AVL

AVL* initAVL(Usines u) {
    AVL* nouveau = malloc(sizeof(AVL));
    if (nouveau == NULL) {
        exit(1);
    }
    // Copie des données
    nouveau->valeur.identifiant = strdup(u.identifiant); //strdup copie une chaîne en allouant de la mémoire pour la nouvelle copie
    nouveau->valeur.capacite_max = u.capacite_max;
    nouveau->valeur.volume_capte = u.volume_capte;
    nouveau->valeur.volume_traite = u.volume_traite;

    nouveau->hauteur = 1;
    nouveau->gauche = NULL;
    nouveau->droit = NULL;
    return nouveau;
}

//Hauteur et equilibre

int hauteur(AVL* a) {
    if (a == NULL){
        return 0;
    } 
    return a->hauteur;
}

void mettreAJourHauteur(AVL* a) {
    if (a == NULL) {
        return;
    }
    int hg = hauteur(a->gauche);
    int hd = hauteur(a->droit);
    if (hg > hd) {
        a->hauteur = hg + 1;
    } else {
        a->hauteur = hd + 1;
    }
}

int facteurEquilibre(AVL* a) {
    if (a == NULL){
        return 0;
    } 
    return hauteur(a->droit) - hauteur(a->gauche);
}

//rotations

//rotation gauche sous arbre A avec son fils droit

AVL* rotationGauche(AVL* A){
   AVL* B = A->droit;
    A->droit = B->gauche;
    B->gauche = A;
    mettreAJourHauteur(A);
    mettreAJourHauteur(B);
    return B;
}
//rotation droite sous arbre A avec son fils gauche

AVL* rotationDroite(AVL* A){
    AVL* B = A->gauche;
    A->gauche = B->droit;
    B->droit = A;
    mettreAJourHauteur(A);
    mettreAJourHauteur(B);
   
    return B;
}

AVL* doubleRotationDroite(AVL* A){
    A->droit = rotationDroite(A->droit); // mettre à jour le fils droit
    return rotationGauche(A);            // puis rotation gauche sur A
}

AVL* doubleRotationGauche(AVL* A){
    A->gauche = rotationGauche(A->gauche); // mettre à jour le fils gauche
    return rotationDroite(A);              // puis rotation droite sur A
}

//insertion dans un AVL

AVL* insererAVL(AVL* a, Usines u) {
    if (a == NULL) {
        return initAVL(u);
    }

    /* comparaison des identifiants (assume non-NULL) */
    int cmp = strcmp(u.identifiant, a->valeur.identifiant);
    if (cmp < 0) {
        a->gauche = insererAVL(a->gauche, u);
    } else if (cmp > 0) {
        a->droit = insererAVL(a->droit, u);
    } else {
        //déjà présent : on met à jour les volumes et la capacité si besoin 
        a->valeur.volume_capte += u.volume_capte;
        a->valeur.volume_traite += u.volume_traite;
        if (u.capacite_max != 0) { // si une capacité fournie, on peut la remplacer ou la garder selon ta politique
            a->valeur.capacite_max = u.capacite_max;
        }
        // libérer la chaîne passée (si on a strdup dans le créateur de noeud externe) :
          // ici on suppose que l'appelant ne doit pas perdre sa copie ; 
         //  si tu as créé u.identifiant via strdup avant l'appel, tu peux free(u.identifiant) ici.
    
        return a;
    }

    /* mettre à jour la hauteur */
    mettreAJourHauteur(a);

    /* équilibrage */
    int eq = facteurEquilibre(a);

    if (eq < -1) { // arbre trop à gauche (hauteur gauche > droite)
        if (strcmp(u.identifiant, a->gauche->valeur.identifiant) < 0) {
            // cas gauche-gauche
            return rotationDroite(a);
        } else {
            // cas gauche-droite
            return doubleRotationGauche(a);
        }
    }
    if (eq > 1) { // arbre trop à droite
        if (strcmp(u.identifiant, a->droit->valeur.identifiant) > 0) {
            // cas droite-droite
            return rotationGauche(a);
        } else {
            // cas droite-gauche
            return doubleRotationDroite(a);
        }
    }

    return a;
}

//Recherche dans un AVL

Usines* rechercherAVL(AVL* a, char* id) {
    if (a == NULL) {
        return NULL;
    }
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
    if (a == NULL) {
        return;
    }
    afficherAVL(a->gauche);
    printf("Usine: %s | Capacité: %d | Capté: %d | Traité: %d\n",
        a->valeur.identifiant,
        a->valeur.capacite_max,
        a->valeur.volume_capte,
        a->valeur.volume_traite);
    afficherAVL(a->droit);
    
}

void libererAVL(AVL* a) {
    if (a == NULL) {
        return;
    }
    libererAVL(a->gauche);
    libererAVL(a->droit);
    if (a->valeur.identifiant){
    free(a->valeur.identifiant);
    }
    free(a);
}


