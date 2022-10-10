//
// Created by pablo on 03/10/2022.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include "commands.h"
#include "main.h"
#include <sys/stat.h>

bool esnumero(char *s) {
    if (s[0] == 45) {
        for (int i = 1; i < strlen(s); i++) {
            if (s[i] >= 48 && s[i] <= 57) {
                return true;
            }
        }
    }
    return false;
}
void salir(bool *t){
    *t = true;
}
void autores(char *arg){
    if (arg == NULL) {
        printf("Pablo Rey Ansemil: pablo.rey.ansemil@udc.es\n");
        printf("Raul Romero Saavedra: raul.romero.saavedra@udc.es \n");
    } else if (strcmp(arg, "-l") == 0) {
        printf("pablo.rey.ansemil@udc.es\nraul.romero.saavedra@udc.es\n");
    } else if (strcmp(arg, "-n") == 0) {
        printf("Pablo Rey Ansemil\nRaul Romero Saavedra\n");
    } else { return; }
}
void pid(char* arg){
    if (arg == NULL) {
        printf("Pid del shell: %d\n", getpid());
    } else if (strcmp(arg, "-p") == 0) {
        printf("Pid del padre del shell: %d\n", getppid());
    } else { return; }
}
void carpeta(char* arg){
    char aux[80];
    if (arg == NULL) {
        getcwd(aux,80)!=NULL? printf("%s\n",aux): perror("Imposible obtener el directorio.\n");
    } else {
        if(chdir(arg)!=0){ printf("El directorio seleccionado no existe.\n");}
    }
}
void fecha(char* arg){
    time_t t = time(NULL);
    struct tm tiempo = *localtime(&t);

    if (arg == NULL) {
        printf("%d:%d:%d\n", tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);
        printf("%d/%d/%d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900);
    } else if (strcmp(arg, "-d") == 0) {
        printf("%d/%d/%d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900);
    } else if (strcmp(arg, "-h") == 0) {
        printf("%d:%d:%d\n", tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);
    } else { return; }
}
void infosis(){
    struct utsname equipo;
    uname(&equipo);
    printf("%s(%s), OS: %s-%s-%s \n", equipo.nodename, equipo.machine, equipo.sysname, equipo.release,
           equipo.version);
}
void ayuda(char* arg) {
    if (arg == NULL) {
        printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n"
               "fin salir bye fecha pid autores hist comando carpeta infosis ayuda\n");
    } else if (strcmp(arg, "autores") == 0) {
        printf("autores [-n|-l] Muestra los nombres y logins de los autores\n");
    } else if (strcmp(arg, "pid") == 0) {
        printf("pid [-p]        Muestra el pid del shell o de su proceso padre\n");
    } else if (strcmp(arg, "carpeta") == 0) {
        printf("carpeta [dir]   Cambia (o muestra) el directorio actual del shell\n");
    } else if (strcmp(arg, "fecha") == 0) {
        printf("fecha [-d|-h]    Muestra la fecha y o la hora actual\n");
    } else if (strcmp(arg, "hist") == 0) {
        printf("hist [-c|-N]    Muestra el historico de comandos, con -c lo borra\n");
    } else if (strcmp(arg, "comando") == 0) {
        printf("comando [-N]    Repite el comando N (del historico)\n");
    } else if (strcmp(arg, "infosis") == 0) {
        printf("infosis         Muestra informacion de la maquina donde corre el shell\n");
    } else if (strcmp(arg, "fin") == 0 || strcmp(arg, "salir") == 0 || strcmp(arg, "bye") == 0) {
        printf("%s   Termina la ejecucion del shell\n", arg);
    } else if (strcmp(arg, "ayuda") == 0) {
        printf("ayuda [cmd]     Muestra ayuda sobre los comandos\n");
    } else if (strcmp(arg, "create") == 0) {
        printf("create [-f] [name]      Crea un directorio o un fichero (-f)\n");
    } else if (strcmp(arg, "stat") == 0) {
        printf("stat [-long][-link][-acc] name1 name2 .. lista ficheros;\n"
               "\t-long: listado largo\n"
               "\t-acc: acesstime\n"
               "\t-link: si es enlace simbolico, el path contenido\n");
    } else if (strcmp(arg, "list") == 0) {
        printf("list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 .. lista contenidos de directorios\n"
               "\t-hid: incluye los ficheros ocultos\n"
               "\t-reca: recursivo (antes)\n"
               "\t-recb: recursivo (despues)\n"
               "\tresto parametros como stat\n");
    } else if (strcmp(arg, "delete") == 0) {
        printf("delete [name1 name2 ..] Borra ficheros o directorios vacios\n");
    } else if (strcmp(arg, "deltree") == 0) {
        printf("deltree [name1 name2 ..] Borra ficheros o directorios no vacios recursivamente\n");
    } else { printf("%s no encontrado\n", arg); }
}

void hist(char* arg, List *L, int *N){
    if (arg == NULL) {
        for (pos aux = first(*L)->next; aux != NULL; aux = next(*L, aux)) {
            printf("%d->%s %s\n", aux->datos.commandNumber, aux->datos.command,aux->datos.argument);
        }
    } else if (strcmp(arg, "-c") == 0) {
        deleteList(L);
        *N = -1;
    } else if (esnumero(arg)) {
        pos aux = first(*L)->next;
        while (aux->datos.commandNumber != abs(atoi(arg)) && aux != NULL) {
            printf("%d->%s %s\n", aux->datos.commandNumber, aux->datos.command,aux->datos.argument);
            aux = next(*L, aux);
        }
    }
}
void comando(char* arg, List *L, int *N, bool *T){
    char*aux2[2];
    if (atoi(arg) > *N) {
        printf("No hay elemento %s en el historico\n", arg);
    } else {
        for (pos aux = first(*L)->next; aux != NULL; aux = aux->next) {
            if (aux->datos.commandNumber == atoi(arg)) {
                printf("Ejecutando comando %d: %s %s\n",*N,aux->datos.command,aux->datos.argument);
                aux2[0]=aux->datos.command;
                aux2[1]=aux->datos.argument;
                procesarEntrada(T,aux2,L,N);
            }
        }

    }
}
void create(char* arg){
    if(arg==NULL){
        carpeta(arg);
    }
    else if(mkdir(arg,0777)!=0){
        perror("No es posible crear el directorio\n");
    }
}
void stat(char* arg){

}
void list(char* arg){

}
void delete(char* arg){

}
void deltree(char* arg){

}
