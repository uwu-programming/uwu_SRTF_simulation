#ifndef __STACK__
#define __STACK__

#include "linked_list.h"

typedef Node Stack;
typedef Node StackElement;

Stack* createStack();

void pushStack(Stack* stack, size_t elementSize, void** elementPointer);

void popStack(Stack* stack, void** elementPointer);

void peekStack(Stack* stack, void** elementPointer);

#endif