#!/bin/bash

# Récupération des arguments 
FICHIER="$1"
CMD="$2"      
ARG="$3"     

# vérification des arguments
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

#Compilation
EXECUTABLE="./c-wildwater"
if [ ! -f "$EXECUTABLE" ]; then
    echo "Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur de compilation."
        exit 2
    fi
fi

#Exécution du programme C aavec mesure du temps
rm -f *.png
echo "Lancement du traitement C..."
START=$(date +%s.%N)

#utilisation des nouvelles variables
$EXECUTABLE "$FICHIER" "$CMD" "$ARG"
if [ $? -ne 0 ]; then
    echo "Erreur lors de l'exécution du programme C."
    exit 3
fi

END=$(date +%s.%N)
DURATION=$(echo "$END - $START" | bc)
echo "Traitement C terminé en $DURATION sec."

# traitement Graphique 
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

        # Isoler l'en-tête (la première ligne)
        head -n 1 "$INPUT" > "header.tmp" 

        # Trier le reste du fichier en ordre DÉCROISSANT
        tail -n +2 "$INPUT" | sort -t';' -k$COL_KEY -n -r > "body_desc.tmp" #tri décroissant

        # Extraire les données
        head -n 10 "body_desc.tmp" > "top10_raw.tmp"
        tail -n 50 "body_desc.tmp" > "min50_raw.tmp"

        # INVERSER L'ORDRE (tri ASCENDANT) pour l'affichage
        sort -t';' -k$COL_KEY -n "top10_raw.tmp" > "body_top10.tmp" #tri croissant
        sort -t';' -k$COL_KEY -n "min50_raw.tmp" > "body_min50.tmp"

        # Reconstruire les fichiers finaux avec l'en-tête
        cat "header.tmp" "body_top10.tmp" > "top10.dat" 
        cat "header.tmp" "body_min50.tmp" > "min50.dat"

        # Nettoyage intermédiaire
        rm "header.tmp" "body_desc.tmp" "top10_raw.tmp" "min50_raw.tmp" "body_top10.tmp" "body_min50.tmp" 2>/dev/null
        cat <<EOF > plot_script.gp #utilisée pour écrire plusieurs lignes dans un fichier de configuration de manière propre
set terminal png size 1200,800
set datafile separator ';'
set style data histograms
set style fill solid border -1
set ylabel "$YLABEL"
set key outside top right

if ("$ARG" eq "all") {
    # Mode Empilé (Bonus)
    set style histogram rowstacked
    set boxwidth 0.8
    set xtics rotate by -45 scale 0
    
    set output 'histo_all_high.png'
    set title "$TITLE (Top 10)"
    plot 'top10.dat' using 4:xtic(1) title 'Real' lc rgb '#ccccff', \
         '' using (\$3-\$4) title 'Loss' lc rgb '#ff9999', \
         '' using (\$2-\$3) title 'Unused' lc rgb '#ccffcc'

    set output 'histo_all_low.png'
    set title "$TITLE (Bottom 50)"
    set xtics rotate by -90 scale 0 font ',8'
    plot 'min50.dat' using 4:xtic(1) title 'Real' lc rgb '#ccccff', \
         '' using (\$3-\$4) title 'Loss' lc rgb '#ff9999', \
         '' using (\$2-\$3) title 'Unused' lc rgb '#ccffcc'

} else {
    # Mode Standard
    set boxwidth 0.5
    set xtics rotate by -45 scale 0
    
    set output 'histo_${ARG}_high.png'
    set title "Top 10 : $TITLE"
    plot 'top10.dat' using 2:xtic(1) notitle linecolor rgb '#4b89dc'

    set output 'histo_${ARG}_low.png'
    set title "Bottom 50 : $TITLE"
    set xtics rotate by -90 scale 0 font ',8'
    set boxwidth 0.8
    plot 'min50.dat' using 2:xtic(1) notitle linecolor rgb '#dc4b4b'
}
EOF

    # Vérifier que gnuplot est disponible
    if ! command -v gnuplot >/dev/null 2>&1; then
        echo "Erreur: gnuplot introuvable. Installez-le (brew install gnuplot) puis relancez."
        exit 5
    fi  
        # Exécution
        gnuplot plot_script.gp
        rm plot_script.gp top10.dat min50.dat 2>/dev/null
        echo "Graphiques générés (Ordre croissant)."
    else
        echo "Erreur: Fichier $INPUT introuvable. Vérifiez l'exécution du C."
        exit 4
    fi
fi
