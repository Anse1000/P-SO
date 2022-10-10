#include <stdio.h>
#include <string.h>
#include "list.h"
#include "commands.h"

void addCommand(int N, char *C, char* A, List *L) {
    item I;
    I.commandNumber = N;
    strcpy(I.command, C);
    strcpy(I.argument,A);
    insert(L, I);
}

int TrocearCadena(char *cadena, char *trozos[]) {
    int i = 1;
    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL)
        return 0;
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

void imprimirPrompt() {
    printf("->");
}

void leerEntrada(char *linea) {
    fgets(linea, 255, stdin);
}

void procesarEntrada(bool *terminado, char **trozos, List *L, int *N) {
    if (trozos[0] == NULL) { return; }
    else {
        if (strcmp(trozos[0], "salir") == 0 || strcmp(trozos[0], "fin") == 0 || strcmp(trozos[0], "bye") == 0) {
            salir(terminado);
        } else if (strcmp(trozos[0], "autores") == 0) {
            autores(trozos[1]);
        } else if (strcmp(trozos[0], "pid") == 0) {
            pid(trozos[1]);
        } else if (strcmp(trozos[0], "carpeta") == 0) {
            carpeta(trozos[1]);
        } else if (strcmp(trozos[0], "fecha") == 0) {
            fecha(trozos[1]);
        } else if (strcmp(trozos[0], "infosis") == 0) {
            infosis();
        } else if (strcmp(trozos[0], "ayuda") == 0) {
            ayuda(trozos[1]);
        } else if (strcmp(trozos[0], "hist") == 0) {
            hist(trozos[1],L,N);
        } else if (strcmp(trozos[0], "comando") == 0) {
            comando(trozos[1],L,N,terminado);
        } else { printf("%s no es un comando del shell\n", trozos[0]); }

    }
}

int main() {
    bool terminado = false;
    char linea[255];
    char *trozos[5];
    List historial = createList();
    int Number = -1;
    while (!terminado) {
        imprimirPrompt();
        Number++;
        leerEntrada(linea);
        TrocearCadena(linea, trozos);
        if (trozos[0] != NULL) {
            addCommand(Number, trozos[0], (trozos[1] != NULL) ? trozos[1] : " ", &historial);
        }
        procesarEntrada(&terminado, trozos, &historial, &Number);
    }
    return 0;
}
