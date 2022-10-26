
#ifndef P0_SO_LIST_H
#define P0_SO_LIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct node *pos;
typedef struct item{
    int commandNumber;
    char commands[100];
}*item;
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
