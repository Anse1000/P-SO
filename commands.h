//
// Created by pablo on 03/10/2022.
//

#ifndef P0_SO_COMMANDS_H
#define P0_SO_COMMANDS_H

#include "list.h"
struct parametros{
    char**arg;
    List *L;
    int *N;
    bool *T;
};
struct comando{
    char* nombre;
    int (*funcion)(struct parametros);
    char* ayuda;
};
extern struct comando comandos[20];
int TrocearCadena(char *cadena, char *trozos[]);
int salir(struct parametros);
int autores(struct parametros);
int pid(struct parametros);
int fecha(struct parametros);
int carpeta(struct parametros);
int infosis();
int ayuda(struct parametros);
int hist(struct parametros);
int comando(struct parametros);
int create(struct parametros);
int stat1(struct parametros);
int list(struct parametros);
int delete(struct parametros);
int deltree(struct parametros);

#endif //P0_SO_COMMANDS_H
