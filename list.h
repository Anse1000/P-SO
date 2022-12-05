
#ifndef P0_SO_LIST_H
#define P0_SO_LIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct node *pos;
typedef struct item{
    int commandNumber;
    char commands[100];
}*item;

enum types{malloced,shared,mapped,all};

typedef struct block{
    void* address;
    unsigned long size;
    char time[80];
    enum types type;
    key_t sharedkey;
    char mapfilename[30];
    int mapfiledesc;
}*block;
typedef struct process{
    pid_t pid;
    char time[80];
    char commandline[100];
    int signalvalue;
    char signal[15];
    char user[20];
}*process;

struct node {
    void* datos;
    pos next;
};

typedef struct node *List;

List createList(int *type);
bool isEmptyList(List L);
void insert(List *l,void* data);
pos first(List l);
pos next(List l, pos p);
void deleteList(List *L);
void deleteelem(pos p, List *L);



#endif //P0_SO_LIST_H
