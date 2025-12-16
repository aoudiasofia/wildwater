#include "../headers/wildwater.h"

// GESTION DU GRAPHE

Station *creerStation(char *id)
{
    Station *s = (Station *)malloc(sizeof(Station));
    if (s == NULL)
        exit(EXIT_FAILURE);

    s->id = strdup(id);
    s->liste_enfants = NULL;
    return s;
}

Liaison *ajouterLiaison(Station *parent, Station *enfant, double fuite)
{
    Liaison *l = (Liaison *)malloc(sizeof(Liaison));
    if (l == NULL)
    {
        exit(EXIT_FAILURE);
    }
    l->enfant = enfant;
    l->pourcentage_fuite = fuite;
    l->suivant = parent->liste_enfants;
    parent->liste_enfants = l;
    return l;
}

// gestion des avl de l'index
// fonctions auxiliaires pour l'équilibrage avec le mot "static"
// car elles ne sont utilisées que dans ce fichier
// et ne doivent pas être visibles ailleurs
// (ce sont les memes fonctions que pour l'avl.c)

static int h_index(NoeudIndex *n)
{
    return (n == NULL) ? 0 : n->hauteur;
}

static int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

static int equilibre_index(NoeudIndex *n)
{
    if (n == NULL)
        return 0;
    return h_index(n->gauche) - h_index(n->droite);
}

static NoeudIndex *rotD_index(NoeudIndex *y)
{
    NoeudIndex *x = y->gauche;
    NoeudIndex *T2 = x->droite;
    x->droite = y;
    y->gauche = T2;
    y->hauteur = max_int(h_index(y->gauche), h_index(y->droite)) + 1;
    x->hauteur = max_int(h_index(x->gauche), h_index(x->droite)) + 1;
    return x;
}

static NoeudIndex *rotG_index(NoeudIndex *x)
{
    NoeudIndex *y = x->droite;
    NoeudIndex *T2 = y->gauche;
    y->gauche = x;
    x->droite = T2;
    x->hauteur = max_int(h_index(x->gauche), h_index(x->droite)) + 1;
    y->hauteur = max_int(h_index(y->gauche), h_index(y->droite)) + 1;
    return y;
}

// Insertion dans l'annuaire
NoeudIndex *insererIndex(NoeudIndex *noeud, char *id, Station *station)
{
    if (noeud == NULL)
    {
        NoeudIndex *n = (NoeudIndex *)malloc(sizeof(NoeudIndex));
        n->id = strdup(id); // On garde une copie de la clé dans l'index
        n->station = station;
        n->gauche = NULL;
        n->droite = NULL;
        n->hauteur = 1;
        return n;
    }

    int cmp = strcmp(id, noeud->id);
    if (cmp < 0)
        noeud->gauche = insererIndex(noeud->gauche, id, station);
    else if (cmp > 0)
        noeud->droite = insererIndex(noeud->droite, id, station);
    else
        return noeud; // Déjà présent, on ne fait rien

    // Équilibrage
    noeud->hauteur = 1 + max_int(h_index(noeud->gauche), h_index(noeud->droite));
    int bal = equilibre_index(noeud);

    if (bal > 1 && strcmp(id, noeud->gauche->id) < 0)
        return rotD_index(noeud);
    if (bal < -1 && strcmp(id, noeud->droite->id) > 0)
        return rotG_index(noeud);
    if (bal > 1 && strcmp(id, noeud->gauche->id) > 0)
    {
        noeud->gauche = rotG_index(noeud->gauche);
        return rotD_index(noeud);
    }
    if (bal < -1 && strcmp(id, noeud->droite->id) < 0)
    {
        noeud->droite = rotD_index(noeud->droite);
        return rotG_index(noeud);
    }

    return noeud;
}

Station *rechercherStation(NoeudIndex *racine, char *id)
{
    if (racine == NULL)
        return NULL;
    int cmp = strcmp(id, racine->id);
    if (cmp == 0)
        return racine->station;
    if (cmp < 0)
        return rechercherStation(racine->gauche, id);
    return rechercherStation(racine->droite, id);
}

// calcule fuites à partir d'une station donnée

double calculerFuites(Station *depart, double volume_initial)
{
    /*algorithme Récursif :
     *un volume d'eau arrive dans la station actuelle
     *compte le nombre d'enfants (liaisons sortantes)
     *le volume est divisé équitablement : Vol_Par_Tuyau = Volume / Nb_Enfants
     *pour chaque tuyau :
     * - Calcul Perte = Vol_Par_Tuyau * (Pourcentage / 100)
     * - Volume Arrivée = Vol_Par_Tuyau - Perte
     * - Total Fuites += Perte + Appeler Récursivement(Enfant, Volume Arrivée)
     */
    if (depart == NULL || volume_initial <= 0)
        return 0.0;

    // 1. Compter les enfants
    int nb_enfants = 0;
    Liaison *curr = depart->liste_enfants;
    while (curr != NULL)
    {
        nb_enfants++;
        curr = curr->suivant;
    }
    if (nb_enfants == 0)
        return 0.0; // Fin de ligne (Usager), pas de fuite avale

    // 2. Répartition équitable
    double vol_par_tuyau = volume_initial / nb_enfants;
    double total_fuites = 0.0;

    // 3. Parcours des enfants
    curr = depart->liste_enfants;
    while (curr != NULL)
    {
        double perte_ici = vol_par_tuyau * (curr->pourcentage_fuite / 100.0);
        double vol_restant = vol_par_tuyau - perte_ici;

        // Somme = Fuite locale + Fuites de tout le sous-réseau
        total_fuites += perte_ici + calculerFuites(curr->enfant, vol_restant);

        curr = curr->suivant;
    }

    return total_fuites;
}

void libererGraphe(NoeudIndex *indexRacine)
{
    if (indexRacine != NULL)
    {
        libererGraphe(indexRacine->gauche);
        libererGraphe(indexRacine->droite);
        Station *s = indexRacine->station;
        if (s != NULL)
        {
            Liaison *l = s->liste_enfants;
            while (l != NULL)
            {
                Liaison *tmp = l;
                l = l->suivant;
                free(tmp);
            }
            free(s->id);
            free(s);
        }
        free(indexRacine->id);
        free(indexRacine);
    }
}