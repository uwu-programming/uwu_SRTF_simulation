#include "linked_list.h"

Node* createNode(size_t dataSize, void** dataPointer){
    Node* newNode = malloc(sizeof(Node));

    newNode -> next = NULL;
    newNode -> previous = NULL;
    newNode -> data = malloc(dataSize);
    *dataPointer = newNode -> data;

    return newNode;
}

void linkedListAddNext(Node* addAt, size_t dataSize, void** dataPointer){
    Node* newNode = malloc(sizeof(Node));

    newNode -> data = malloc(dataSize);
    
    newNode -> previous = addAt;
    newNode -> next = addAt -> next;
    addAt -> next = newNode;

    *dataPointer = newNode -> data;
}

void linkedListAddPrevious(Node* addAt, size_t dataSize, void** dataPointer){
    Node* newNode = malloc(sizeof(Node));

    newNode -> data = malloc(dataSize);

    newNode -> next = addAt;
    newNode -> previous = addAt -> previous;
    addAt -> previous = newNode;

    *dataPointer = newNode -> data;
}