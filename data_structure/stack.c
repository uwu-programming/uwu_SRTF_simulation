#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

Stack* createStack(){
    Stack* newStack = malloc(sizeof(Stack));

    newStack -> next = NULL;
    newStack -> previous = NULL;
    newStack -> data = NULL;

    return newStack;
}

void pushStack(Stack* stack, size_t elementSize, void** elementPointer){
    linkedListAddNext(stack, elementSize, elementPointer);
}

void popStack(Stack* stack, void** elementPointer){
    if (stack -> next == NULL){
        *elementPointer = NULL;
    }else {
        *elementPointer = stack -> next -> data;
        stack -> next = stack -> next -> next;
    }
}

void peekStack(Stack* stack, void** elementPointer){
    if (stack -> next == NULL){
        *elementPointer = NULL;
    }else {
        *elementPointer = stack -> next -> data;
    }
}