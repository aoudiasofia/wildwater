#!/bin/bash

# Récupération des arguments 
FICHIER="$1"
CMD="$2"      
ARG="$3"     

# 1. Vérification des arguments
if [ -z "$FICHIER" ] || [ -z "$CMD" ]; then
    echo "Usage: $0 <fichier.dat> <commande> [parametre]"
    exit 1
fi

if [ "$CMD" == "help" ]; then
    echo "Aide C-WildWater :"
    echo "  histo max | src | real : Histogrammes simples"
    echo "  histo all              : Histogramme empilé (Bonus)"
    echo "  leaks <ID>             : Calcul fuites"
    exit 0
fi

#  2. Compilation
EXECUTABLE="./c-wildwater"
if [ ! -f "$EXECUTABLE" ]; then
    echo "Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur de compilation."
        exit 2
    fi
fi

# 3. Exécution du programme C aavec mesure du temps
rm -f *.png
echo "Lancement du traitement C..."
START=$(date +%s.%N)

# Utilisation des nouvelles variables
$EXECUTABLE "$FICHIER" "$CMD" "$ARG"
if [ $? -ne 0 ]; then
    echo "Erreur lors de l'exécution du programme C."
    exit 3
fi

END=$(date +%s.%N)
DURATION=$(echo "$END - $START" | bc)
echo "Traitement C terminé en $DURATION sec."

#  4. Traitement Graphique 
if [ "$CMD" == "histo" ]; then
    
    #config
    if [ "$ARG" == "max" ]; then
        INPUT="vol_max.dat"
        TITLE="Capacité Maximale"
        YLABEL="Capacité (m3)"
        COL_KEY=2
    elif [ "$ARG" == "src" ]; then
        INPUT="vol_captation.dat"
        TITLE="Volume Source Capté"
        YLABEL="Volume (m3)"
        COL_KEY=2
    elif [ "$ARG" == "real" ]; then
        INPUT="vol_traitement.dat"
        TITLE="Volume Réellement Traité"
        YLABEL="Volume (m3)"
        COL_KEY=2
    elif [ "$ARG" == "all" ]; then
        INPUT="histo_all.dat"
        TITLE="Données Station"
        YLABEL="Volume (M.m3)"
        COL_KEY=2
    else
        echo "Paramètre '$ARG' inconnu pour histo."
        exit 1
    fi
    # Fin du bloc if/elif/else
   
   if [ -f "$INPUT" ]; then
        echo "Génération des graphiques pour $INPUT..."

        # 1. Isoler l'en-tête (la première ligne)
        head -n 1 "$INPUT" > "header.tmp"

        # 2. Trier le reste du fichier en ordre DÉCROISSANT
        tail -n +2 "$INPUT" | sort -t';' -k$COL_KEY -n -r > "body_desc.tmp"

        # 3. Extraire les données
        head -n 10 "body_desc.tmp" > "top10_raw.tmp"
        tail -n 50 "body_desc.tmp" > "min50_raw.tmp"

        # 4. INVERSER L'ORDRE (tri ASCENDANT) pour l'affichage
        sort -t';' -k$COL_KEY -n "top10_raw.tmp" > "body_top10.tmp"
        sort -t';' -k$COL_KEY -n "min50_raw.tmp" > "body_min50.tmp"

        # 5. Reconstruire les fichiers finaux avec l'en-tête
        cat "header.tmp" "body_top10.tmp" > "top10.dat"
        cat "header.tmp" "body_min50.tmp" > "min50.dat"

        # Nettoyage intermédiaire
        rm "header.tmp" "body_desc.tmp" "top10_raw.tmp" "min50_raw.tmp" "body_top10.tmp" "body_min50.tmp" 2>/dev/null