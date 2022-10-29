

#include "list.h"

List createList() {
    struct node *N = malloc(sizeof(struct node));
    N->next = NULL;
    return N;
}
bool isEmptyList(List L) {
    if (L->next == NULL)
        return true;
    else return false;
}

void insert(List *l, void* data) {
    pos p = *l;
    while (p->next != NULL)
        p = p->next;
    p->next = malloc(sizeof(struct node));
    p->next->next = NULL;
    p->next->datos = data;
}

pos first(List l) {
    return l;
}

pos next(List L, pos p) {
    return p->next;
}

void deleteList(List *L) {
    pos p;
    while (!isEmptyList(*L)) {
        p = *L;
        *L = (*L)->next;
        free(p);
    }
}
void deleteelem(pos p,List *L){
    pos q;

    if (p == (*L)->next)
    { // Delete first element
        (*L)->next = (*L)->next->next;
    }
    else if (p->next == NULL)
    { //Delete last element
        for (q = *L; q->next->next != NULL; q = q->next)
            ;
        q->next = NULL;
    }
    else
    { //Middle deletion Overwrite p with q
        //p--> element we want to delete
        //q-->next element

        q = p->next;
        p->datos = q->datos;
        p->next = q->next;
        p = q;
    }

    free(p);
}