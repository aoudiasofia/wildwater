# Projet MEF2-A
# WILDWATER — C‑WildWater 🌊

Traitement de données hydrauliques : génération d'histogrammes et calcul des fuites à partir d'un fichier .dat.

---

## 🤝 Collaborateurs
- [**AOUDIA Sofia**](https://github.com/aoudiasofia)
- [**DELECHENEAU Camille**]()
- [**PHILIPPOT Lucie**]()

---

## 🚀 Fonctionnalités

- Commande `histo` : génère des fichiers de données (vol_max, vol_captation, vol_traitement, histo_all) puis des histogrammes PNG (top10 / bottom50).
- Commande `leaks` : calcule les pertes totales pour une usine donnée et écrit dans `leaks.dat`.
- Script `client.sh` : wrapper pratique qui exécute le binaire C, trie/filtre les .dat et appelle gnuplot.

---

## 📂 Structure du projet (fichiers importants)

- `src/` ou racine :
  - `main.c` (ou `c-wildwater`) — lecture CSV, construction des structures, génération des .dat
  - `client.sh` — script de post‑traitement + génération Gnuplot
  - `Makefile`
- `headers/wildwater.h` — définitions des structures Usine, AVL, Station, prototypes (histo + leaks)
- Fichiers produits :
  - `vol_max.dat`, `vol_captation.dat`, `vol_traitement.dat`, `histo_all.dat`
  - `leaks.dat`
  - PNG générés : `histo_<param>_high.png`, `histo_<param>_low.png`, `histo_all_*.png`

---

## 🛠️ Installation & compilation

### Prérequis
- gcc / clang
- make (optionnel)
- gnuplot (pour générer les PNG)

Installer gnuplot sur macOS (Homebrew) :
```bash
brew install gnuplot
```

### Avec make (recommandé)
```bash
cd /Users/sofia/Desktop/wildwater/wildwater
make
```

### Sans make
```bash
gcc -Wall -o c-wildwater main.c ...           # ajouter les fichiers sources nécessaires
```

Rendre le script exécutable :
```bash
chmod +x client.sh
```

---

## ▶️ Exemples d'utilisation

1) Générer les .dat (exécuter le programme C) :
```bash
./c-wildwater data.dat histo max
```

2) Lancer le wrapper graphique (génère PNG via gnuplot) :
```bash
./client.sh data.dat histo max
open histo_max_high.png
```

3) Calculer les fuites pour une usine :
```bash
./c-wildwater data.dat leaks USINE_ID
# puis vérifier leaks.dat (ou utiliser client.sh si prévu)
```

---

## ⚠️ Notes et bonnes pratiques

- Vérifier que `gnuplot` est installé et accessible dans le `PATH` : `command -v gnuplot`.
- `client.sh` vérifie la présence de gnuplot ; sans gnuplot aucun PNG ne sera produit.
- Les chemins sont sensibles à la casse (macOS / Linux). Vérifier que les fichiers .dat existent après l'exécution du binaire C.
- Utiliser `snprintf`/`strtod`/`strtoll` dans le code pour éviter débordements et erreurs de conversion (déjà pris en compte dans le projet).

---

## 📄 Rapport / Documentation
Ajouter ici le lien vers le rapport de projet ou la documentation si disponible.

---

