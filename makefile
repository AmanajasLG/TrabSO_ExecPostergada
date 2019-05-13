all: escalonador

escalonador:
	gcc escalonador.c -o escalonador

clean:
	rm -rf *o escalonador
