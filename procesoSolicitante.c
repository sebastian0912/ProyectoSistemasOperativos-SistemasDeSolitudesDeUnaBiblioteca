#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include "solicitantes.h"
#include <time.h>
#define MAX_LIMIT 40
#define MAXIMA_LONGITUD_CADENA 1000

int enviarPipeArchivo(char pipe[], int cont, sSolicitante soli[] ){
    char mensaje [MAXIMA_LONGITUD_CADENA];
    int  fd,fd1, pid, creado = 0, res, leer;
    envio datos[cont];

    for(int i=0;i<cont;i++){
        strcpy(datos[i].tipo, soli[i].tipo);
        strcpy(datos[i].nombreLibro, soli[i].nombreLibro);
        strcpy(datos[i].ISBN, soli[i].ISBN);
    }
    mode_t fifo_mode = S_IRUSR | S_IWUSR;
    // Se abre el pipe cuyo nombre se recibe como argumento del main.
    do {
        fd = open(pipe, O_WRONLY);
        if (fd == -1) {
            perror("pipe");
            printf(" Se volvera a intentar despues\n");
            sleep(10);
        } else creado = 1;
    } while (creado == 0);

    char nombre[MAX_LIMIT];
    time(NULL);
    pid = rand() % 1000;
    datos[0].pid = pid;

    // Nombre de un segundo pipe
    sprintf(nombre, "Pipe%d", pid );
    for(int i=0; i<cont; i++){
      strcpy(datos[i].segundopipe, nombre);
    }

    // Se crea un segundo pipe para la comunicacion con el server.
    unlink(nombre);
    if (mkfifo (datos[0].segundopipe, fifo_mode) == -1) {
        perror("Client  mkfifo");
        exit(1);
    }
    // se envia el nombre del pipe al otro proceso.
    for(int i=0;i<cont;i++) {
        write(fd, &datos[i], sizeof(datos[i]));
      sleep(2);
    }
       close(fd);



    // Se abre el segundo pipe
    creado = 0;
    do{
        if ((fd1 = open(datos[0].segundopipe, O_RDONLY)) == -1) {
            perror(" Cliente  Abriendo el segundo pipe. Se volvera a intentar ");
            sleep(5);
        } else creado = 1;
    } while (creado == 0);

    // Se lee un mensaje por el segundo pipe.

    fd1 = open(datos[0].segundopipe, O_RDONLY);

    do {
        leer = read(fd1,mensaje, 11);
        if (leer == -1) {
          perror("proceso lector:");
          exit(1);
        }
        if (leer == 0) {
          break;
        }
        printf("El proceso receptor termina y lee %s \n", mensaje);


    } while (leer > 0);


}

void  imprimir(sSolicitante *emp, int cuantos){
    int i;
    printf("Solicitudes:\n");
    for(i=0; i < cuantos; i++)
        printf("%d %s %s %s \n", i , emp[i].tipo, emp[i].nombreLibro, emp[i].ISBN);
}


int main(int argc, char * argv[]) {
      if(argc < 3){
          printf("Numero de argumentos invalidos \n");
          printf("Ejemplo -----> ./solicitante -p pipenom -i PSB.txt \n");
          exit(1);
      }
      else{
          FILE *recibe = fopen(argv[2], "r");
          int mPrincipal, bandera, posI=0, posJ, flag;
          bool archivo = true;
          char nLibro[MAX_LIMIT];
          char temp;
          char *prueba = "-i";
          char *prueba2 = "-P";
          char salir;
          sSolicitante aux;

          for(int i=0; i<argc; i++){
              if(strcmp(argv[i],prueba)==0){
                  bandera=1;
                  break;
              }
              else {
                  bandera=-1;
                  posI++;
              }
          }
          if(posI+1 == 2){
              posJ = 4;
          }
          else{
              posJ = 2;
          }

          if(bandera == 1){
              printf("%s \n", argv[posI+1]);
              printf("%s \n", argv[posJ]);
              char lin[MAXLIN];
              char  *archi = argv[posI+1];
              int cont=0, i=0, j, aux;
              FILE *fp;
              fp = fopen(archi, "r");
              if(fp == NULL){
                  printf("No se ha podido abrir");
                  exit(1);
              }
              else {
                  printf("Se logro abrir  \n");

                  //Contar las lineas del archivo para asignar memoria
                  while (!feof(fp)){
                      fgets(lin,70,fp);
                      cont++;
                  }
                  sSolicitante soli[cont];
                  printf("Numero de solicitudes: %d \n", cont);
                  //pone el cursor al inicio del archivo
                  rewind(fp);
                  // Arreglo de cadenas: aquí almacenamos todas las palabras
                  char palabras[cont][MAXIMA_LONGITUD_CADENA];
                  // Útil para leer el archivo
                  char buferArchivo[MAXIMA_LONGITUD_CADENA];
                  // Necesitamos este ayudante para saber en qué línea vamos
                  int indice = 0;
                  // Mientras podamos leer una línea del archivo
                  while (fgets(buferArchivo, MAXIMA_LONGITUD_CADENA, fp)){
                      // Remover salto de línea
                      strtok(buferArchivo, "\n");
                      // Copiar la línea a nuestro arreglo, usando el índice
                      memcpy(palabras[indice], buferArchivo, MAXIMA_LONGITUD_CADENA);
                      // Aumentarlo en cada iteración
                      indice++;
                  }
                char cadena[MAXIMA_LONGITUD_CADENA];


                  /*strcpy(cadena, palabras[0]);
                  printf("Cadena: %s\n",cadena);*/
                  int iterador=0;
                  char delimitador[] = ",";
                  char *token;
                  for(int i=0; i<cont;i++) {
                      iterador=0;
                      strcpy(cadena, palabras[i]);
                      token = strtok(cadena, delimitador);
                      if (token != NULL) {
                          while (token != NULL) {
                              if (iterador == 0) {
                                  strcpy(soli[i].tipo, token);
                              } else if (iterador == 1) {
                                  strcpy(soli[i].nombreLibro, token);
                              } else if (iterador == 2) {
                                  strcpy(soli[i].ISBN, token);
                              }
                              // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
                              token = strtok(NULL, delimitador);
                              iterador++;
                          }
                      }
                  }
                  int flag;
                  char pipeTemp[20];
                  strcpy(pipeTemp,argv[posJ]);

                  for (int i = 0; i < cont; i++) {
                      printf("PRUEBA SOLICITUDES \n");
                      printf("%s \t", soli[i].tipo);
                      printf("%s \t", soli[i].nombreLibro);
                      printf("%s \n", soli[i].ISBN);
                  }
                  enviarPipeArchivo(pipeTemp,cont,soli);
              }
          }
          else if(bandera ==-1){
            do{
              int flag;
              char *nombrePipe = argv[2];
              char salir;
              int tam, pid;
              printf("Cuantas peticiones realizara? \n");
              scanf("%i",&tam);
              sSolicitante soli[tam];
              printf("SISTEMAS DE PRESTAMOS DE LIBROS\n");
              for(int i=0; i<tam; i++){
                  printf("Escriba el nombre del libro: ");
                  scanf("%c",&temp); // temp statement to clear buffer
                  scanf("%[^\n]", soli[i].nombreLibro);
                  printf("Escriba el ISBN: ");
                  scanf("%s", soli[i].ISBN);
                  printf("Proceso que desea realizar: ");
                  scanf("%s", soli[i].tipo);
              }
              char pipeTemp[20];
                  strcpy(pipeTemp,argv[posJ]);

                  for (int i = 0; i < tam; i++) {
                      printf("PRUEBA SOLICITUDES \n");
                      printf("%s \t", soli[i].tipo);
                      printf("%s \t", soli[i].nombreLibro);
                      printf("%s \n", soli[i].ISBN);
                  }
                  enviarPipeArchivo(pipeTemp,tam,soli);
              printf("¿Desea realizar otra operacion? escriba s para si o n para no");
              scanf("%c", &salir);
              if(salir !='s'){
                flag=1;
              }
              else{
                flag=-1;
              }
            }while(salir == 's');
      }
  }
}




