CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lm
EXEC = c-wildwater
SRCDIR = src
HEADDIR = headers
SRC = $(SRCDIR)/main.c $(SRCDIR)/avl.c $(SRCDIR)/leaks.c
OBJ = $(SRC:.c=.o)

all: $(EXEC)
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f $(SRCDIR)/*.o
mrproper: clean
	rm -f $(EXEC)