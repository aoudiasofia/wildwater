#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/wildwater.h"

#define TAILLE_BUFFER 4096

void recuperer_colonne(char *ligne, int index_colonne, char *dest, int taille_max)
{
    int col_actuelle = 1;
    char *debut = ligne;
    char *fin = NULL;
    while (col_actuelle < index_colonne && debut != NULL)
    {
        debut = strchr(debut, ';');
        if (debut != NULL)
        {
            debut++;
            col_actuelle++;
        }
    }
    if (debut != NULL)
    {
        fin = strchr(debut, ';');
        int longueur;
        if (fin != NULL)
            longueur = fin - debut;
        else
            longueur = strcspn(debut, "\r\n");
        if (longueur >= taille_max)
            longueur = taille_max - 1;
        strncpy(dest, debut, longueur);
        dest[longueur] = '\0';
    }
    else
    {
        dest[0] = '\0';
    }
}

int est_usine(char *str)
{
    if (strstr(str, "Facility") != NULL)
        return 1;
    if (strstr(str, "Plant") != NULL)
        return 1;
    if (strstr(str, "Unit") != NULL)
        return 1;
    if (strstr(str, "Module") != NULL)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    // Le sujet implique des arguments : fichier.dat commande [param]
    // Exemple : ./c-wildwater data.dat histo max
    if (argc < 4) // On attend au moins 3 arguments + le nom du prog
    {
        fprintf(stderr, "Usage: %s <fichier.dat> <commande> <param>\n", argv[0]);
        return 1;
    }

    FILE *fichier = fopen(argv[1], "r");
    if (fichier == NULL)
    {
        fprintf(stderr, "Erreur ouverture fichier %s\n", argv[1]);
        return 2;
    }

    char *commande = argv[2];
    char *param = argv[3];

    NoeudAVL *racine = NULL;
    char buffer[TAILLE_BUFFER];
    char col2[256], col3[256], col4[256], col5[256];

    while (fgets(buffer, TAILLE_BUFFER, fichier))
    {
        recuperer_colonne(buffer, 2, col2, 256);
        recuperer_colonne(buffer, 3, col3, 256);
        recuperer_colonne(buffer, 4, col4, 256);
        recuperer_colonne(buffer, 5, col5, 256); // Fuites

        // 1. Définition Usine
        if (est_usine(col2) && (strcmp(col3, "-") == 0 || strlen(col3) == 0))
        {
            long long capacite = atoll(col4);
            racine = insererNoeud(racine, col2, capacite, 0, 0);
        }

        // 2. Source -> Usine
        else if (est_usine(col3) && !est_usine(col2) && strcmp(col4, "-") != 0)
        {
            double vol_source = atof(col4);
            double fuite_pct = 0.0;

            // Si la col 5 n'est pas vide ou "-", on la parse
            if (strcmp(col5, "-") != 0 && strlen(col5) > 0)
            {
                fuite_pct = atof(col5);
            }

            // Calcul du volume réel (traité) = Source * (1 - fuite%)
            // Attention : Si 3.777 signifie 3.777%, on divise par 100.
            double vol_reel = vol_source * (1.0 - (fuite_pct / 100.0));

            racine = insererNoeud(racine, col3, 0, vol_source, vol_reel);
        }
    }
    fclose(fichier);

    // --- Gestion des commandes ---
    if (strcmp(commande, "histo") == 0)
    {
        char nom_sortie[256];
        int mode = 0;

        if (strcmp(param, "max") == 0)
        {
            sprintf(nom_sortie, "vol_max.dat");
            mode = 1;
        }
        else if (strcmp(param, "src") == 0)
        {
            sprintf(nom_sortie, "vol_captation.dat");
            mode = 2;
        }
        else if (strcmp(param, "real") == 0)
        {
            sprintf(nom_sortie, "vol_traitement.dat");
            mode = 3;
        }
        else if (strcmp(param, "all") == 0)
        { // AJOUT DU BONUS
            sprintf(nom_sortie, "histo_all.dat");
            mode = 4;
        }

        if (mode > 0)
        {
            FILE *f_out = fopen(nom_sortie, "w");
            if (f_out != NULL)
            {
                // Pour le mode all, on ajoute l'entête CSV comme dans l'exemple du prof
                if (mode == 4)
                {
                    fprintf(f_out, "identifier;max volume(M.m3/year);source volume(M.m3/year);real volume(M.m3/year)\n");
                }
                parcoursInverse(racine, f_out, mode);
                fclose(f_out);
                printf("Fichier généré : %s\n", nom_sortie);
            }
        }
    }

    libererAVL(racine);
    return 0;
}