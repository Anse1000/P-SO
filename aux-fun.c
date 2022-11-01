//
// Created by pablo on 29/10/2022.
//
#include "commands.h"

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
    for (i = 1; a[i] != NULL && a[i][0] == '-'; i++);
    return a[i] == NULL ? -1 : i;
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

void insertMemoria(List *M, unsigned long size, void *address, enum types type, ...) {//sharedkey,mapfilename,mapfiledesc
    va_list argptr;
    va_start(argptr, type);
    block B = malloc(sizeof(struct block));
    time_t t = time(NULL);
    struct tm T = *localtime(&t);
    B->type = type;
    B->address = address;
    sprintf(B->time, "%d/%d/%d-%d:%02d\t", T.tm_year + 1900, T.tm_mon + 1, T.tm_mday, T.tm_hour, T.tm_min);
    B->size = size;
    B->sharedkey = va_arg(argptr, int);
    if (type==mapped) {
        strcpy(B->mapfilename, va_arg(argptr, char*));
        B->mapfiledesc = va_arg(argptr, int);
    }
    insert(M, B);
    free(B);
    va_end(argptr);
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam) {
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags = flags | IPC_CREAT | IPC_EXCL;
    if (clave == IPC_PRIVATE)  /*no nos vale*/
    {
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL, 0)) == (void *) -1) {
        aux = errno;
        if (tam)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return (NULL);
    }
    shmctl(id, IPC_STAT, &s);
    return (p);
}

void *MapearFichero(char *fichero, int protection, List *M) {
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;

    if (protection & PROT_WRITE)
        modo = O_RDWR;
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
        return NULL;
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED)
        return NULL;
    insertMemoria(M, (int) s.st_size, p, mapped, fichero, df);
    return p;
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

char *imprimir_info_archivo(char *archivo, const unsigned int *opciones, struct stat s, char *buffer) {
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
        if (opciones[2] == 1 && S_ISLNK(s.st_mode)) {
            aux2 = (int) readlink(archivo, aux, 80);
            if (aux2 == -1) {
                strcpy(aux, "**error al obtener el enlace");
            }
            sprintf(buffer, "%s -> %s ", buffer, aux);
        }
    } else {
        sprintf(buffer, "%ld  %s", s.st_size, archivo);
    }

    strcat(buffer, "\n");
    return buffer;
}

void opciones_list(char **arg, int archivo, unsigned int *aux) {
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
}
void opciones_memory(char **arg,unsigned int *aux){
    for(int i = 0;arg[i]!=NULL;i++){
        if(strcmp(arg[i],"-all")==0){
            aux[0]=1;
            aux[1]=1;
            aux[2]=1;
            break;
        }
        else if(strcmp(arg[i],"-vars")==0){
            aux[0]=1;
            continue;
        }
        else if(strcmp(arg[i],"-funcs")==0){
            aux[1]=1;
            continue;
        }
        else if(strcmp(arg[i],"-blocks")==0){
            aux[2]=1;
            continue;
        }
        else if(strcmp(arg[i],"-pmap")==0){
            aux[3]=1;
            continue;
        }
        else{continue;}
    }
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
        printf("**** %s ***\n", archivo);
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
    } else {
        perror("Imposible acceder al directorio:");
    }
}

void borrar_rec(char *file) {
    DIR *d;
    struct dirent *dir;
    char aux[3000];
    if (remove(file) != 0) {
        d = opendir(file);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_name[0] == '.') { continue; }
                strcpy(aux, file);
                strcat(strcat(aux, "/"), dir->d_name);
                if (remove(aux) != 0) {
                    borrar_rec(aux);
                }
                remove(aux);
            }
        }
    }
}

char *enum_str(enum types t) {
    if (t == malloced) {
        return "malloc";
    } else if (t == shared) {
        return "shared";
    } else if (t == mapped) {
        return "mapped";
    } else {
        return "";
    }
}

void imprimirmem(List M, enum types t) {
    char buffer[300], key[40];
    printf("***Lista de bloques asignados %s para el proceso %d\n", enum_str(t), getpid());
    for (pos i = first(M)->next; i != NULL; i = next(M, i)) {
        if (t != all) {
            if (((block) i->datos)->type == t) {
                sprintf(buffer, "\t %p \t %lu %s  %s", ((block) i->datos)->address, ((block) i->datos)->size,
                        ((block) i->datos)->time, enum_str(((block) i->datos)->type));
                if (((block) i->datos)->type == shared) {
                    snprintf(key, 40, " (key %d) \n", ((block) i->datos)->sharedkey);
                    strcat(buffer, key);
                } else {
                    strcat(buffer, "\n");
                }
            }
        } else {
            sprintf(buffer, "\t %p \t %lu %s  %s", ((block) i->datos)->address, ((block) i->datos)->size,
                    ((block) i->datos)->time, enum_str(((block) i->datos)->type));
            if (((block) i->datos)->type == shared) {
                snprintf(key, 40, " (key %d) \n", ((block) i->datos)->sharedkey);
                strcat(buffer, key);
            } else {
                strcat(buffer, "\n");
            }

        }
        printf("%s", buffer);
    }
}

void unmalloc(unsigned long s, List *M) {
    for (pos i = first(*M)->next; i != NULL; i = next(*M, i)) {
        if (((block) i->datos)->size == s && ((block) i->datos)->type == malloced) {
            free(((block) i->datos)->address);
            deleteelem(i, M);
            return;
        }
    }
    printf("No existen bloques de ese tamano\n");
}

void unshare(key_t key, List *M) {
    for (pos i = first(*M)->next; i != NULL; i = next(*M, i)) {
        if (((block) i->datos)->type == shared && ((block) i->datos)->sharedkey == key) {
            if (shmdt(((block) i->datos)->address) == -1) {
                perror("Error:");
            } else {
                deleteelem(i, M);
                return;
            }
        }
    }
}

void unmap(char *file, List *M) {
    for (pos i = first(*M)->next; i != NULL; i = next(*M, i)) {
        if (((block) i->datos)->type == mapped && strcmp(((block) i->datos)->mapfilename, file) == 0) {
            if (munmap(((block) i->datos)->address, ((block) i->datos)->size) == -1) {
                perror("Error:");

            } else {
                deleteelem(i, M);
                return;
            }
        }
    }
    printf("Archivo %s no mapeado\n", file);
}
void Recursiva (int n)
{
    char automatico[2048];
    static char estatico[2048];

    printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

    if (n>0)
        Recursiva(n-1);
}
ssize_t LeerFichero (char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}
ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite)
{
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;

    if ((df=open(f,flags,0777))==-1)
        return -1;

    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}
