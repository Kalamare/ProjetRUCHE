#include "list.h"

List *createList() {
    List *newList = malloc(sizeof(*newList));
    newList->size = 0;
    return newList;
}

void insertHead(List *list, ListElement *element) {
    element->nextElement = list->firstElement;
    list->firstElement = element;
    list->size++;
}

void insertElement(List *list, ListElement *element) {
    if (list->firstElement == NULL) {
        insertHead(list, element);
        return;
    }

    ListElement *actualElement = list->firstElement;

    while (actualElement->nextElement != NULL) {
        actualElement = actualElement->nextElement;
    }

    actualElement->nextElement = element;
    list->size++;
}

void removeElement(List *list, ListElement *element) {
    if (list->firstElement == NULL) {
        return;
    }
    ListElement *actualElement = list->firstElement;

    if (actualElement == element) {
        list->firstElement = actualElement->nextElement;
        list->size--;
        return;
    }

    while (actualElement->nextElement != NULL) {
        if (actualElement->nextElement == element) {
            actualElement->nextElement = actualElement->nextElement->nextElement;
            list->size--;
            return;
        }

        actualElement = actualElement->nextElement;
    }
}

void freeList(List *list) {
    if (list == NULL) {
        return;
    }
    ListElement *actualElement = list->firstElement;
    ListElement *nextElement;

    while (actualElement != NULL) {
        nextElement = actualElement->nextElement;
        free(actualElement);
        actualElement = nextElement;
    }

    free(list);
}