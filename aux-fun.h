//
// Created by pablo on 29/10/2022.
//

#ifndef P0_SO_AUX_FUN_H
#define P0_SO_AUX_FUN_H
#include "commands.h"
struct SEN{
    char *nombre;
    int signal;
}SEN;

bool esnumero(char *s);

int encontrar_archivo(char **a);

char LetraTF(mode_t m);

char *ConvierteModo2(mode_t m);

void insertMemoria(List *M, unsigned long size, void *address, enum types type, ...);

void *ObtenerMemoriaShmget(key_t clave, size_t tam);

void *MapearFichero(char *fichero, int protection, List *M);

void opciones_stat(char **arg, int archivo, unsigned int *aux);

char *imprimir_info_archivo(char *archivo, const unsigned int *opciones, struct stat s, char *buffer);

void opciones_list(char **arg, int archivo, unsigned int *aux);

void encontrar_dir(char *archivo, char aux[30][100], int *last);

void imprimir_info_dir(char *archivo, unsigned int oplist, const unsigned int *opstat);

void borrar_rec(char *file);

char *enum_str(enum types t);

void imprimirmem(List M, enum types t);

void unmalloc(unsigned long s, List *M);

void unshare(key_t key, List *M);

void unmap(char *file, List *M);

void Recursiva(int n);

ssize_t LeerFichero (char *f, void *p, size_t cont);

ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite);

void opciones_memory(char **arg,unsigned int *aux);

void execution(char **args,struct parametros p,int op);

void actualizarSignal(struct process *job);
#endif //P0_SO_AUX_FUN_H
