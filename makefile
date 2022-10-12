

all: procesoSolicitante procesoReceptor
clean: rm *.o *.exe procesoSolicitante
clean: rm *.o *.exe procesoReceptor
clean: rm pipenom

procesoReceptor: procesoReceptor.o solicitantes.h
	gcc -o receptor procesoReceptor.o -pthread

procesoReceptor.o: procesoReceptor.c solicitantes.h
	gcc -c procesoReceptor.c 

procesoSolicitante: procesoSolicitante.o solicitantes.h
	gcc -o solicitante procesoSolicitante.o

procesoSolicitante.o: procesoSolicitante.c solicitantes.h
	gcc -c procesoSolicitante.c

