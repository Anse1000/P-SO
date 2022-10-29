//
// Created by pablo on 03/10/2022.
//

#ifndef P0_SO_COMMANDS_H
#define P0_SO_COMMANDS_H

#include "list.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdarg.h>

struct parametros{
    char**arg;
    List *H;
    int *N;
    bool *T;
    List *M;
};
struct comando{
    char* nombre;
    int (*funcion)(struct parametros);
    char* ayuda;
};
extern struct comando comandos[30];
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
int allocate(struct parametros);
int deallocate(struct parametros);
int io(struct parametros);
int memdump(struct parametros);
int memory(struct parametros);
int recurse(struct parametros);
int memfill(struct parametros);

#endif //P0_SO_COMMANDS_H
