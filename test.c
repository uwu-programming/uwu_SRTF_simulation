#include "linked_list.h"
#include "expression_notation.h"
#include "process_data_dependency.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    printf("%s\n", infixToPrefix("a+b*c"));
    printf("%s\n", infixToPrefix("a*b+c"));
    printf("%s\n", infixToPrefix("a*b+c/d"));
    printf("%s\n", infixToPrefix("(a+b)*(c+d)"));
    printf("%s\n", infixToPrefix("((a+b)/(c*d))-(e+f)"));
    printf("%s\n", infixToPrefix("(a+b) * (c-d) / (e+f)"));
    printf("%s\n", infixToPrefix("(e+f)*((a+b)+c)"));
    printf("%s\n", infixToPrefix("ans1+ans2"));

    printf("\n");

    printf("%d\n", calculateDependency(infixToPrefix("a+b*c")));
    printf("%d\n", calculateDependency(infixToPrefix("a*b+c")));
    printf("%d\n", calculateDependency(infixToPrefix("a*b+c/d")));
    printf("%d\n", calculateDependency(infixToPrefix("((a+b)/(c*d))-(e+f)")));
    printf("%d\n", calculateDependency(infixToPrefix("(a+b) * (c-d) / (e+f)")));
    printf("%d\n", calculateDependency(infixToPrefix("(e+f)*((a+b)+c)")));

    printf("\ngetIndependentCalculation:\n");
    Node* calList = getIndependentCalculation(infixToPrefix("(a+b) * (c-d) / (e+f)"));
    Node* dummy = malloc(sizeof(Node*));
    dummy = calList;
    while(dummy -> next != NULL){
        dummy = dummy -> next;
        printf("ex: %s\n", (*(ExpressionInformation**)(dummy -> data)) -> prefixExpression);
    }

    printf("\ndependency information:\n");
    DependencyInformation* dependencyInformation = createDepenencyInformation("(a+b) * (c-d) / (e+f)");
    printf("infix: %s\n", dependencyInformation -> infixExpression);
    printf("prefix: %s\n", dependencyInformation -> prefixExpression);
    printf("thread: %d\n", dependencyInformation -> threadAmount);
    printf("dependency: %d\n", dependencyInformation -> dependencyAmount);
    dummy = dependencyInformation -> independentCalculationList;
    while (dummy -> next != NULL){
        dummy = dummy -> next;
        printf("ex: %s\n", (*(ExpressionInformation**)(dummy -> data)) -> prefixExpression);
    }
}