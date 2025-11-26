#include "linked_list.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

void main(){
    int* placeholder;

    Node* mynode = createNode(
        sizeof(int),
        (void*)(&placeholder)
    );

    linkedListAddNext(mynode, sizeof(int), (void*)(&placeholder));
    *placeholder = 100;

    linkedListAddNext(mynode, sizeof(int), (void*)(&placeholder));
    *placeholder = 10;

    linkedListAddNext(mynode, sizeof(int), (void*)(&placeholder));
    *placeholder = 50;

    linkedListAddNext(mynode, sizeof(int), (void*)(&placeholder));
    *placeholder = 3;

    Node* test = malloc(sizeof(Node));
    test = mynode;

    while (test -> next != NULL){
        if (test -> data != NULL)
            printf("%d\n", *((int*)(test -> data)));
        test = test -> next;
    }

    printf("\nstack:\n");

    Stack* stack = createStack();
    pushStack(stack, sizeof(int), (void*)(&placeholder));
    *placeholder = 1;
    pushStack(stack, sizeof(int), (void*)(&placeholder));
    *placeholder = 2;
    pushStack(stack, sizeof(int), (void*)(&placeholder));
    *placeholder = 3;
    pushStack(stack, sizeof(int), (void*)(&placeholder));
    *placeholder = 4;
    pushStack(stack, sizeof(int), (void*)(&placeholder));
    *placeholder = 5;

    popStack(stack, (void*)(&placeholder));
    printf("pop: %d\n", *placeholder);
    popStack(stack, (void*)(&placeholder));
    printf("pop: %d\n", *placeholder);
    popStack(stack, (void*)(&placeholder));
    printf("pop: %d\n", *placeholder);
    popStack(stack, (void*)(&placeholder));
    printf("pop: %d\n", *placeholder);
    popStack(stack, (void*)(&placeholder));
    printf("pop: %d\n", *placeholder);
}