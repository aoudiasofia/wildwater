#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/wildwater.h"

#define TAILLE_BUFFER 4096 // Taille suffisante pour une ligne longue

// Cette fonction est nécessaire car 'strtok' gère mal les champs vides (ex: ";;")
// Elle copie le contenu de la colonne demandée (index commence à 1) dans 'dest'
void recuperer_colonne(char *ligne, int index_colonne, char *dest, int taille_max)
{
    int col_actuelle = 1;
    char *debut = ligne;
    char *fin = NULL;

    // On parcourt la ligne jusqu'à trouver la bonne colonne
    while (col_actuelle < index_colonne && debut != NULL)
    {
        debut = strchr(debut, ';');
        if (debut != NULL)
        {
            debut++; // On saute le point-virgule
            col_actuelle++;
        }
    }

    // Si on a trouvé le début de la colonne
    if (debut != NULL)
    {
        // On cherche la fin (prochain point-virgule ou fin de ligne)
        fin = strchr(debut, ';');

        int longueur;
        if (fin != NULL)
        {
            longueur = fin - debut;
        }
        else
        {
            // C'est la dernière colonne, on va jusqu'au saut de ligne
            longueur = strcspn(debut, "\r\n");
        }

        // Sécurité pour ne pas dépasser le buffer de destination
        if (longueur >= taille_max)
            longueur = taille_max - 1;

        // Copie et fermeture de la chaîne
        strncpy(dest, debut, longueur);
        dest[longueur] = '\0';
    }
    else
    {
        // Colonne introuvable
        dest[0] = '\0';
    }
}

// Fonction principale

int main(int argc, char *argv[])
{
    // 1. Vérification des arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <fichier_donnees.csv>\n", argv[0]);
        return 1;
    }

    // 2. Ouverture du fichier
    FILE *fichier = fopen(argv[1], "r");
    if (fichier == NULL)
    {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 2;
    }

    printf("Traitement du fichier %s en cours...\n", argv[1]);

    // Initialisation de l'arbre
    NoeudAVL *racine = NULL;
    char buffer[TAILLE_BUFFER];

    // Variables temporaires pour le parsing
    char col2[256]; // Identifiant Amont
    char col3[256]; // Identifiant Aval
    char col4[256]; // Volume / Capacité

    // 3. Lecture ligne par ligne
    while (fgets(buffer, TAILLE_BUFFER, fichier))
    {
        // Extraction des colonnes utiles (2, 3 et 4)
        recuperer_colonne(buffer, 2, col2, 256);
        recuperer_colonne(buffer, 3, col3, 256);
        recuperer_colonne(buffer, 4, col4, 256);

        // --- Logique de filtrage selon le sujet ---

        // CAS A : Définition d'une Usine (Capacité) [cite: 78-82]
        // La colonne 2 contient "Facility", la colonne 3 est vide ou "-"
        if (strstr(col2, "Facility") != NULL && (strcmp(col3, "-") == 0 || strlen(col3) == 0))
        {
            // On convertit la capacité (long long car grand nombre)
            long long capacite = atoll(col4);
            // On insère (ou met à jour) avec 0 en volume source
            racine = insererNoeud(racine, col2, capacite, 0);
        }

        // CAS B : Connexion Source -> Usine (Volume Source) [cite: 69-73]
        // La colonne 3 contient "Facility" (C'est l'usine qui reçoit)
        // ET la colonne 2 NE contient PAS "Facility" (ce n'est pas une usine qui alimente une autre)
        // ET la colonne 4 contient un volume (pas "-")
        else if (strstr(col3, "Facility") != NULL && strstr(col2, "Facility") == NULL && strcmp(col4, "-") != 0)
        {
            // On convertit le volume
            double volume = atof(col4);
            // On insère l'usine (si elle n'existe pas encore) avec capacité 0
            // mais on AJOUTE ce volume au total
            racine = insererNoeud(racine, col3, 0, volume);
        }
    }

    // 4. Fermeture et Affichage
    fclose(fichier);

    printf("\n--- Résultat de l'import (Parcours Infixe) ---\n");
    parcoursInfixe(racine);

    // 5. Nettoyage mémoire obligatoire
    libererAVL(racine);

    return 0;
}