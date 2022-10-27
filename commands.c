//
// Created by pablo on 03/10/2022.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include "commands.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdarg.h>

struct comando comandos[30] = {{"autores", autores, "autores [-n|-l] Muestra los nombres y logins de los autores\n"},
                               {"pid", pid, "pid [-p]        Muestra el pid del shell o de su proceso padre\n"},
                               {"infosis", infosis,
                                "infosis         Muestra informacion de la maquina donde corre el shell\n"},
                               {"carpeta", carpeta,
                                "carpeta [dir]   Cambia (o muestra) el directorio actual del shell\n"},
                               {"fecha", fecha, "fecha [-d|-h]    Muestra la fecha y o la hora actual\n"},
                               {"hist", hist, "hist [-c|-N]    Muestra el historico de comandos, con -c lo borra\n"},
                               {"comando", comando, "comando [-N]    Repite el comando N (del historico)\n"},
                               {"fin", salir, "Termina la ejecucion del shell"},
                               {"salir", salir, "Termina la ejecucion del shell"},
                               {"exit", salir, "Termina la ejecucion del shell"},
                               {"bye", salir, "Termina la ejecucion del shell"},
                               {"ayuda", ayuda, "ayuda [cmd]     Muestra ayuda sobre los comandos\n"},
                               {"create", create, "create [-f] [name]      Crea un directorio o un fichero (-f)\n"},
                               {"stat", stat1, "stat [-long][-link][-acc] name1 name2 .. lista ficheros;\n"
                                               "\t-long: listado largo\n"
                                               "\t-acc: acesstime\n"
                                               "\t-link: si es enlace simbolico, el path contenido\n"},
                               {"list", list,
                                "list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 .. lista contenidos de directorios\n"
                                "\t-hid: incluye los ficheros ocultos\n"
                                "\t-reca: recursivo (antes)\n"
                                "\t-recb: recursivo (despues)\n"
                                "\tresto parametros como stat\n"},
                               {"delete", delete, "delete [name1 name2 ..] Borra ficheros o directorios vacios\n"},
                               {"deltree", deltree,
                                "deltree [name1 name2 ..] Borra ficheros o directorios no vacios recursivamente\n"},
                               {"allocate",allocate,"allocate [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria\n"
                                                    "        -malloc tam: asigna un bloque malloc de tamano tam\n"
                                                    "        -createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n"
                                                    "        -shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n"
                                                    "        -mmap fich perm: mapea el fichero fich, perm son los permisos\n"},
                               {"deallocate",deallocate,"deallocate [-malloc|-shared|-delkey|-mmap|addr]..       Desasigna un bloque de memoria\n"
                                                        "        -malloc tam: desasigna el bloque malloc de tamano tam\n"
                                                        "        -shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n"
                                                        "        -delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n"
                                                        "        -mmap fich: desmapea el fichero mapeado fich\n"
                                                        "        addr: desasigna el bloque de memoria en la direccion addr\n"},
                               {"i-o",io,"i-o [read|write] [-o] fiche addr cont\n"
                                "        read fich addr cont: Lee cont bytes desde fich a addr\n"
                                "        write [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n"
                                "                addr es una direccion de memoria\n"},
                               {"memdump",memdump,"memdump addr cont       Vuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n"},
                               {"memory",memory,"memory [-blocks|-funcs|-vars|-all|-pmap] ..     Muestra muestra detalles de la memoria del proceso\n"
                                                "                -blocks: los bloques de memoria asignados\n"
                                                "                -funcs: las direcciones de las funciones\n"
                                                "                -vars: las direcciones de las variables\n"
                                                "                :-all: todo\n"
                                                "                -pmap: muestra la salida del comando pmap(o similar)\n"},
                               {"recurse",recurse,"recurse [n]     Invoca a la funcion recursiva n veces"},
                               {"memfill",memfill,"memfill addr cont byte  Llena la memoria a partir de addr con byte"}};

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

int TrocearCadena(char *cadena, char *trozos[]) {
    int i = 1;
    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL)
        return 0;
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

int encontrar_archivo(char **a) {
    int i;
    for (i = 1;a[i]!=NULL && a[i][0] == '-'; i++);
    return a[i]==NULL?-1:i;
}

char LetraTF(mode_t m) {
    switch (m & S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK:
            return 's'; /*socket */
        case S_IFLNK:
            return 'l'; /*symbolic link*/
        case S_IFREG:
            return '-'; /* fichero normal*/
        case S_IFBLK:
            return 'b'; /*block device*/
        case S_IFDIR:
            return 'd'; /*directorio */
        case S_IFCHR:
            return 'c'; /*char device*/
        case S_IFIFO:
            return 'p'; /*pipe*/
        default:
            return '?'; /*desconocido, no deberia aparecer*/
    }
}

char *ConvierteModo2(mode_t m) {
    static char permisos[12];
    strcpy(permisos, "---------- ");

    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r';    /*propietario*/
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r';    /*grupo*/
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r';    /*resto*/
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's';    /*setuid, setgid y stickybit*/
    if (m & S_ISGID) permisos[6] = 's';
    if (m & S_ISVTX) permisos[9] = 't';

    return permisos;
}
void insertMemoria(List* M,int size,void* address,enum types type,...){//sharedkey,mapfilename,mapfiledesc
    va_list argptr;
    va_start(argptr, type);
    block B=malloc(sizeof(struct block));
    time_t t = time(NULL);
    struct tm T = *localtime(&t);
    B->type=type;
    B->address=address;
    sprintf(B->time, "%d/%d/%d-%d:%02d\t", T.tm_year + 1900, T.tm_mon + 1, T.tm_mday, T.tm_hour, T.tm_min);
    B->size=size;
    B->sharedkey= va_arg(argptr,int);
    if(va_arg(argptr,char*)!=NULL){
        strcpy(B->mapfilename, va_arg(argptr,char*));
    }
    B->mapfiledesc=va_arg(argptr,int);
    insert(M,B);
    va_end(argptr);
}
void * ObtenerMemoriaShmget (key_t clave, size_t tam)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  /*no nos vale*/
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    return (p);
}
void * MapearFichero (char * fichero, int protection,List*M)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    insertMemoria(M,(int)s.st_size,p,mapped,fichero,df);
    return p;
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
        for(int i=0;comandos[i].nombre != NULL;i++){
            printf("%s ",comandos[i].nombre);
        }
        printf("\n");
    }
    else {
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
            printf("%d->%s\n",((item)aux->datos)->commandNumber, ((item)aux->datos)->commands);
        }
    } else if (strcmp(p.arg[1], "-c") == 0) {
        deleteList(p.H);
        *p.N = -1;
    } else if (esnumero(p.arg[1])) {
        pos aux = first(*p.H)->next;
        while (aux != NULL && ((item)aux->datos)->commandNumber != abs(atoi(p.arg[1]))) {
            printf("%d->%s\n", ((item)aux->datos)->commandNumber, ((item)aux->datos)->commands);
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
            if (((item)aux->datos)->commandNumber == atoi(p.arg[1])) {
                printf("Ejecutando comando %d: %s\n", ((item)aux->datos)->commandNumber, ((item)aux->datos)->commands);
                q.N = p.N;
                q.H = p.H;
                q.T = p.T;
                TrocearCadena(((item)aux->datos)->commands, trozo);
                q.arg = trozo;
                for (int i = 0; comandos[i].nombre != NULL; i++) {
                    if (strcmp(comandos[i].nombre, strtok(((item)aux->datos)->commands, " ")) == 0) {
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

void opciones_stat(char **arg, int archivo, unsigned int *aux) {

    for (int i = 1; i < archivo; i++) {
        if (strcmp(arg[i], "-long") == 0) {
            aux[0] = 1;
            continue;
        } else if (strcmp(arg[i], "-acc") == 0) {
            aux[1] = 1;
            continue;
        } else if (strcmp(arg[i], "-link") == 0) {
            aux[2] = 1;
            continue;
        } else { continue; }
    }
}

char *imprimir_info_archivo(char *archivo,const unsigned int *opciones, struct stat s, char *buffer) {
    char aux[80];
    int aux2;
    struct tm T;
    if (opciones[1] == 1) {
        T = *localtime(&s.st_atim.tv_sec);
    } else {
        T = *localtime(&s.st_mtim.tv_sec);
    }
    sprintf(aux, "%d/%d/%d-%d:%02d\t", T.tm_year + 1900, T.tm_mon + 1, T.tm_mday, T.tm_hour, T.tm_min);
    if (opciones[0] == 1) {
        sprintf(buffer, "%s %ld (%.10ld)\t%s\t%s\t%s\t%ld  %s",
                aux, s.st_nlink, s.st_ino, getpwuid(s.st_uid)->pw_name, getgrgid(s.st_gid)->gr_name,
                ConvierteModo2(s.st_mode), s.st_size, archivo);
        if (opciones[2] == 1&& S_ISLNK(s.st_mode)) {
            aux2=(int)readlink(archivo,aux,80);
            if(aux2==-1){
                strcpy(aux,"**error al obtener el enlace");
            }
            sprintf(buffer, "%s -> %s ", buffer,aux);
        }
    } else {
        sprintf(buffer, "%ld  %s", s.st_size, archivo);
    }

    strcat(buffer, "\n");
    return buffer;
}

int stat1(struct parametros p) {
    char aux[80];
    char buffer[1000];
    unsigned int opciones[3] = {0,0,0};
    struct stat s;
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
        return -1;
    }
    int archivo = encontrar_archivo(p.arg);
    if (archivo==-1){
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

unsigned int *opciones_list(char **arg, int archivo, unsigned int *aux) {
    for (int i = 1; i < archivo; i++) {
        if (strcmp(arg[i], "-reca") == 0) {
            aux[0] = 1;
            break;
        } else if (strcmp(arg[i], "-recb") == 0) {
            aux[1] = 1;
            continue;
        } else if (strcmp(arg[i], "-hid") == 0) {
            aux[2] = 1;
            continue;
        } else { continue; }
    }
    return aux;
}

void encontrar_dir(char *archivo, char aux[30][100], int *last) {
    DIR *d;
    struct dirent *dir;
    struct stat s;
    char aux1[80];
    d = opendir(archivo);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.')continue;
            strcpy(aux1, archivo);
            strcat(strcat(aux1, "/"), dir->d_name);
            stat(aux1, &s);
            if (S_ISDIR(s.st_mode)) {
                sprintf(aux[*last], "%s/%s", archivo, dir->d_name);
                *last += 1;
                encontrar_dir(aux[*last - 1], aux, last);
            }
        }
    }
    closedir(d);
}

void imprimir_info_dir(char *archivo, unsigned int oplist, const unsigned int *opstat) {
    DIR *d;
    struct dirent *dir;
    char buffer[1000];
    struct stat s;
    char aux1[100];
    d = opendir(archivo);
    if (d) {
        printf("\n**** %s ***\n", archivo);
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.' && oplist != 1) { continue; }
            strcpy(aux1, archivo);
            strcat(strcat(aux1, "/"), dir->d_name);
            if (lstat(aux1, &s) == -1) {
                perror("Error al obtener los datos del fichero");
                continue;
            }
            printf("%s", imprimir_info_archivo(dir->d_name, opstat, s, buffer));
        }
        closedir(d);
    }

    else{
        perror("Imposible acceder al directorio:");
    }
}

int list(struct parametros p) {
    char aux[30][100];
    //for (int i = 0; i < (sizeof(aux) / sizeof(char *)); i++) aux[i] = malloc(sizeof(char *));
    char carpeta[80];
    unsigned int oplist[3] = {0,0,0};
    unsigned int opstat[3] = {0,0,0};
    if (p.arg[1] == NULL) {
        getcwd(carpeta, 80) != NULL ? printf("%s\n", carpeta) : perror("Imposible obtener el directorio");
        return -1;
    }
    int archivo = encontrar_archivo(p.arg);
    if(archivo==-1){
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
void borrar_rec(char * file){
    DIR *d;
    struct dirent *dir;
    char aux[3000];
    if (remove(file)!=0){
        d = opendir(file);
        if(d){
            while ((dir = readdir(d)) != NULL) {
                if(dir->d_name[0]=='.'){ continue;}
                strcpy(aux, file);
                strcat(strcat(aux, "/"), dir->d_name);
                if(remove(aux)!=0){
                    borrar_rec(aux);
                }
                remove(aux);
            }
        }
    }
}

int deltree(struct parametros p) {
    char aux[80];
    if (p.arg[1] == NULL) {
        getcwd(aux, 80) != NULL ? printf("%s\n", aux) : perror("Imposible obtener el directorio");
        return -1;
    }
    for(int i=1;p.arg[i]!=NULL;i++){
        borrar_rec(p.arg[1]);
        if(remove(p.arg[1])==-1){perror("No es posible el borrado:");}
    }
    return 0;
}
char *enum_str(enum types t){
    if(t==malloced){
        return "malloc";
    }
    else if(t==shared){
        return "shared";
    }
    else{
        return "mapped";
    }
}

int allocate(struct parametros p){
    void *aux;
    char buffer[300],key[40];
    if(p.arg[1]==NULL){
        printf("***Lista de bloques asignados para el proceso %d\n",getpid());
        for (pos i = first(*p.M)->next; i != NULL; i = next(*p.M, i)){
                sprintf(buffer,"\t %p \t %s  %s",((block)i->datos)->address,((block)i->datos)->time,enum_str(((block)i->datos)->type));
                if(((block)i->datos)->type==shared){
                    snprintf(key,40," (key %d) \n",((block)i->datos)->sharedkey);
                    strcat(buffer,key);
                }
                else{
                    strcat(buffer,"\n");
                }
                printf("%s",buffer);
            }
        }
    else if(strcmp(p.arg[1],"-malloc")==0){
        if(p.arg[2]==NULL){
            printf("***Lista de bloques asignados malloc para el proceso %d\n",getpid());
            for (pos i = first(*p.M)->next; i != NULL; i = next(*p.M, i)){
                if(((block)i->datos)->type==malloced){
                    printf("\t %p \t %s  %s\n",((block)i->datos)->address,((block)i->datos)->time,enum_str(((block)i->datos)->type));
                }
            }
        }
        if((aux=malloc(atoi(p.arg[2])))!=NULL){
            printf("Asignados %d bytes en %p\n",atoi(p.arg[2]),aux);
            insertMemoria(p.M,atoi(p.arg[2]),aux,malloced);
        }
        else{
            printf("No se asignan bloques de 0 bytes\n");
        }
    }
    else if(strcmp(p.arg[1],"-createshared")==0){
        if(p.arg[2]==NULL||p.arg[3]==NULL){
            printf("***Lista de bloques asignados shared para el proceso %d\n",getpid());
            for (pos i = first(*p.M)->next; i != NULL; i = next(*p.M, i)){
                if(((block)i->datos)->type==shared){
                    printf("\t %p \t %s  %s (key %d)\n",((block)i->datos)->address,((block)i->datos)->time,enum_str(((block)i->datos)->type),((block)i->datos)->sharedkey);
                }
            }
        }
        else{
            if((aux = ObtenerMemoriaShmget(atoi(p.arg[2]),atoi(p.arg[3])))== NULL){
                perror("Imposible asignar memoria compartida:");
            }
            else{
                printf("Asignados %d bytes en %p\n",atoi(p.arg[3]),aux);
                insertMemoria(p.M,atoi(p.arg[3]),aux,shared,((key_t)atoi(p.arg[2])));
            }

        }
    }
    else if(strcmp(p.arg[1],"-shared")==0){
        if(p.arg[2]==NULL){
            printf("***Lista de bloques asignados shared para el proceso %d\n",getpid());
            for (pos i = first(*p.M)->next; i != NULL; i = next(*p.M, i)){
                if(((block)i->datos)->type==shared){
                    printf("\t %p \t %s  %s (key %d)\n",((block)i->datos)->address,((block)i->datos)->time,enum_str(((block)i->datos)->type),((block)i->datos)->sharedkey);
                }
            }
        }
        else{
            if((aux=ObtenerMemoriaShmget(atoi(p.arg[2]),0))==NULL){
                perror("Imposible asignar memoria compartida:");
            }
            else{
                struct shmid_ds buf;
                shmctl(atoi(p.arg[2]), IPC_STAT, &buf);
                printf("Memoria compartida de clave %d en %p\n",atoi(p.arg[2]),aux);
                insertMemoria(p.M,(int)buf.shm_segsz,aux,shared,atoi(p.arg[2]));
            }
        }
    }
    else if(strcmp(p.arg[1],"-mmap")==0){
        char *perm;
        int protection=0;
        if(p.arg[2]==NULL||p.arg[3]==NULL){
            printf("***Lista de bloques asignados mmap para el proceso %d\n",getpid());
            for (pos i = first(*p.M)->next; i != NULL; i = next(*p.M, i)){
                if(((block)i->datos)->type==mapped){
                    printf("\t %p \t %s  %s\n",((block)i->datos)->address,((block)i->datos)->time,enum_str(((block)i->datos)->type));
                }
            }
        }
        else{
            if(strlen(perm=p.arg[3])<4){
                if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
                if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
                if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
            }
            if ((aux =MapearFichero(p.arg[2],protection,p.M))==NULL)
                perror ("Imposible mapear fichero");
            else{
                printf ("fichero %s mapeado en %p\n", p.arg[2], aux);
            }
        }
    }
    return 0;
}
int deallocate(struct parametros p){
    return 0;
}
int io(struct parametros p){
    return 0;
}
int memdump(struct parametros p){
    return 0;
}
int memory(struct parametros p){
    return 0;
}
int recurse(struct parametros p){
    return 0;
}
int memfill(struct parametros p){
    return 0;
}