#include "process.h"

Process* createProcess(expression infixExpression){
    Process* newProcess = malloc(sizeof(Process));

    newProcess -> processState = NEW;
    newProcess -> dependencyInformation = createDepenencyInformation(infixExpression);
}