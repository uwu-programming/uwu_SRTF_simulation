#ifndef __LINKED_LIST__
#define __LINKED_LIST__

typedef struct LinkedListNode Node;

struct LinkedListNode{
    Node* next;
    Node* previous;

    void* data;
};

Node* createNode(void* data, void** dataPointer);

void linkedListAddNext(void* data, Node* addAt, void** dataPointer);

void linkedListAddPrevious(void* data, Node* addAt, void** dataPointer);

#endif