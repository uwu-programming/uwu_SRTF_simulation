#ifndef __LINKED_LIST__
#define __LINKED_LIST__

#include <stdlib.h>

typedef struct LinkedListNode Node;

struct LinkedListNode{
    Node* next;
    Node* previous;

    void* data;
};

Node* createNode(size_t dataSize, void** dataPointer);

void linkedListAddNext(Node* addAt, size_t dataSize, void** dataPointer);

void linkedListAddPrevious(Node* addAt, size_t dataSize, void** dataPointer);

#endif