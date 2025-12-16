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
        if (fin != NULL){
            longueur = fin - debut;
        }
        else {
            longueur = strcspn(debut, "\r\n");
        }
        if (longueur >= taille_max){
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
    if(strstr(str, "Facility") != NULL){
        return 1;
    } 
    if(strstr(str, "Plant") != NULL){
        return 1;
    }
    if(strstr(str, "Unit") != NULL){
        return 1;
    }
    if( strstr(str, "Module") != NULL){
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
                double vol_reel = vol_source * (1.0 - (fuite_pct / 100.0)); // On applique la perte captage
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
                if (mode == 4){
                    fprintf(f_out, "identifier;max volume(M.m3/year);source volume(M.m3/year);real volume(M.m3/year)\n");
                } 
                parcoursInverse(racine, f_out, mode);
                fclose(f_out);
            }
        }
        libererAVL(racine);
    }


   
}

    
    