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
    u->volume_traité = 0.0;

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

NoeudAVL *insererNoeud(NoeudAVL *noeud, char *id, long long capacite, double ajoute_volume)
{
    // 1. Insertion normale d'ABR (récursive)
    if (noeud == NULL)
    {
        Usine *nouvelleUsine = creerUsine(id, capacite);
        // Si on insère via une ligne "Source", on a déjà un volume à ajouter
        nouvelleUsine->volume_source = ajoute_volume;
        return creerNoeud(nouvelleUsine);
    }

    // On compare les ID (ordre alphabétique) pour savoir où aller
    int compar = strcmp(id, noeud->usine->id);

    if (compar < 0)
    {
        noeud->gauche = insererNoeud(noeud->gauche, id, capacite, ajoute_volume);
    }
    else if (compar > 0)
    {
        noeud->droite = insererNoeud(noeud->droite, id, capacite, ajoute_volume);
    }
    else
    {
        // CAS EGALITE : L'usine existe déjà ! On met à jour les données.
        // C'est ici qu'on gère le fait que les lignes du CSV soient dans le désordre.

        // Si on a reçu une capacité valide (ligne 'Usine'), on la met à jour
        if (capacite > 0)
        {
            noeud->usine->capacite = capacite;
        }
        // Si on a reçu un volume (ligne 'Source' ou 'Usine->Stockage'), on l'ajoute
        if (ajoute_volume > 0)
        {
            noeud->usine->volume_source += ajoute_volume;
        }

        // On retourne le noeud inchangé (structurellement)
        return noeud;
    }

    // 2. Mise à jour de la hauteur du noeud ancêtre
    noeud->hauteur = 1 + max(hauteur(noeud->gauche), hauteur(noeud->droite));

    // 3. Vérification de l'équilibre
    int balance = facteurEquilibre(noeud);

    // 4. Si déséquilibré, on applique les rotations

    // Cas Gauche-Gauche
    if (balance > 1 && strcmp(id, noeud->gauche->usine->id) < 0)
        return rotationDroite(noeud);

    // Cas Droite-Droite
    if (balance < -1 && strcmp(id, noeud->droite->usine->id) > 0)
        return rotationGauche(noeud);

    // Cas Gauche-Droite
    if (balance > 1 && strcmp(id, noeud->gauche->usine->id) > 0)
    {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }

    // Cas Droite-Gauche
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

    void parcoursInfixe(NoeudAVL * racine)
    {
        if (racine != NULL)
        {
            parcoursInfixe(racine->gauche);
            printf("%s : Capacité %lld - Source %.2f\n",
                   racine->usine->id,
                   racine->usine->capacite,
                   racine->usine->volume_source);
            parcoursInfixe(racine->droite);
        }
    }