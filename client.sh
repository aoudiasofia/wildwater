#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de gestion
# ===================================================================

DATA_FILE="$1"
COMMAND="$2"
PARAM="$3"

# 1. Vérification des arguments
if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    echo "Usage: $0 <fichier.dat> <commande> [parametre]"
    exit 1
fi

if [ "$COMMAND" == "help" ]; then
    echo "Aide C-WildWater :"
    echo "  histo max | src | real : Histogrammes"
    echo "  histo all              : Bonus (Empilé)"
    echo "  leaks <ID>             : Calcul fuites"
    exit 0
fi

# 2. Compilation
EXECUTABLE="./c-wildwater"
if [ ! -f "$EXECUTABLE" ]; then
    echo "Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        exit 2
    fi
fi

# 3. Exécution du C
rm -f *.png
echo "Lancement du traitement C..."
START=$(date +%s.%N)

$EXECUTABLE "$DATA_FILE" "$COMMAND" "$PARAM"
if [ $? -ne 0 ]; then
    echo "Erreur C."
    exit 3
fi

END=$(date +%s.%N)
DURATION=$(echo "$END - $START" | bc)
echo "Traitement C terminé en $DURATION sec."

# 4. Traitement Graphique
if [ "$COMMAND" == "histo" ]; then
    
    # Configuration selon le paramètre
    if [ "$PARAM" == "max" ]; then
        INPUT="vol_max.dat"; TITLE="Capacité Maximale"; YLABEL="Capacité (m3)"
        COL_KEY=2 # On trie sur la colonne 2
    elif [ "$PARAM" == "src" ]; then
        INPUT="vol_captation.dat"; TITLE="Volume Source Capté"; YLABEL="Volume (m3)"
        COL_KEY=2
    elif [ "$PARAM" == "real" ]; then
        INPUT="vol_traitement.dat"; TITLE="Volume Réellement Traité"; YLABEL="Volume (m3)"
        COL_KEY=2
    elif [ "$PARAM" == "all" ]; then
        INPUT="histo_all.dat"; TITLE="Plant data"; YLABEL="Volume (M.m3)"
        COL_KEY=2 # Pour 'all', la hauteur totale de la barre est la capacité max (Col 2)
    fi

    if [ -f "$INPUT" ]; then
        echo "Traitement Gnuplot pour $INPUT..."

        # 1. Séparer l'en-tête (la première ligne)
        head -n 1 "$INPUT" > "header.tmp"
        
        # 2. Trier le reste du fichier en ordre DÉCROISSANT (Plus grand au plus petit)
        # Cela nous permet de trouver les "Vrais" Top 10
        tail -n +2 "$INPUT" | sort -t';' -k$COL_KEY -n -r > "body_desc.tmp"

        # 3. Extraire les données
        head -n 10 "body_desc.tmp" > "top10_raw.tmp"
        tail -n 50 "body_desc.tmp" > "min50_raw.tmp"

        # 4. INVERSER L'ORDRE pour l'affichage (Plus petit au plus grand) [CORRECTION ICI]
        # On re-trie les fichiers extraits en ordre ASCENDANT (-n sans le -r)
        sort -t';' -k$COL_KEY -n "top10_raw.tmp" > "body_top10.tmp"
        sort -t';' -k$COL_KEY -n "min50_raw.tmp" > "body_min50.tmp"

        # 5. Reconstruire les fichiers finaux avec l'en-tête
        cat "header.tmp" "body_top10.tmp" > "top10.dat"
        cat "header.tmp" "body_min50.tmp" > "min50.dat"

        # Nettoyage intermédiaire
        rm "header.tmp" "body_desc.tmp" "top10_raw.tmp" "min50_raw.tmp" "body_top10.tmp" "body_min50.tmp"

        # --- GÉNÉRATION DU SCRIPT GNUPLOT ---
        cat <<EOF > plot_script.gp
set terminal png size 1200,800
set datafile separator ';'
set style data histograms
set style fill solid border -1
set ylabel "$YLABEL"
set key outside top right

if ("$PARAM" eq "all") {
    # Mode Empilé (Bonus)
    set style histogram rowstacked
    set boxwidth 0.8
    set xtics rotate by -45 scale 0
    
    set output 'histo_all_high.png'
    set title "$TITLE (10 greatest)"
    # 'every ::1' pour sauter l'en-tête
    plot 'top10.dat' every ::1 using 4:xtic(1) title 'Real' lc rgb '#ccccff', \
         '' every ::1 using (\$3-\$4) title 'Loss' lc rgb '#ff9999', \
         '' every ::1 using (\$2-\$3) title 'Unused' lc rgb '#ccffcc'

    set output 'histo_all_low.png'
    set title "$TITLE (50 lowest)"
    set xtics rotate by -90 scale 0 font ',8'
    plot 'min50.dat' every ::1 using 4:xtic(1) title 'Real' lc rgb '#ccccff', \
         '' every ::1 using (\$3-\$4) title 'Loss' lc rgb '#ff9999', \
         '' every ::1 using (\$2-\$3) title 'Unused' lc rgb '#ccffcc'

} else {
    # Mode Standard
    set boxwidth 0.5
    set xtics rotate by -45 scale 0
    
    set output 'histo_${PARAM}_high.png'
    set title "Top 10 : $TITLE"
    plot 'top10.dat' using 2:xtic(1) notitle linecolor rgb '#4b89dc'

    set output 'histo_${PARAM}_low.png'
    set title "Bottom 50 : $TITLE"
    set xtics rotate by -90 scale 0 font ',8'
    set boxwidth 0.8
    plot 'min50.dat' using 2:xtic(1) notitle linecolor rgb '#dc4b4b'
}
EOF

        # Exécution
        gnuplot plot_script.gp
        rm plot_script.gp top10.dat min50.dat 2>/dev/null
        echo "Graphiques générés (Ordre croissant)."
    else
        echo "Erreur: Fichier $INPUT introuvable."
    fi
fi