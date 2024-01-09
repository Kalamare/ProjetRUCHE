#include <stdio.h>
#include <stdlib.h>

#ifndef LIST_H
#define LIST_H


typedef struct List List;
typedef struct ListElement ListElement;

struct ListElement {
    void *value;
    ListElement *nextElement;
};

struct List {
    ListElement *firstElement;
    int size;
};

List *createList();

void insertHead(List *list, ListElement *element);

void insertElement(List *list, ListElement *element);

void removeElement(List *list, ListElement *element);

void freeList(List *list);

#endif