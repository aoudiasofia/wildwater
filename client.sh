#!/bin/bash

# 1. Vérification des arguments
# Le premier argument doit être le fichier de données
DATA_FILE="$1"
COMMAND="$2"
PARAM="$3"

if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    echo "Usage: $0 <fichier.dat> <commande> [parametre]"
    echo "Commandes :"
    echo "  histo <max|src|real>"
    echo "  leaks <\"ID Usine\">"
    echo "  help"
    exit 1
fi

if [ "$COMMAND" == "help" ]; then
    echo "Aide du projet C-WildWater"
    echo "./client.sh data.csv histo max  : Histogramme capacité max"
    echo "./client.sh data.csv histo src  : Histogramme volume source"
    echo "./client.sh data.csv leaks ID   : Calcul des fuites pour une usine"
    exit 0
fi

# 2. Vérification de la compilation
EXECUTABLE="./c-wildwater"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Exécutable non trouvé. Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur de compilation. Arrêt."
        exit 2
    fi
fi

# 3. Nettoyage des anciens fichiers (Optionnel mais propre)
rm -f *.dat *.png

# 4. Exécution et Chronométrage
echo "Lancement du traitement C..."
# On capture le temps de début (en secondes + nanosecondes)
START=$(date +%s.%N)

# Appel du programme C
# Note : Pour l'instant le C ne gère pas encore les arguments 'max', 'src', etc.
# On lui passe juste le fichier pour vérifier qu'il tourne.
$EXECUTABLE "$DATA_FILE" "$COMMAND" "$PARAM"
CODE_RETOUR=$?

END=$(date +%s.%N)

# 5. Gestion des erreurs du C
if [ $CODE_RETOUR -ne 0 ]; then
    echo "Le programme C a rencontré une erreur (Code: $CODE_RETOUR)."
    exit 3
fi

# 6. Affichage du temps d'exécution
# Calcul de la différence avec bc pour gérer les flottants
DURATION=$(echo "$END - $START" | bc)
echo "Traitement terminé en $DURATION secondes."

# Ici, nous ajouterons plus tard l'appel à Gnuplot pour générer les images.