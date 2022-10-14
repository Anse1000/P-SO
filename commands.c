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
#include <fcntl.h>

struct comando comandos[20] = {{"autores", autores, "autores [-n|-l] Muestra los nombres y logins de los autores\n"},
                             {"pid",     pid,     "pid [-p]        Muestra el pid del shell o de su proceso padre\n"},
                             {"infosis", infosis, "infosis         Muestra informacion de la maquina donde corre el shell\n"},
                             {"carpeta", carpeta, "carpeta [dir]   Cambia (o muestra) el directorio actual del shell\n"},
                             {"fecha",   fecha,   "fecha [-d|-h]    Muestra la fecha y o la hora actual\n"},
                             {"hist",    hist,    "hist [-c|-N]    Muestra el historico de comandos, con -c lo borra\n"},
                             {"comando", comando, "comando [-N]    Repite el comando N (del historico)\n"},
                             {"fin",     salir,   "Termina la ejecucion del shell"},
                             {"salir",   salir,   "Termina la ejecucion del shell"},
                             {"bye",     salir,   "Termina la ejecucion del shell"},
                             {"ayuda",   ayuda,   "ayuda [cmd]     Muestra ayuda sobre los comandos\n"},
                             {"create",  create,  "create [-f] [name]      Crea un directorio o un fichero (-f)\n"},
                             {"stat",    stat1,    "stat [-long][-link][-acc] name1 name2 .. lista ficheros;\n"
                                                  "\t-long: listado largo\n"
                                                  "\t-acc: acesstime\n"
                                                  "\t-link: si es enlace simbolico, el path contenido\n"},
                             {"list",    list,    "list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 .. lista contenidos de directorios\n"
                                                  "\t-hid: incluye los ficheros ocultos\n"
                                                  "\t-reca: recursivo (antes)\n"
                                                  "\t-recb: recursivo (despues)\n"
                                                  "\tresto parametros como stat\n"},
                             {"delete",  delete,  "delete [name1 name2 ..] Borra ficheros o directorios vacios\n"},
                             {"deltree", deltree, "deltree [name1 name2 ..] Borra ficheros o directorios no vacios recursivamente\n"}};

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

int salir(struct parametros p) {
    *p.T = true;
    return 0;
}

int autores(struct parametros p) {
    if (p.arg[1] == NULL) {
        printf("Pablo Rey Ansemil: pablo.rey.ansemil@udc.es\n");
        printf("Raul Romero Saavedra: raul.romero.saavedra@udc.es \n");
    } else if (strcmp(p.arg[1], "-l") == 0) {
        printf("pablo.rey.ansemil@udc.es\nraul.romero.saavedra@udc.es\n");
    } else if (strcmp(p.arg[1], "-n") == 0) {
        printf("Pablo Rey Ansemil\nRaul Romero Saavedra\n");
    }
    return 0;
}

int pid(struct parametros p) {
    if (p.arg[1] == NULL) {
        printf("Pid del shell: %d\n", getpid());
    } else if (strcmp(p.arg[1], "-p") == 0) {
        printf("Pid del padre del shell: %d\n", getppid());
    }
    return 0;
}

int carpeta(struct parametros p) {
    char aux[80];
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio.\n");
    } else {
        if (chdir(p.arg[1]) != 0) { printf("El directorio seleccionado no existe.\n"); }
    }
    return 0;
}

int fecha(struct parametros p) {
    time_t t = time(NULL);
    struct tm tiempo = *localtime(&t);

    if (p.arg[1] == NULL) {
        printf("%d:%d:%d\n", tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);
        printf("%d/%d/%d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900);
    } else if (strcmp(p.arg[1], "-d") == 0) {
        printf("%d/%d/%d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900);
    } else if (strcmp(p.arg[1], "-h") == 0) {
        printf("%d:%d:%d\n", tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);
    }
    return 0;
}

int infosis() {
    struct utsname equipo;
    uname(&equipo);
    printf("%s(%s), OS: %s-%s-%s \n", equipo.nodename, equipo.machine, equipo.sysname, equipo.release,
           equipo.version);
    return 0;
}

int ayuda(struct parametros p) {
    for(int i=0;comandos[i].nombre!=NULL;i++){
        if(strcmp(comandos[i].nombre,p.arg[1])==0){
            printf("%s", comandos[i].ayuda);
            return 0;
        }
    } printf("%s no encontrado\n", p.arg[1]);
    return 0;
}

int hist(struct parametros p) {
    if (p.arg[1] == NULL) {
        for (pos aux = first(*p.L)->next; aux != NULL; aux = next(*p.L, aux)) {
            printf("%d->%s %s\n", aux->datos.commandNumber, aux->datos.command, aux->datos.argument);
        }
    } else if (strcmp(p.arg[1], "-c") == 0) {
        deleteList(p.L);
        *p.N = -1;
    } else if (esnumero(p.arg[1])) {
        pos aux = first(*p.L)->next;
        while (aux->datos.commandNumber != abs(atoi(p.arg[1])) && aux != NULL) {
            printf("%d->%s %s\n", aux->datos.commandNumber, aux->datos.command, aux->datos.argument);
            aux = next(*p.L, aux);
        }
    }
    return 0;
}

int comando(struct parametros p) {
    char *aux2[2];
    if (atoi(p.arg[1]) > *p.N) {
        printf("No hay elemento %s en el historico\n", p.arg[1]);
    } else {
        for (pos aux = first(*p.L)->next; aux != NULL; aux = aux->next) {
            if (aux->datos.commandNumber == atoi(p.arg[1])) {
                printf("Ejecutando comando %d: %s %s\n", *p.N, aux->datos.command, aux->datos.argument);
                aux2[0] = aux->datos.command;
                aux2[1] = aux->datos.argument;
                procesarEntrada(p.T, aux2, p.L, p.N);
            }
        }

    }
    return 0;
}

int create(struct parametros p) {
    char aux[80];
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio.\n");
    } else if(strcmp(p.arg[1], "-f") == 0){
        open(p.arg[2],O_CREAT,0777);
    } else{
        if(mkdir(p.arg[1], 0777) != 0) {
            perror("No es posible crear el directorio\n");
        }
    }
    return 0;
}

int stat1(struct parametros p) {
    return 0;
}

int list(struct parametros p) {
    return 0;
}

int delete(struct parametros p) {
    return 0;
}

int deltree(struct parametros p) {
    return 0;
}
