# 🌊 WILDWATER — C‑WildWater

**Traitement et analyse de données hydrauliques massives.**
Ce projet permet d'analyser un réseau de distribution d'eau, de générer des histogrammes de consommation et de calculer les pertes (fuites) sur le réseau à partir de fichiers `.dat`.

---

## 🤝 L'Équipe

Projet réalisé par :

- [**AOUDIA Sofia**](https://github.com/aoudiasofia)
- [**DELECHENEAU Camille**](https://github.com/delecheneaucamille)
- [**PHILIPPOT Lucie**](https://github.com/luciephilippot)

---

## 🚀 Fonctionnalités

Le projet se divise en deux modules principaux :

### 1. Module `histo` (Analyse de volumes)

- Génère des statistiques sur les stations (max, captation, traitement, global).
- Utilise **Arbres AVL** pour un tri et un stockage rapide des données.
- Produit automatiquement des graphiques PNG via **Gnuplot** (Top 10 / Bottom 50).

### 2. Module `leaks` (Détection de fuites)

- Reconstitue réseau sous forme de **Graphe orienté**.
- Calcule les pertes totales pour une usine donnée en parcourant récursivement le réseau jusqu'aux clients.
- Exporte les résultats dans le fichier `leaks.dat`.

---

## 📂 Structure du projet

```text
.
├── src/
│   ├── main.c           # Point d'entrée, parsing CSV et logique prncipale
    ├── avl.c
    ├── leaks.c
├── headers/
│   └── wildwater.h      # Prototypes et structures (Station, Liaison, AVL)
├── client.sh            # Script Shell (Wrapper) : lance le C + Gnuplot
├── Makefile             # Automatisation de la compilation
├── c-wildwater_v3.dat      
└── README.md
```

## 🛠️ Installation & Compilation

### Prérequis

- **GCC** ou **Clang** : Compilateur C standard.
- **Gnuplot** : Indispensable pour la génération des graphiques (commande `histo`).
  - *macOS* : `brew install gnuplot`
  - *Linux* : `sudo apt-get install gnuplot`

### Compilation

**Option 1 : Avec Make (Recommandé)**
À la racine du projet, lancez simplement :

```bash
make

### Sans make
```bash
gcc -Wall -o c-wildwater src/main.c
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

1) Lancer le wrapper graphique (génère PNG via gnuplot) :

```bash
./client.sh data.dat histo max
open histo_max_high.png
```

1) Calculer les fuites pour une usine :

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

## 📄 Rapport de projet

  Vous pouvez consulter le rapport de projet en cliquant sur le lien, ci-dessous :

- [📕 Rapport de projet (PDF)](rapport-Wildwater-MEF2-A.pdf)
