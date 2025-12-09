#include "../headers/wildwater.h"

// Gestion des Usines
Usine *creerUsine(char *id, long long capacite)
{
    Usine *u = (Usine *)malloc(sizeof(Usine));
    if (u == NULL)
    {
        fprintf(stderr, "Erreur d'allocation pour l'usine.\n");
        exit(EXIT_FAILURE);
    }

    u->id = strdup(id); // strdup alloue de la mémoire et copie la chaine 'id' dedans
    if (u->id == NULL)
    {
        fprintf(stderr, "Erreur d'allocation pour l'ID de l'usine.\n");
        free(u);
        exit(EXIT_FAILURE);
    }

    u->capacite = capacite;
    u->volume_source = 0.0;
    u->volume_traite = 0.0;
    u->volume_source = 0.0;

    return u;
}

void libererUsine(Usine *u)
{
    if (u != NULL)
    {
        if (u->id != NULL)
        {
            free(u->id);
        }
        free(u);
    }
}

// Gestion de l'AVL

int hauteur(NoeudAVL *n)
{
    if (n == NULL)
        return 0;
    return n->hauteur;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int facteurEquilibre(NoeudAVL *n)
{
    if (n == NULL)
        return 0;
    return hauteur(n->gauche) - hauteur(n->droite);
}

NoeudAVL *creerNoeud(Usine *data)
{
    NoeudAVL *n = (NoeudAVL *)malloc(sizeof(NoeudAVL));
    if (n == NULL)
    {
        fprintf(stderr, "Erreur d'allocation pour le noeud AVL.\n");
        exit(EXIT_FAILURE);
    }
    n->usine = data;
    n->gauche = NULL;
    n->droite = NULL;
    n->hauteur = 1; // Un nouveau noeud est toujours une feuille de hauteur 1
    return n;
}

NoeudAVL *rotationDroite(NoeudAVL *y)
{
    NoeudAVL *x = y->gauche;
    NoeudAVL *T2 = x->droite;

    x->droite = y;
    y->gauche = T2;

    y->hauteur = max(hauteur(y->gauche), hauteur(y->droite)) + 1;
    x->hauteur = max(hauteur(x->gauche), hauteur(x->droite)) + 1;

    return x;
}

NoeudAVL *rotationGauche(NoeudAVL *x)
{
    NoeudAVL *y = x->droite;
    NoeudAVL *T2 = y->gauche;

    y->gauche = x;
    x->droite = T2;

    x->hauteur = max(hauteur(x->gauche), hauteur(x->droite)) + 1;
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droite)) + 1;

    return y;
}

NoeudAVL *insererNoeud(NoeudAVL *noeud, char *id, long long capacite, double ajoute_volume_source, double ajoute_volume_reel)
{
    // 1. Insertion normale
    if (noeud == NULL)
    {
        Usine *nouvelleUsine = creerUsine(id, capacite);
        nouvelleUsine->volume_source = ajoute_volume_source;
        nouvelleUsine->volume_traite = ajoute_volume_reel;
        return creerNoeud(nouvelleUsine);
    }

    int compar = strcmp(id, noeud->usine->id);

    if (compar < 0)
        noeud->gauche = insererNoeud(noeud->gauche, id, capacite, ajoute_volume_source, ajoute_volume_reel);
    else if (compar > 0)
        noeud->droite = insererNoeud(noeud->droite, id, capacite, ajoute_volume_source, ajoute_volume_reel);
    else
    {
        // Mise à jour existante
        if (capacite > 0)
            noeud->usine->capacite = capacite;
        if (ajoute_volume_source > 0)
            noeud->usine->volume_source += ajoute_volume_source;
        if (ajoute_volume_reel > 0)
            noeud->usine->volume_traite += ajoute_volume_reel;
        return noeud;
    }

    // 2. Mise à jour hauteur et 3. Equilibrage (inchangé)
    noeud->hauteur = 1 + max(hauteur(noeud->gauche), hauteur(noeud->droite));
    int balance = facteurEquilibre(noeud);

    if (balance > 1 && strcmp(id, noeud->gauche->usine->id) < 0)
        return rotationDroite(noeud);
    if (balance < -1 && strcmp(id, noeud->droite->usine->id) > 0)
        return rotationGauche(noeud);
    if (balance > 1 && strcmp(id, noeud->gauche->usine->id) > 0)
    {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }
    if (balance < -1 && strcmp(id, noeud->droite->usine->id) < 0)
    {
        noeud->droite = rotationDroite(noeud->droite);
        return rotationGauche(noeud);
    }

    return noeud;
}

void libererAVL(NoeudAVL *racine)
{
    if (racine != NULL)
    {
        libererAVL(racine->gauche);
        libererAVL(racine->droite);

        libererUsine(racine->usine);
        free(racine);
    }
}

// mode 1: max, mode 2: src, mode 3: real
void parcoursInverse(NoeudAVL *racine, FILE *flux_sortie, int mode)
{
    if (racine != NULL)
    {
        // Ordre inverse : Droite -> Racine -> Gauche
        parcoursInverse(racine->droite, flux_sortie, mode);

        // Écriture selon le mode
        // Note: Le sujet demande l'unité en M.m3 (Millions de m3) [cite: 568, 571, 576]
        // Les données sont en m3 (ou milliers selon lecture), ajustons si nécessaire.
        // Ici on écrit tel quel, on ajustera l'échelle dans le main ou le gnuplot.

        switch (mode)
        {
        case 1: // max
            fprintf(flux_sortie, "%s;%lld\n", racine->usine->id, racine->usine->capacite);
            break;
        case 2: // src
            fprintf(flux_sortie, "%s;%.2f\n", racine->usine->id, racine->usine->volume_source);
            break;
        case 3: // real (volume traité)
            fprintf(flux_sortie, "%s;%.2f\n", racine->usine->id, racine->usine->volume_traite);
            break;
        }

        parcoursInverse(racine->gauche, flux_sortie, mode);
    }
}