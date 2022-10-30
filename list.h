
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

struct node {
    void* datos;
    pos next;
    int type;//0 item 1 block
};

typedef struct node *List;

List createList(int type);
bool isEmptyList(List L);
void insert(List *l,void* data);
pos first(List l);
pos next(List l, pos p);
void deleteList(List *L);
void deleteelem(pos p, List *L);



#endif //P0_SO_LIST_H
