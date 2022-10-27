
#ifndef P0_SO_LIST_H
#define P0_SO_LIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct node *pos;
typedef struct item{
    int commandNumber;
    char commands[100];
}*item;

enum types{malloced,shared,mapped};

typedef struct block{
    void* address;
    unsigned int size;
    char time[80];
    enum types type;
    key_t sharedkey;
    char mapfilename[30];
    int mapfiledesc;
}*block;

struct node {
    void* datos;
    pos next;
};

typedef struct node *List;

List createList();
bool isEmptyList(List L);
void insert(List *l,void* data);
pos first(List l);
pos next(List l, pos p);
void deleteList(List *L);




#endif //P0_SO_LIST_H
