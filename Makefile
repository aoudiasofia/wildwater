# ==========================================
# Makefile pour le projet C-WildWater
# ==========================================

# Compilateur
CC = gcc

# Options de compilation :
# -Wall -Wextra : Active tous les avertissements (très important pour la notation)
# -g : Ajoute les informations de débogage (utile si tu as des erreurs de mémoire)
CFLAGS = -Wall -Wextra -g

# Bibliothèques à lier (nécessaire car wildwater.h inclut math.h)
LIBS = -lm

# Nom de l'exécutable final
EXEC = c-wildwater

# Dossiers
SRCDIR = src
HEADDIR = headers

# Liste automatique des fichiers sources (.c) et objets (.o)
SRC = $(SRCDIR)/main.c $(SRCDIR)/avl.c
OBJ = $(SRC:.c=.o)

# ------------------------------------------
# Règles de construction
# ------------------------------------------

# Règle par défaut (lancée par la commande 'make')
all: $(EXEC)

# Édition de liens : Crée l'exécutable final à partir des .o
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

# Compilation : Transforme chaque .c en .o
# $< désigne le fichier source, $@ désigne la cible
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

# ------------------------------------------
# Règles de nettoyage
# ------------------------------------------

# Supprime les fichiers temporaires (.o) - Demandé par le sujet
clean:
	rm -f $(SRCDIR)/*.o

# Supprime tout (fichiers .o ET l'exécutable)
mrproper: clean
	rm -f $(EXEC)