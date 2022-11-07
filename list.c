

#include <string.h>
#include "list.h"
#include "aux-fun.h"

List createList(int *type) {
    struct node *N = malloc(sizeof(struct node));
    N->next = NULL;
    N->datos=type;
    return N;
}
bool isEmptyList(List L) {
    if (L->next == NULL)
        return true;
    else return false;
}

void insert(List *l, void* data) {
    pos p = *l;
    int aux=*(int*)(*l)->datos;
    while (p->next != NULL)
        p = p->next;
    p->next = malloc(sizeof(struct node));
    p->next->next = NULL;
    if(!aux){
        p->next->datos= malloc(sizeof(struct item));
        memmove(p->next->datos,data, sizeof(struct item));
    }
    else{
        p->next->datos= malloc(sizeof(struct block));
        memmove(p->next->datos,data, sizeof(struct block));
    }
}

pos first(List l) {
    return l;
}

pos next(List L, pos p) {
    return p->next;
}

void deleteList(List *L) {
    pos p;
    int aux =*(int*)(*L)->datos;
    while (!isEmptyList(*L)) {
        p = *L;
        *L = (*L)->next;
        if(aux){
            switch(((block)p->next->datos)->type){
                case malloced:
                    free(((block)p->next->datos)->address);
                    break;
                case shared:
                    shmdt(((block) p->next->datos)->address);
                    break;
                case mapped:
                    munmap(((block) p->next->datos)->address, ((block) p->next->datos)->size);
                    break;
                case all:
                    break;
            }
        }
        free(p->next->datos);
        free(p);
    }
}
void deleteelem(pos p,List *L){
    pos q;
    if (p == (*L)->next)
    {
        (*L)->next = (*L)->next->next;
    }
    else if (p->next == NULL)
    {
        for (q = *L; q->next->next != NULL; q = q->next);
        q->next = NULL;
    }
    else
    {
        q = p->next;
        p->datos = q->datos;
        p->next = q->next;
        p = q;
    }
    free(p->datos);
    free(p);
}