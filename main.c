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

int procesarEntrada(bool *terminado, char **trozos, List *L, int *N) {
    struct parametros p;
    if (trozos[0] == NULL) { return 0; }
    else {
        p.arg=trozos;
        p.T=terminado;
        p.N=N;
        p.L=L;
        for(int i=0;comandos[i].nombre!=NULL;i++){
            if(strcmp(comandos[i].nombre,trozos[0])==0){
                return comandos[i].funcion(p);
            }
        }
        return 0;
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
