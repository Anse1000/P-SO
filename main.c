#include <stdio.h>
#include <string.h>
#include "list.h"
#include "commands.h"

void addCommand(int N, char *C, List *L) {
    item I=malloc(sizeof(struct item));
    I->commandNumber = N;
    C[strlen(C)-1]='\0';
    strcpy(I->commands,C);
    insert(L,I);
    free(I);
}

void imprimirPrompt() {
    printf("->");
}

void leerEntrada(char *linea) {
    fgets(linea, 255, stdin);
}

int procesarEntrada(bool *terminado, char **trozos, List *H, int *N, List *M) {
    struct parametros p;
    if (trozos[0] == NULL) { return 0; }
    else {
        p.arg=trozos;
        p.T=terminado;
        p.N=N;
        p.H=H;
        p.M=M;
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
    char linea[255],aux[255];
    char *trozos[10];
    List historial = createList(0);
    List memoria = createList(1);
    int Number = -1;
    while (!terminado) {
        imprimirPrompt();
        leerEntrada(linea);
        strcpy(aux,linea);
        TrocearCadena(linea, trozos);
        if (trozos[0] != NULL) {
            Number++;
            addCommand(Number, aux, &historial);
        }
        procesarEntrada(&terminado, trozos, &historial, &Number,&memoria);
    }
    deleteList(&historial);
    deleteList(&memoria);
    free(historial);
    free(memoria);
    return 0;
}
