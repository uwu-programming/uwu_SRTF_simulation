#include <linked_list.h>
#include <stdio.h>
#include <stdlib.h>

void main(){
    int* placeholder;

    Node* mynode = createNode(
        malloc(sizeof(int)),
        (void*)(&placeholder)
    );

    linkedListAddNext(malloc(sizeof(int)), mynode, (void*)(&placeholder));
    *placeholder = 100;

    linkedListAddNext(malloc(sizeof(int)), mynode, (void*)(&placeholder));
    *placeholder = 10;

    linkedListAddNext(malloc(sizeof(int)), mynode, (void*)(&placeholder));
    *placeholder = 50;

    linkedListAddNext(malloc(sizeof(int)), mynode, (void*)(&placeholder));
    *placeholder = 3;

    Node* test = malloc(sizeof(Node));
    test = mynode;

    while (test -> data != NULL || test -> next != NULL){
        printf("%d\n", *((int*)(test -> data)));
        test = test -> next;
    }
}