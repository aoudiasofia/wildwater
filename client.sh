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
