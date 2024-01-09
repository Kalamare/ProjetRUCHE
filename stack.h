#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef STACK_H
#define STACK_H

typedef struct Stack Stack;
typedef struct StackElement StackElement;

struct StackElement {
    void *value;
    StackElement *nextElement;
};

struct Stack {
    StackElement *firstElement;
    int elementsCount;
};

Stack *createStack();

StackElement *pop(Stack *stack);

void push(Stack *stack, StackElement *element);

Stack* pushAll(Stack *stack, Stack *stackToPush);

void freeStack(Stack *stack);

bool isStackEmpty(Stack *stack);

#endif