#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

Node* createNode(void* data, void** dataPointer){
    Node* newNode = malloc(sizeof(Node));

    newNode -> next = NULL;
    newNode -> previous = NULL;
    newNode -> data = data;
    *dataPointer = (newNode -> data);

    return newNode;
}

void linkedListAddNext(void* data, Node* addAt, void** dataPointer){
    Node* newNode = malloc(sizeof(Node));

    newNode -> data = data;
    
    if (addAt -> next == NULL){
        addAt -> next = newNode;
    }else {
        newNode -> next = addAt -> next;
        addAt -> next = newNode;
    }

    *dataPointer = (newNode -> data);
}

void linkedListAddPrevious(void* data, Node* addAt, void** dataPointer){

}