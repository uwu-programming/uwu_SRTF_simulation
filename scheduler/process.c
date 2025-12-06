#include "process.h"

Process* createProcess(expression infixExpression, int processID, TimeFrame arrivalTime){
    Process* newProcess = malloc(sizeof(Process));

    newProcess -> processState = NEW;
    newProcess -> processID = processID;
    newProcess -> dependencyInformation = createDepenencyInformation(infixExpression);

    newProcess -> arrivalTime = arrivalTime;
    newProcess -> initialBurstTime = newProcess -> dependencyInformation -> threadAmount;
    newProcess -> remainingBurstTime = newProcess -> initialBurstTime;
    newProcess -> waitingTime = 0;

    return newProcess;
}