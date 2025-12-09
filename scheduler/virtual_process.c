#include "virtual_process.h"

Process* createProcess(expression infixExpression, int processID, TimeFrame arrivalTime){
    Process* newProcess = malloc(sizeof(Process));

    newProcess -> processState = NEW;
    newProcess -> processID = processID;
    newProcess -> dependencyInformation = createDepenencyInformation(infixExpression);

    newProcess -> arrivalTime = arrivalTime;
    newProcess -> completionTime = 0;
    newProcess -> initialBurstTime = newProcess -> dependencyInformation -> threadAmount;
    newProcess -> remainingBurstTime = newProcess -> initialBurstTime;
    newProcess -> waitingTime = 0;

    return newProcess;
}

ProcessHistory* createProcessHistory(Process* executedProcess, ExpressionInformation* executedExpression, TimeFrame timeStart, TimeFrame timeEnd){
    ProcessHistory* newProcessHistory = malloc(sizeof(ProcessHistory));

    newProcessHistory -> executedProcess = executedProcess;
    newProcessHistory -> executedExpression = executedExpression;
    newProcessHistory -> timeStart = timeStart;
    newProcessHistory -> timeEnd = timeEnd;
}