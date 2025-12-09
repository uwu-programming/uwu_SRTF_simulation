#include "linked_list.h"
#include "expression_notation.h"
#include "process_data_dependency.h"
#include "stack.h"
#include "virtual_process.h"
#include "process_scheduler.h"
#include "processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_t t1, t2;
int a = 0;

void keepPrinting(){
    while(1){
        printf("hewwo %d\n", ++a);
    }
}

void keepPrinting2(){
    while(1){
        printf("uwu %d\n", ++a);
    }
}

void main(){
    int* placeholder;

    Node* mynode = createNode(
        sizeof(int),
        (void*)(&placeholder)
    );

    printf("created node testc\n");

    linkedListAddNext(mynode, sizeof(int), (void*)(&placeholder));
    *placeholder = 100;

    printf("first addnext\n");

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
    printf("%s\n", infixToPrefix("(a+b+c+d)*e"));
    printf("%s\n", infixToPrefix("ans1+ans2"));

    printf("\n");

    printf("%d\n", calculateDependency(infixToPrefix("a+b+c+d+e+f")));
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
    DependencyInformation* dependencyInformation = createDepenencyInformation("(a+b)*(c+d)+x+y");
    printf("infix: %s\n", dependencyInformation -> infixExpression);
    printf("prefix: %s\n", dependencyInformation -> prefixExpression);
    printf("thread: %d\n", dependencyInformation -> threadAmount);
    printf("dependency: %d\n", dependencyInformation -> dependencyAmount);
    dummy = dependencyInformation -> independentCalculationList;
    while (dummy -> next != NULL){
        dummy = dummy -> next;
        printf("ex: %s\n", (*(ExpressionInformation**)(dummy -> data)) -> prefixExpression);
    }

    printf("\nprocess:\n");
    ProcessScheduler* processScheduler = createProcessScheduler(4, ON);
    printf("done create process scheduler\n");

    printf("process scheduler core: %d\n", processScheduler -> processorCoreAmount);
    dummy = processScheduler -> processorList;
    while (dummy -> next != NULL){
        dummy = dummy -> next;
        printf("processor: %d\n", (*((Processor**)(dummy -> data))) -> processorID);
    }
    addProcess(processScheduler, "((a+b)/(c*d))-(e+f)");
    addProcess(processScheduler, "a+b*c");
    addProcess(processScheduler, "x-y");
    addProcess(processScheduler, "x-y+z");
    addProcess(processScheduler, "j+k");
    addProcess(processScheduler, "a*b+c/d");
    addProcess(processScheduler, "a*b+c");

    printf("\nsort:\n");
    sortProcessPriority(processScheduler);
    dummy = processScheduler -> processList;
    while (dummy -> next != NULL){
        dummy = dummy -> next;
        printf("expression: %s\n", (*((Process**)(dummy -> data))) -> dependencyInformation -> prefixExpression);
    }
    printf("i am the best programmer\n");

    // pthread_create(&t1, NULL, (void*)keepPrinting, NULL);
    // pthread_create(&t2, NULL, (void*)keepPrinting2, NULL);
    // pthread_join(t1, NULL);
    // pthread_join(t2, NULL);

    printf("%d?", PTHREAD_NORMAL_MUTEX_INITIALIZER);
    printf("%d?", PTHREAD_NORMAL_MUTEX_INITIALIZER);
    printf("%d?", PTHREAD_NORMAL_MUTEX_INITIALIZER);
    printf("\n");
    
    pthread_mutex_t m1, m2, m3;
    printf("m%p? ", &m1);
    printf("m%p? ", &m2);
    printf("m%p?\n", &m3);

    ProcessScheduler* secondScheduler = createProcessScheduler(2, ON);
    addProcess(secondScheduler, "(x+y)*(m+n)");
    addProcess(secondScheduler, "(x+y)*(k+l)");
    addProcess(secondScheduler, "(x+y)*(i+j)");
    addProcess(secondScheduler, "(x+y)*(g+h)+a+b+c+d+e");
    addProcess(secondScheduler, "(x+y)*(e+f)");
    addProcess(secondScheduler, "q+w");
    addProcess(secondScheduler, "c+d");
    addProcess(secondScheduler, "a+b");
    addProcess(secondScheduler, "a+b+c-d+e");
    addProcess(secondScheduler, "z+x");

    //processSchedulerNextTimeframe(secondScheduler);

    for (int i = 0; i < 20; i++){
        processSchedulerNextTimeframe(secondScheduler);
    }
    // processSchedulerNextTimeframe(secondScheduler);
    // processSchedulerNextTimeframe(secondScheduler);
    // processSchedulerNextTimeframe(secondScheduler);

    printf("done\n");
}