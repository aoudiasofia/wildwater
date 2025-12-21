#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../headers/wildwater.h"

#define TAILLE_BUFFER 4096

// Fonction pour parser les colonnes CSV
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
        {
            longueur = fin - debut;
        }
        else
        {
            longueur = strcspn(debut, "\r\n");
        }
        if (longueur >= taille_max)
        {
            longueur = taille_max - 1;
        }
        strncpy(dest, debut, longueur);
        dest[longueur] = '\0';
    }
    else
    {
        dest[0] = '\0';
    }
}

// Vérifie si l'ID correspond à un type de station connu
int est_usine(char *str)
{
    if (strstr(str, "Facility") != NULL)
    {
        return 1;
    }
    if (strstr(str, "Plant") != NULL)
    {
        return 1;
    }
    if (strstr(str, "Unit") != NULL)
    {
        return 1;
    }
    if (strstr(str, "Module") != NULL)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
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
    char *param = argv[3]; // Sert d'option (max/src) ou d'ID pour leaks

    char buffer[TAILLE_BUFFER];
    char col2[256], col3[256], col4[256], col5[256];

    // MODE HISTO : ARBRE AVL DES USINES

    if (strcmp(commande, "histo") == 0)
    {
        NoeudAVL *racine = NULL;

        while (fgets(buffer, TAILLE_BUFFER, fichier))
        {
            recuperer_colonne(buffer, 2, col2, 256);
            recuperer_colonne(buffer, 3, col3, 256);
            recuperer_colonne(buffer, 4, col4, 256);
            recuperer_colonne(buffer, 5, col5, 256);

            // capacité
            if (est_usine(col2) && (strcmp(col3, "-") == 0 || strlen(col3) == 0))
            {
                long long capacite = atoll(col4);
                racine = insererNoeud(racine, col2, capacite, 0, 0);
            }
            // volume entrant
            else if (est_usine(col3) && !est_usine(col2) && strcmp(col4, "-") != 0)
            {
                double vol_source = atof(col4);
                double fuite_pct = (strcmp(col5, "-") != 0) ? atof(col5) : 0.0; // si col5 n'est pas "-" alors fuite = atof(col5) sinon fuite = 0.0.
                double vol_reel = vol_source * (1.0 - (fuite_pct / 100.0));     // On applique la perte captage
                racine = insererNoeud(racine, col3, 0, vol_source, vol_reel);
            }
        }
        fclose(fichier);

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
        {
            sprintf(nom_sortie, "histo_all.dat");
            mode = 4;
        }

        if (mode > 0)
        {
            FILE *f_out = fopen(nom_sortie, "w");
            if (f_out != NULL)
            {
                if (mode == 4)
                {
                    fprintf(f_out, "identifier;max volume(M.m3/year);source volume(M.m3/year);real volume(M.m3/year)\n");
                }
                parcoursInverse(racine, f_out, mode);
                fclose(f_out);
            }
        }
        libererAVL(racine);
    }

    // MODE LEAKS : GRAPHE DE DISTRIBUTION

    else if (strcmp(commande, "leaks") == 0)
    {

        NoeudIndex *index = NULL;
        double volume_initial_usine = 0.0;
        char *target_id = param;
        int usine_trouvee = 0;

        while (fgets(buffer, TAILLE_BUFFER, fichier))
        {
            recuperer_colonne(buffer, 2, col2, 256); // Amont
            recuperer_colonne(buffer, 3, col3, 256); // Aval
            recuperer_colonne(buffer, 4, col4, 256); // Volume
            recuperer_colonne(buffer, 5, col5, 256); // Fuite

            // CAS 1 : Source -> Usine Ciblée (On accumule le volume de départ)
            if (strcmp(col3, target_id) == 0 && !est_usine(col2) && strcmp(col4, "-") != 0)
            {
                double vol = atof(col4);
                double fuite = (strcmp(col5, "-") != 0) ? atof(col5) : 0.0;
                // Le volume qui entre VRAIMENT dans l'usine (après perte captage)
                volume_initial_usine += vol * (1.0 - (fuite / 100.0));
                usine_trouvee = 1;
            }

            // CAS 2 : Connexion Réseau (Amont -> Aval)
            // On ignore les lignes "Définition Usine" (col3 vide) et les lignes "Source -> Usine"
            if (strcmp(col3, "-") != 0 && strlen(col3) > 0 && est_usine(col3) == 0)
            {

                // Recherche ou Création du noeud Amont
                Station *s_amont = rechercherStation(index, col2);
                if (s_amont == NULL)
                {
                    s_amont = creerStation(col2);
                    index = insererIndex(index, col2, s_amont);
                }

                // Recherche ou Création du noeud Aval
                Station *s_aval = rechercherStation(index, col3);
                if (s_aval == NULL)
                {
                    s_aval = creerStation(col3);
                    index = insererIndex(index, col3, s_aval);
                }

                // Création de la liaison avec le % de fuite
                double fuite_troncon = (strcmp(col5, "-") != 0) ? atof(col5) : 0.0; // si col5 n'est pas "-" alors fuite = atof(col5) sinon fuite = 0.0.
                ajouterLiaison(s_amont, s_aval, fuite_troncon);

                // Si on croise l'ID de l'usine dans le graphe comme émetteur, on note qu'elle existe
                if (strcmp(col2, target_id) == 0)
                    usine_trouvee = 1;
            }
        }
        fclose(fichier);

        // --- CALCUL ET ÉCRITURE ---

        FILE *f_leaks = fopen("leaks.dat", "a");
        if (f_leaks == NULL)
        {
            libererGraphe(index);
            return 3;
        }

        // Si le fichier est vide, on met l'entête
        fseek(f_leaks, 0, SEEK_END); // seek end pour vérifier la taille
        if (ftell(f_leaks) == 0)
        {
            fprintf(f_leaks, "identifier;Leak volume (M.m3/year)\n");
        }

        if (usine_trouvee == 0)
        {
            fprintf(f_leaks, "%s;-1\n", target_id);
            printf("Usine %s introuvable ou sans données.\n", target_id);
        }
        else
        {
            Station *start_node = rechercherStation(index, target_id);
            double total_pertes = 0.0;
            if (start_node != NULL)
            {
                total_pertes = calculerFuites(start_node, volume_initial_usine);
            }

            fprintf(f_leaks, "%s;%.3f\n", target_id, total_pertes);
            printf("Usine : %s | Volume Init : %.2f | Pertes Totales : %.2f\n",
                   target_id, volume_initial_usine, total_pertes);
        }

        fclose(f_leaks);
        libererGraphe(index);
    }

    return 0;
}
