// Creditos de lectura de bases de datos a la profesora mariela curiel
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include "solicitantes.h"
#include "libreria.h"
#include <semaphore.h>
#include <pthread.h>
#define LIMITE 80
#define MAXLIBROS 100
#define FECHA 20
#define TAMBUF 10

// Declaracion del arreglo de libros
sLibros libros[MAXLIBROS];
int nlibros;
char *archi;

// Variables globales para la implementación del buffer
envio BUFFER[TAMBUF];  // buffer donde se pondrá la informacion
int pcons = 0, pprod = 0;
sem_t s, espacios, elementos;  // semaforos para la implementacion del buffer

void archivoSalidad(char *tipo, int ISBN) {
  
  int j = 0, cont = 0, ndisponibles = 0;
  char *modo = "w";  // w es para sobrescribir, a+ es para añadir al existente
  FILE *archivo = fopen(archi, modo);
  // Si por alguna razón el archivo no fue abierto, salimos inmediatamente
  if (archivo == NULL) {
    printf("Error abriendo archivo %s", archi);
    exit(1);
  } else {
    for (int i = 0; i < nlibros; i++) {
      fprintf(archivo,"%s,%d,%d",libros[i].nombreLibro,libros[i].ISBN,libros[i].cuantos);
      fprintf(archivo, "\n");
      while (j < libros[i].cuantos) {
        fprintf(archivo, "%d,%c,%s", libros[i].ejemplares[j].numeroEjemplar, libros[i].ejemplares[j].estado, libros[i].ejemplares[j].fecha);
        fprintf(archivo, "\n");
        if(libros[i].ejemplares[j].estado == 'D'){
          ndisponibles++;
        }
        j++;
      }
      fprintf(archivo,"Numero de ejemplares disponibles: %d",ndisponibles);
      fprintf(archivo, "\n");
      ndisponibles=0;
      j = 0;
    }
  }
  // Al final, cerramos el archivo
  fclose(archivo);
  puts("Contenido escrito correctamente\n");
}



void fechaSolicitudActualizada(char tipo, char *fechaAntes) {
  time_t rawtime;
  struct tm *info;
  char buffer[FECHA];
  time(&rawtime);
  info = localtime(&rawtime);
  // IMPORTANTE
  //info->tm_mday -= 1;
  strftime(buffer, 80, "%d-%m-%Y", info);
  //printf("Fecha de hoy : %s\n", buffer);
  char *tokenA = strtok(fechaAntes, "/");
  char *tokenB = strtok(buffer, "/");
  int diaA, mesA;
  int diaB, mesB;
  int cont = 0;
  while (tokenA) {
    if (cont == 0) {
      diaA = atoi(tokenA);
    }
    if (cont == 1) {
      mesA = atoi(tokenA);
    }
    tokenA = strtok(NULL, "/");
  }
  cont = 0;
  while (tokenB) {
    if (cont == 0) {
      diaB = atoi(tokenB);
    }
    if (cont == 1) {
      mesB = atoi(tokenB);
    }
    tokenB = strtok(NULL, "/");
  }
  if (strcmp(fechaAntes,buffer)==0 && tipo == 'R') {
    char buffer2[FECHA];
    info->tm_mday += 7;
    strftime(buffer2, 80, "%d-%m-%Y", info);
    strcpy(fechaAntes,buffer2);
  }
  if (strcmp(fechaAntes,buffer)!=0 && tipo == 'P') {
    strcpy(fechaAntes,buffer);
  }
  if (strcmp(fechaAntes,buffer)!=0 && tipo == 'D') {
    strcpy(fechaAntes,buffer);
  }
  if (diaA < diaB && mesA == mesB) {
    strcpy(fechaAntes,buffer);
  }
  if (diaA > diaB && mesA < mesB) {
    strcpy(fechaAntes,buffer);
  }
  if (diaA > diaB && mesA == mesB && tipo == 'D') {
    strcpy(fechaAntes,buffer);
  }
  if (diaA == diaB && mesA == mesB && tipo == 'R') {
    char *vacio="";
    strcpy(fechaAntes, vacio);
    diaA = diaA + 7;
    if (diaA > 30) {
      diaA = diaA - 30;
      mesA++;
    }
    char *cebo;
    sprintf(cebo, "%d", diaA);
    strcat(fechaAntes, cebo);
    strcat(fechaAntes, "-");
    sprintf(cebo, "%d", mesA);
    strcat(fechaAntes, cebo);
    strcat(fechaAntes, "-");
    strcat(fechaAntes, "2021");
  }
}

void solicitudes(int isbn, char *tipo) {
  int i, j = 0, y = 0;
  sLibros *bib = libros;
  for (i = 0; i < nlibros; i++) {
    if (libros[i].ISBN == isbn) {
      printf("Nombre: %s, ISBN: %d, Num Ejemplares: %d \n",
             libros[i].nombreLibro, libros[i].ISBN, libros[i].cuantos);
      break;
    }
  }
  while (j < bib[i].cuantos) {
    printf("%d,%c,%s\n", bib[i].ejemplares[j].numeroEjemplar,
           bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);

    if (strcmp(tipo, "R") == 0) {
      if (bib[i].ejemplares[j].estado != 'D') {
        bib[i].ejemplares[j].estado = 'R';
        fechaSolicitudActualizada(bib[i].ejemplares[j].estado,
                                  bib[i].ejemplares[j].fecha);

        printf("------->%d,%c,%s\n", bib[i].ejemplares[j].numeroEjemplar,
               bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);

        break;
      }
    } else if (strcmp(tipo, "D") == 0) {
      if (bib[i].ejemplares[j].estado != 'D' &&
          bib[i].ejemplares[j].estado != 'R') {
        bib[i].ejemplares[j].estado = 'D';
        fechaSolicitudActualizada(bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);

        printf("------->%d,%c,%s\n", bib[i].ejemplares[j].numeroEjemplar,
               bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);
        break;
      }
    }

    j++;
  }
}

void solicitudesP(int isbn, char *tipo) {
  int i, j = 0;
  sLibros *bib = libros;
  for (i = 0; i < nlibros; i++) {
    if (libros[i].ISBN == isbn) {
      printf("Nombre: %s, ISBN: %d, Num Ejemplares: %d \n",
             bib[i].nombreLibro, bib[i].ISBN, bib[i].cuantos);
      break;
    }
  }
  while (j < libros[i].cuantos) {
    printf("%d,%c,%s\n", bib[i].ejemplares[j].numeroEjemplar, bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);
    if (strcmp(tipo, "P") == 0) {
      if (bib[i].ejemplares[j].estado != 'P' &&  bib[i].ejemplares[j].estado != 'R') {
        libros[i].ejemplares[j].estado = 'P';
        fechaSolicitudActualizada(bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);
        printf("------->%d,%c,%s\n", bib[i].ejemplares[j].numeroEjemplar, bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);
        break;
      }
    }
    j++;
  }
}

void imprimirlibros() {
  int i, j = 0;

  sLibros *bib = libros;

  printf("Estos son los %d  libros almacenados\n", nlibros);

  for (i = 0; i < nlibros; i++) {
    printf("%s,%d,%d\n", bib[i].nombreLibro, bib[i].ISBN, bib[i].cuantos);
    while (j < bib[i].cuantos) {
      printf("%d,%c,%s\n", bib[i].ejemplares[j].numeroEjemplar,
             bib[i].ejemplares[j].estado, bib[i].ejemplares[j].fecha);
      j++;
    }
    j = 0;
  }
}
int verificarEnBd(int isbn){
  imprimirlibros();
  int i;
  sLibros *bib = libros;
  for (i = 0; i < nlibros; i++) {
    if (bib[i].ISBN == isbn) {
      return 1;      
    }
    else{
      return -1;
    }
  }
  return -1;
}
// Funcion para tomar datos del buffer
void *take(envio *e) {
  int a, j = 0, y = 0;
  envio temp, *pe;

  pe = e;  // pe ahora apunta al buffer
  int i = 0;

  for (;;) {
    sem_wait(&elementos);
    sem_wait(&s);
    memcpy(&temp, &pe[pcons], sizeof(envio));
    pe[pcons].pid = 0;  // para indicar que la posición está vacia
    pcons = (pcons + 1) % TAMBUF;
    if (temp.pid == -1) {  // el ultimo elemento
      sem_post(&s);
      sem_post(&espacios);
      break;
    } else {
      // Se imprime el elemento que se toma del buffer
      printf("Thread leyo %s %s %s \n",temp.tipo,temp.nombreLibro,temp.ISBN);
      // imprimirlibros(nlibros,libros);
      solicitudes(atoi(temp.ISBN), temp.tipo);
      // printf("\n");
      archivoSalidad(temp.tipo, atoi(temp.ISBN));
      sem_post(&s);
      sem_post(&espacios);
    }
  }
  printf("thread Receptor termina\n");
  pthread_exit(NULL);
}

// Funcion para colocar elementos del buffer.
void *put(envio *e) {
  sem_wait(&espacios);
  sem_wait(&s);
  if (BUFFER[pprod].pid == 0) memcpy(&BUFFER[pprod], e, sizeof(envio));
  pprod = (pprod + 1) % TAMBUF;
  sem_post(&s);
  sem_post(&elementos);
}

void obtenerInfo(char *linea, char *nom, int *isbn, int *cuantos) {
  char aux[20], aux1[20];
  int libro, num;
  int i = 0;
  char *x;

  x = linea;
  while (linea[i] != ',') {
    nom[i] = linea[i];
    i++;
    x++;
  }
  x++;
  strcpy(aux, x);
  sscanf(aux, "%d,%d", isbn, cuantos);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Numero de argumentos invalidos\n");
    printf("Ejemplo -----> ./receptor -s BD1.txt -p pipenom -f BD.txt \n");
    exit(1);
  } else {
        int fd, fd1, fd2 = -1, pid, n, cuantos, res, posI = 0, posJ = 0,
                     posY = 0, leer, creado=0;
        envio datos;
        char *prueba = "-p";
        char *prueba2 = "-f";
        char *prueba3 = "-s";
        pthread_t thread1;

        for (int i = 0; i < argc; i++) {
          if (strcmp(argv[i], prueba) == 0) {
            break;
          } else {
            posI++;
          }
        }
        for (int j = 0; j < argc; j++) {
          if (strcmp(argv[j], prueba2) == 0) {
            break;
          } else {
            posJ++;
          }
        }
        for (int z = 0; z < argc; z++) {
          if (strcmp(argv[z], prueba3) == 0) {
            break;
          } else {
            posY++;
          }
        }
        char *pipe = argv[posI + 1];
        char *archiBd = argv[posJ + 1];
        char *archiSa = argv[posY + 1];
        archi = argv[posY + 1];
        printf("%s \n", pipe);
        printf("%s \n", archiBd);
        printf("%s \n", archiSa);

        char line[80], nom[60], fecha[FECHA], status;
        FILE *fp;
        int i, edad = 0, j, isbn, indice;

        float salario;

        fp = fopen(archiBd, "r");
        i = 0;

        while (!feof(fp)) {
          fgets(line, LIMITE, fp);
          obtenerInfo(line, nom, &isbn, &cuantos);
          strcpy(libros[i].nombreLibro, nom);
          libros[i].ISBN = isbn;
          libros[i].cuantos = cuantos;
          for (j = 0; j < cuantos; j++) {
            fscanf(fp, "%d,%c,%s\n", &indice, &status, fecha);
            libros[i].ejemplares[j].numeroEjemplar = indice;
            libros[i].ejemplares[j].estado = status;
            strcpy(libros[i].ejemplares[j].fecha, fecha);
          }
          i++;
        }
        // Imprimir estructura de datos
        nlibros = i;
        // imprimirlibros();
        fclose(fp);
        //do{
                mode_t fifo_mode = S_IRUSR | S_IWUSR;

                // inicializacion de los semáforos siguiendo el algoritmo productor,
                // consumidor
                sem_init(&s, 0, 1);
                sem_init(&espacios, 0, TAMBUF);
                sem_init(&elementos, 0, 0);

                // inicializacion del buffer (se coloca en 0 la pid para indicar que la
                // posicion está vacia
                for (i = 0; i < TAMBUF; i++) BUFFER[i].pid = 0;

                // Creación del hilo
                pthread_create(&thread1, NULL, (void *)take, (void *)BUFFER);

                if (mkfifo(pipe, fifo_mode) == -1) {
                  perror("Server mkfifo");
                  exit(1);
                }
                
                fd = open(pipe, O_RDONLY);
                                
                // Creacion del pipe inicial, el que se recibe como argumento del main
                do {
                    leer = read(fd, &datos, sizeof(datos));
                    if (leer == -1) {
                      perror("proceso lector:");
                      exit(1);
                    }
                    if (leer == 0) {
                      break;
                    }
                    
                    printf(" El central leyó  %s %s %s\n", datos.tipo, datos.nombreLibro,datos.ISBN );
                    for (int i = 0; i < nlibros; i++) {
                          if ( libros[i].ISBN == atoi(datos.ISBN)) {
                              printf("Bases de datos: %d, Llega: %s \n", libros[i].ISBN ,datos.ISBN);
                              do {
                                  if ((fd1 = open(datos.segundopipe, O_WRONLY)) == -1) {
                                     perror(" Server Abriendo el segundo pipe ");
                                     printf(" Se volvera a intentar despues\n");
                                     sleep(5); //los unicos sleeps que deben colocar son los que van en los ciclos para abrir los pipes.
                                  } else creado = 1;
                               }  while (creado == 0);
                                // Se escribe un mensaje para el  proceso (nom1)
                               write(fd1, "Confirmado", 11);
                                
                              if ((strcmp(datos.tipo, "P") == 0)) {
                                solicitudesP(atoi(datos.ISBN), datos.tipo);
                              }else {
                                put(&datos);
                              }
                           
                          }
                      
                      else{
                        write(fd1,"Rechazado", 11);
                        
                      }
                      
                    }
                    //segundopipe(datos.segundopipe,flag);
                } while (leer > 0);


                printf("----------------------------------------------\n");

                datos.pid = -1;

                put(&datos);
                
                

                  
                // Se escribe un mensaje para el  proceso (nom1)
                /*for(){

                }*/
               
                
        //}while(fd2==-1);

  }
}
