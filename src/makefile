# all: escalonador

# escalonador:
# 	gcc escalonador.c -o escalonador

# clean:
# 	rm -rf *o escalonador

# all: escalonador

# escalonador:
# 	gcc escalonador.c -o escalonador

# clean:
# 	rm -rf *o escalonador

IDIR 	= ../include
CC 		= gcc
CFLAGS 	= -std=c11 -Wall -I $(IDIR)
EXEC	= main

ODIR	= ../obj
LDIR 	= ../lib

LIBS	= 

_DEPSESC = escalonador.h
DEPSESC = $(patsubst %,$(IDIR)/%,$(_DEPSESC))

_OBJESC = escalonador.o 
OBJESC = $(patsubst %,$(ODIR)/%,$(_OBJESC))

_DEPSEXEC = executa_postergado.h
DEPSEXEC = $(patsubst %,$(IDIR)/%,$(_DEPSEXEC))

_OBJEXEC = executa_postergado.o 
OBJEXEC = $(patsubst %,$(ODIR)/%,$(_OBJEXEC))

$(ODIR)/%.o: %.c $(DEPSESC)
	$(CC) -c -o $@ $< $(CFLAGS)

escalonador: $(OBJESC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

executa_postergado: $(OBJEXEC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 