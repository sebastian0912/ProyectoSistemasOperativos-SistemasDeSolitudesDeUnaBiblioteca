#define MAXLIB 100
#define MAXLIN 80
#define MAXNOMBRE 70
typedef struct est {
  char tipo [2];
  char nombreLibro [MAXNOMBRE];
  char ISBN [10];
} sSolicitante;

typedef  struct estuConPipe {
    char tipo [2];
    char nombreLibro [MAXNOMBRE];
    char ISBN [10];
    char segundopipe[20];
    int pid;
} envio;

