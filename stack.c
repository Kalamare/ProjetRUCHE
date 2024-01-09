#include "stack.h"
#include "world.h"

Stack *createStack() {
    Stack *newStack = malloc(sizeof(*newStack));
    newStack->elementsCount = 0;
    return newStack;
}

void push(Stack *stack, StackElement *element) {
    if (stack->firstElement == NULL) {
        stack->firstElement = element;
        stack->elementsCount++;
        return;
    }

    element->nextElement = stack->firstElement;
    stack->firstElement = element;
    stack->elementsCount++;
}

StackElement *pop(Stack *stack) {
    if (stack->firstElement == NULL) {
        return NULL;
    }

    StackElement *element = stack->firstElement;
    stack->firstElement = element->nextElement;
    stack->elementsCount--;
    return element;
}

Stack *pushAll(Stack *stack, Stack *stackToPush) {
    if (stack == NULL) {
        Stack *newStack = createStack();
        return pushAll(newStack, stackToPush);
    }
    if (stackToPush == NULL || stackToPush->firstElement == NULL) {
        return stack;
    }
    StackElement *actualElement = stackToPush->firstElement;
    StackElement *nextElement;

    while (actualElement != NULL) {
        nextElement = actualElement->nextElement;
        push(stack, actualElement);
        //printf("Pushing %d, %d\n", ((Location *) actualElement->value)->x, ((Location *) actualElement->value)->y);
        actualElement = nextElement;
    }

    return stack;
}

bool isStackEmpty(Stack *stack) {
    return stack->elementsCount == 0;
}

void freeStack(Stack *stack) {
    if (stack == NULL) {
        return;
    }
    StackElement *actualElement = stack->firstElement;
    StackElement *nextElement;

    while (actualElement != NULL) {
        nextElement = actualElement->nextElement;
        free(actualElement);
        actualElement = nextElement;
    }

    free(stack);
}