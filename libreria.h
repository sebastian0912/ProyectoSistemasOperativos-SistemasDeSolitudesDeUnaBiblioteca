#define MAXLIB 100
#define MAXLIN 80
#define MAXNOMBRE1 40
#define MAXESTADO 2
#define FECHA 20
#define ISEJ 10
typedef  struct estruct2 {
    int numeroEjemplar;
    char estado;
    char fecha[FECHA];
} sEjemplares;

typedef struct estruct {
    char nombreLibro [MAXNOMBRE1];
    int ISBN;
    int cuantos;
    sEjemplares ejemplares[ISEJ];
} sLibros;
