#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de gestion
# ===================================================================

# 1. Vérification des arguments [cite: 1391-1393]
DATA_FILE="$1"
COMMAND="$2"
PARAM="$3"

if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    echo "Usage: $0 <fichier.dat> <commande> [parametre]"
    echo "Exemples :"
    echo "  $0 data.csv histo max"
    echo "  $0 data.csv leaks \"Plant #JA200000I\""
    exit 1
fi

if [ "$COMMAND" == "help" ]; then
    echo "Aide du projet C-WildWater"
    echo "histo max  : Histogramme capacité max"
    echo "histo src  : Histogramme volume source"
    echo "histo real : Histogramme volume traité"
    echo "leaks ID   : Calcul des fuites pour une usine"
    exit 0
fi

# 2. Vérification / Compilation [cite: 1413]
EXECUTABLE="./c-wildwater"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Compiling..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur de compilation."
        exit 2
    fi
fi

# Nettoyage des graphiques précédents
rm -f *.png

# 3. Exécution du C et Chronométrage [cite: 1416]
echo "Lancement du traitement C..."
START=$(date +%s.%N)

$EXECUTABLE "$DATA_FILE" "$COMMAND" "$PARAM"
CODE_RETOUR=$?

END=$(date +%s.%N)

if [ $CODE_RETOUR -ne 0 ]; then
    echo "Erreur lors de l'exécution du programme C."
    exit 3
fi

DURATION=$(echo "$END - $START" | bc)
echo "Traitement C terminé en $DURATION secondes."


# 4. Traitement Graphique (Gnuplot) 
if [ "$COMMAND" == "histo" ]; then
    
    # Définition des noms de fichiers selon le paramètre
    if [ "$PARAM" == "max" ]; then
        INPUT="vol_max.dat"
        TITLE="Capacité Maximale"
        YLABEL="Capacité (m3)"
    elif [ "$PARAM" == "src" ]; then
        INPUT="vol_captation.dat"
        TITLE="Volume Source Capté"
        YLABEL="Volume (m3)"
    elif [ "$PARAM" == "real" ]; then
        INPUT="vol_traitement.dat"
        TITLE="Volume Réellement Traité"
        YLABEL="Volume (m3)"
    fi

    if [ -f "$INPUT" ]; then
        echo "Génération des graphiques pour $INPUT..."

        # Tri numérique décroissant (plus grand au plus petit) sur la 2ème colonne
        sort -t';' -k2 -n -r "$INPUT" > "sorted_$INPUT"

        # Extraction des données
        head -n 10 "sorted_$INPUT" > "top10_$INPUT"
        tail -n 50 "sorted_$INPUT" > "min50_$INPUT"

        # Appel Gnuplot : On génère une image avec 2 graphiques (Multiplot)
        # ou deux images séparées. Ici, je fais 2 images pour la clarté.
        
        # Graphique 1 : TOP 10
        gnuplot -e "
            set terminal png size 1000,600;
            set output 'histo_${PARAM}_high.png';
            set title 'Top 10 : $TITLE';
            set style data histograms;
            set style fill solid;
            set boxwidth 0.5;
            set xtics rotate by -45 scale 0;
            set datafile separator ';';
            set ylabel '$YLABEL';
            plot 'top10_$INPUT' using 2:xtic(1) notitle linecolor rgb '#4b89dc';
        "

        # Graphique 2 : MIN 50 (Attention, c'est beaucoup de barres !)
        gnuplot -e "
            set terminal png size 1600,600;
            set output 'histo_${PARAM}_low.png';
            set title 'Bottom 50 : $TITLE';
            set style data histograms;
            set style fill solid;
            set boxwidth 0.8;
            set xtics rotate by -90 scale 0 font ',8';
            set datafile separator ';';
            set ylabel '$YLABEL';
            plot 'min50_$INPUT' using 2:xtic(1) notitle linecolor rgb '#dc4b4b';
        "

        echo "Graphiques générés : histo_${PARAM}_high.png et histo_${PARAM}_low.png"
        
        # Nettoyage fichiers temporaires
        rm "sorted_$INPUT" "top10_$INPUT" "min50_$INPUT"
    else
        echo "Erreur: Le fichier $INPUT n'a pas été généré par le programme C."
    fi
fi