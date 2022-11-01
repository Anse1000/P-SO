//
// Created by pablo on 03/10/2022.
//

#include "commands.h"
#include "aux-fun.h"

struct comando comandos[30] = {{"autores",    autores,    "autores [-n|-l] Muestra los nombres y logins de los autores\n"},
                               {"pid",        pid,        "pid [-p]        Muestra el pid del shell o de su proceso padre\n"},
                               {"infosis",    infosis,
                                                          "infosis         Muestra informacion de la maquina donde corre el shell\n"},
                               {"carpeta",    carpeta,
                                                          "carpeta [dir]   Cambia (o muestra) el directorio actual del shell\n"},
                               {"fecha",      fecha,      "fecha [-d|-h]    Muestra la fecha y o la hora actual\n"},
                               {"hist",       hist,       "hist [-c|-N]    Muestra el historico de comandos, con -c lo borra\n"},
                               {"comando",    comando,    "comando [-N]    Repite el comando N (del historico)\n"},
                               {"fin",        salir,      "Termina la ejecucion del shell"},
                               {"salir",      salir,      "Termina la ejecucion del shell"},
                               {"exit",       salir,      "Termina la ejecucion del shell"},
                               {"bye",        salir,      "Termina la ejecucion del shell"},
                               {"ayuda",      ayuda,      "ayuda [cmd]     Muestra ayuda sobre los comandos\n"},
                               {"create",     create,     "create [-f] [name]      Crea un directorio o un fichero (-f)\n"},
                               {"stat",       stat1,      "stat [-long][-link][-acc] name1 name2 .. lista ficheros;\n"
                                                          "\t-long: listado largo\n"
                                                          "\t-acc: acesstime\n"
                                                          "\t-link: si es enlace simbolico, el path contenido\n"},
                               {"list",       list,
                                                          "list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 .. lista contenidos de directorios\n"
                                                          "\t-hid: incluye los ficheros ocultos\n"
                                                          "\t-reca: recursivo (antes)\n"
                                                          "\t-recb: recursivo (despues)\n"
                                                          "\tresto parametros como stat\n"},
                               {"delete",     delete,     "delete [name1 name2 ..] Borra ficheros o directorios vacios\n"},
                               {"deltree",    deltree,
                                                          "deltree [name1 name2 ..] Borra ficheros o directorios no vacios recursivamente\n"},
                               {"allocate",   allocate,   "allocate [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria\n"
                                                          "        -malloc tam: asigna un bloque malloc de tamano tam\n"
                                                          "        -createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n"
                                                          "        -shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n"
                                                          "        -mmap fich perm: mapea el fichero fich, perm son los permisos\n"},
                               {"deallocate", deallocate, "deallocate [-malloc|-shared|-delkey|-mmap|addr]..       Desasigna un bloque de memoria\n"
                                                          "        -malloc tam: desasigna el bloque malloc de tamano tam\n"
                                                          "        -shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n"
                                                          "        -delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n"
                                                          "        -mmap fich: desmapea el fichero mapeado fich\n"
                                                          "        addr: desasigna el bloque de memoria en la direccion addr en base 16\n"},
                               {"i-o",        io,         "i-o [read|write] [-o] fiche addr cont\n"
                                                          "        read fich addr cont: Lee cont bytes desde fich a addr\n"
                                                          "        write [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n"
                                                          "                addr es una direccion de memoria\n"},
                               {"memdump",    memdump,    "memdump addr cont       Vuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n"},
                               {"memory",     memory,     "memory [-blocks|-funcs|-vars|-all|-pmap] ..     Muestra muestra detalles de la memoria del proceso\n"
                                                          "                -blocks: los bloques de memoria asignados\n"
                                                          "                -funcs: las direcciones de las funciones\n"
                                                          "                -vars: las direcciones de las variables\n"
                                                          "                :-all: todo\n"
                                                          "                -pmap: muestra la salida del comando pmap(o similar)\n"},
                               {"recurse",    recurse,    "recurse [n]     Invoca a la funcion recursiva n veces"},
                               {"memfill",    memfill,    "memfill addr cont byte  Llena la memoria a partir de addr con byte"}};


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
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
    } else {
        if (chdir(p.arg[1]) != 0) { printf("El directorio seleccionado no existe.\n"); }
    }
    return 0;
}

int fecha(struct parametros p) {
    time_t t = time(NULL);
    struct tm tiempo = *localtime(&t);

    if (p.arg[1] == NULL) {
        printf("%d:%02d:%02d\n", tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);
        printf("%d/%d/%d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900);
    } else if (strcmp(p.arg[1], "-d") == 0) {
        printf("%d/%d/%d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900);
    } else if (strcmp(p.arg[1], "-h") == 0) {
        printf("%d:%02d:%02d\n", tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec);
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
    if (p.arg[1] == NULL) {
        printf("ayuda [cmd]     Muestra ayuda sobre los comandos\n\tComandos disponibles:\n");
        for (int i = 0; comandos[i].nombre != NULL; i++) {
            printf("%s ", comandos[i].nombre);
        }
        printf("\n");
    } else {
        for (int i = 0; comandos[i].nombre != NULL; i++) {
            if (strcmp(comandos[i].nombre, p.arg[1]) == 0) {
                printf("%s", comandos[i].ayuda);
                return 0;
            }
        }
        printf("%s no encontrado\n", p.arg[1]);
        return -1;

    }
    return 0;
}

int hist(struct parametros p) {
    if (p.arg[1] == NULL) {
        for (pos aux = first(*p.H)->next; aux != NULL; aux = next(*p.H, aux)) {
            printf("%d->%s\n", ((item) aux->datos)->commandNumber, ((item) aux->datos)->commands);
        }
    } else if (strcmp(p.arg[1], "-c") == 0) {
        deleteList(p.H);
        *p.N = -1;
    } else if (esnumero(p.arg[1])) {
        pos aux = first(*p.H)->next;
        while (aux != NULL && ((item) aux->datos)->commandNumber != abs(atoi(p.arg[1]))) {
            printf("%d->%s\n", ((item) aux->datos)->commandNumber, ((item) aux->datos)->commands);
            aux = next(*p.H, aux);
        }
    }
    return 0;
}

int comando(struct parametros p) {
    struct parametros q;
    char *trozo[10];
    if (p.arg[1] == NULL) {
        hist(p);
        return 0;
    }
    if (atoi(p.arg[1]) > *p.N) {
        printf("No hay elemento %s en el historico\n", p.arg[1]);
    } else {
        for (pos aux = first(*p.H)->next; aux != NULL; aux = aux->next) {
            if (((item) aux->datos)->commandNumber == atoi(p.arg[1])) {
                printf("Ejecutando comando %d: %s\n", ((item) aux->datos)->commandNumber,
                       ((item) aux->datos)->commands);
                q.N = p.N;
                q.H = p.H;
                q.T = p.T;
                TrocearCadena(((item) aux->datos)->commands, trozo);
                q.arg = trozo;
                for (int i = 0; comandos[i].nombre != NULL; i++) {
                    if (strcmp(comandos[i].nombre, strtok(((item) aux->datos)->commands, " ")) == 0) {
                        return comandos[i].funcion(q);
                    }
                }
            }
        }
    }
    return -1;
}

int create(struct parametros p) {
    char aux[80];
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
    } else if (strcmp(p.arg[1], "-f") == 0) {
        open(p.arg[2], O_CREAT, 0777);
    } else {
        if (mkdir(p.arg[1], 0777) != 0) {
            perror("No es posible crear el directorio");
        }
    }
    return 0;
}

int stat1(struct parametros p) {
    char aux[80];
    char buffer[1000];
    unsigned int opciones[3] = {0, 0, 0};
    struct stat s;
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
        return -1;
    }
    int archivo = encontrar_archivo(p.arg);
    if (archivo == -1) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
        return -1;
    }
    opciones_stat(p.arg, archivo, opciones);
    for (int j = archivo; p.arg[j] != NULL; j++) {
        if (lstat(p.arg[j], &s) == -1) {
            perror("Error al obtener los datos del fichero");
            continue;
        }
        printf("%s", imprimir_info_archivo(p.arg[j], opciones, s, buffer));
    }

    return 0;
}

int list(struct parametros p) {
    char aux[30][100];
    //for (int i = 0; i < (sizeof(aux) / sizeof(char *)); i++) aux[i] = malloc(sizeof(char *));
    char carpeta[80];
    unsigned int oplist[3] = {0, 0, 0};
    unsigned int opstat[3] = {0, 0, 0};
    if (p.arg[1] == NULL) {
        getcwd(carpeta, 80) != NULL ? printf("%s\n", carpeta) : perror("Imposible obtener el directorio");
        return -1;
    }
    int archivo = encontrar_archivo(p.arg);
    if (archivo == -1) {
        getcwd(carpeta, 80) != NULL ? printf("%s\n", carpeta) : perror("Imposible obtener el directorio");
        return -1;
    }
    opciones_list(p.arg, archivo, oplist);
    opciones_stat(p.arg, archivo, opstat);
    for (int j = archivo; p.arg[j] != NULL; j++) {
        int last = 0;
        encontrar_dir(p.arg[j], aux, &last);
        if (oplist[0] == 1) {
            for (int i = 0; i <= last; i++) {
                imprimir_info_dir(aux[i], oplist[2], opstat);
            }
            imprimir_info_dir(p.arg[j], oplist[2], opstat);
        } else if (oplist[1] == 1) {
            imprimir_info_dir(p.arg[j], oplist[2], opstat);
            for (int i = 0; i < last; i++) {
                imprimir_info_dir(aux[i], oplist[2], opstat);
            }
        } else {
            imprimir_info_dir(p.arg[j], oplist[2], opstat);
        }
    }
    return 0;
}

int delete(struct parametros p) {
    char aux[80];
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
        return -1;
    }
    for (int j = 1; p.arg[j] != NULL; j++) {
        if (remove(p.arg[j]) != 0) {
            perror("Error ");
            return -1;
        }
    }
    return 0;
}


int deltree(struct parametros p) {
    char aux[80];
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
        return -1;
    }
    for (int i = 1; p.arg[i] != NULL; i++) {
        borrar_rec(p.arg[1]);
        if (remove(p.arg[1]) == -1) { perror("No es posible el borrado:"); }
    }
    return 0;
}

int allocate(struct parametros p) {
    void *aux;
    unsigned long aux2;
    int aux3;
    if (p.arg[1] == NULL) {
        imprimirmem(*p.M, all);
    } else if (strcmp(p.arg[1], "-malloc") == 0) {
        if (p.arg[2] == NULL) {
            imprimirmem(*p.M, malloced);
        } else if ((aux = malloc(aux2 = strtoul(p.arg[2], NULL, 10))) != NULL) {
            printf("Asignados %lu bytes en %p\n", aux2, aux);
            insertMemoria(p.M, aux2, aux, malloced);
        } else {
            printf("No se asignan bloques de 0 bytes\n");
        }
    } else if (strcmp(p.arg[1], "-createshared") == 0) {
        if (p.arg[2] == NULL || p.arg[3] == NULL) {
            imprimirmem(*p.M, shared);
        } else {
            if ((aux = ObtenerMemoriaShmget(aux3 = atoi(p.arg[2]), aux2 = strtoul(p.arg[3], NULL, 10))) == NULL) {
                perror("Imposible asignar memoria compartida:");
            } else {
                printf("Asignados %d bytes en %p\n", aux3, aux);
                insertMemoria(p.M, aux2, aux, shared, aux3);
            }

        }
    } else if (strcmp(p.arg[1], "-shared") == 0) {
        if (p.arg[2] == NULL) {
            imprimirmem(*p.M, shared);
        } else {
            if ((aux = ObtenerMemoriaShmget(aux3 = atoi(p.arg[2]), 0)) == NULL) {
                perror("Imposible asignar memoria compartida:");
            } else {
                struct shmid_ds buf;
                shmctl(aux3, IPC_STAT, &buf);
                printf("Memoria compartida de clave %d en %p\n", aux3, aux);
                insertMemoria(p.M, (int) buf.shm_segsz, aux, shared, aux3);
            }
        }
    } else if (strcmp(p.arg[1], "-mmap") == 0) {
        char *perm;
        int protection = 0;
        if (p.arg[2] == NULL || p.arg[3] == NULL) {
            imprimirmem(*p.M, mapped);
        } else {
            if (strlen(perm = p.arg[3]) < 4) {
                if (strchr(perm, 'r') != NULL) protection |= PROT_READ;
                if (strchr(perm, 'w') != NULL) protection |= PROT_WRITE;
                if (strchr(perm, 'x') != NULL) protection |= PROT_EXEC;
            }
            if ((aux = MapearFichero(p.arg[2], protection, p.M)) == NULL)
                perror("Imposible mapear fichero");
            else {
                printf("fichero %s mapeado en %p\n", p.arg[2], aux);
            }
        }
    }
    return 0;
}

int deallocate(struct parametros p) {
    unsigned long s;
    if (p.arg[1] == NULL) {
        imprimirmem(*p.M, all);
    } else if (strcmp(p.arg[1], "-malloc") == 0) {
        if (p.arg[2] == NULL) {
            imprimirmem(*p.M, malloced);
        } else if ((s = strtoul(p.arg[2], NULL, 10)) == 0) {
            printf("No se asignan bloques de 0 bytes\n");
        } else {
            unmalloc(s, p.M);
        }
    } else if (strcmp(p.arg[1], "-shared") == 0) {
        if (p.arg[2] == NULL) {
            imprimirmem(*p.M, shared);
        } else {
            unshare(atoi(p.arg[2]), p.M);
        }
    } else if (strcmp(p.arg[1], "-delkey") == 0) {
        key_t clave;
        int id;
        if (p.arg[2] == NULL || (clave = (key_t) strtoul(p.arg[2], NULL, 10)) == IPC_PRIVATE) {
            printf("      delkey necesita clave_valida\n");
            return -1;
        }
        if ((id = shmget(clave, 0, 0666)) == -1) {
            perror("shmget: imposible obtener memoria compartida");
            return -1;
        }
        if (shmctl(id, IPC_RMID, NULL) == -1)
            perror("shmctl: imposible eliminar memoria compartida\n");
    } else if (strcmp(p.arg[1], "-mmap") == 0) {
        if (p.arg[2] == NULL) {
            imprimirmem(*p.M, mapped);
        } else {
            unmap(p.arg[2], p.M);
        }

    } else {
        char aux[20];
        for (pos i = first(*p.M)->next; i != NULL; i = next(*p.M, i)) {
            snprintf(aux, 20, "%p", ((block) i->datos)->address);
            if (strcmp(p.arg[1], aux) == 0) {
                switch (((block) i->datos)->type) {
                    case malloced:
                        unmalloc(((block) i->datos)->size, p.M);
                        break;
                    case shared:
                        unshare(((block) i->datos)->sharedkey, p.M);
                        break;
                    case mapped:
                        unmap(((block) i->datos)->mapfilename, p.M);
                        break;
                    case all:
                        break;
                }
                return 0;
            }
        }
        printf("Direccion %s no asignada con malloc shared o mmap\n", p.arg[1]);
    }
    return 0;
}

int io(struct parametros p) {
    void *aux;
    int o=0;
    size_t cont=-1;
    ssize_t n;
    if(p.arg[1]==NULL){
        printf("uso: e-s [read|write] ......\n");
        return -1;
    }
    else if(strcmp(p.arg[1],"read")==0){
        if (p.arg[2]==NULL || p.arg[3]==NULL){
            printf ("faltan parametros\n");
            return -1;
        }
        sscanf(p.arg[3],"%p",&aux);  /*convertimos de cadena a puntero*/
        if (p.arg[4]!=NULL)
            cont=(size_t) atoll(p.arg[4]);

        if ((n=LeerFichero(p.arg[2],aux,cont))==-1)
            perror ("Imposible leer fichero");
        else
            printf ("leidos %lld bytes de %s en %p\n",(long long) n,p.arg[2],aux);
        return 0;
    }
    else if(strcmp(p.arg[1],"write")==0){
        if (p.arg[2]==NULL){
            printf ("faltan parametros\n");
            return -1;
        }
        if(strcmp(p.arg[2],"-o")==0){
            o=1;
        }
        if (p.arg[2+o]==NULL || p.arg[3+o]==NULL){
            printf ("faltan parametros\n");
            return -1;
        }
        sscanf(p.arg[3+o],"%p",&aux);  /*convertimos de cadena a puntero*/
        if (p.arg[4+o]!=NULL)
            cont=(size_t) atoll(p.arg[4+o]);

        if ((n= EscribirFichero(p.arg[2+o],aux,cont,o))==-1)
            perror ("Imposible escribir fichero");
        else
            printf ("escritos %lld bytes de %s en %p\n",(long long) n,p.arg[2+o],aux);
        return 0;
    }
    return -1;
}

int memdump(struct parametros p) {
    int cont;
    char* aux;
    if(p.arg[1]==NULL){
        return -1;
    }
    if(p.arg[2]==NULL) cont=25;
    else cont=atoi(p.arg[2]);
    sscanf(p.arg[1],"%p",&aux);
    printf("Volcando %d bytes desde la direccion %p\n",cont,aux);
    for(int i=0;i<cont;i++){
        printf("%02X ",aux[i]);
    }
    printf("\n");
    return 0;
}
//para memory
int g1,g2,g3;
int memory(struct parametros p) {
    int a,b,c;
    static int d,e,f;
    auto int g,h,i;
    unsigned int opciones[4]={0,0,0,0};
    char pid[10];
    if(p.arg[1]==NULL){
        opciones[0]=1;
        opciones[1]=1;
        opciones[2]=1;
    }
    else{opciones_memory(p.arg,opciones);}
    if(opciones[0]==1){
        printf("Variables locales    \t%p ,%p ,%p\n",&a,&b,&c);
        printf("Variables automaticas \t%p ,%p ,%p\n",&g,&h,&i);
        printf("Variables estaticas \t%p ,%p ,%p\n",&d,&e,&f);
        printf("Variables globales    \t%p ,%p ,%p\n",&g1,&g2,&g3);
    }
    if(opciones[1]==1){
        printf("Funciones programa    \t%p ,%p ,%p\n",fecha,infosis,autores);
        printf("Funciones libreria    \t%p ,%p ,%p\n",printf,scanf,strcmp);
    }
    if(opciones[2]==1){
        imprimirmem(*p.M,all);
    }
    if(opciones[3]==1&&(opciones[0]==0&&opciones[1]==0&&opciones[2]==0)){
        sprintf(pid,"%d",getpid());
        char *args[3]={"-x",pid,NULL};
        if(fork()==0){
            execvp("pmap",args);
        }
        wait(NULL);
    }
    return 0;
}

int recurse(struct parametros p) {
    if(p.arg[1]==NULL){
        return -1;
    }
    else {
        Recursiva(atoi(p.arg[1]));
        return 0;
    }
}

int memfill(struct parametros p) {
    return 0;
}