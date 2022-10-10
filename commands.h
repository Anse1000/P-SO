//
// Created by pablo on 03/10/2022.
//

#ifndef P0_SO_COMMANDS_H
#define P0_SO_COMMANDS_H

#include "list.h"

void salir(bool *t);
void autores(char* arg);
void pid(char* arg);
void fecha(char* arg);
void carpeta(char* arg);
void infosis();
void ayuda(char* arg);
void hist(char* arg, List *L, int *N);
void comando(char* arg, List *L, int *N, bool *T);
void create(char* arg);
void stat(char* arg);
void list(char* arg);
void delete(char* arg);
void deltree(char* arg);

#endif //P0_SO_COMMANDS_H
