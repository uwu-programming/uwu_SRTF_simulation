#include "virtual_process.h"
#include <stdio.h>

Process* createProcess(expression infixExpression, int processID, TimeFrame arrivalTime){
    Process* newProcess = malloc(sizeof(Process));

    newProcess -> processState = NEW;
    newProcess -> processID = processID;
    newProcess -> dependencyInformation = createDepenencyInformation(infixExpression);

    newProcess -> arrivalTime = arrivalTime;
    newProcess -> completionTime = -1;
    newProcess -> initialBurstTime = newProcess -> dependencyInformation -> threadAmount;
    newProcess -> remainingBurstTime = newProcess -> initialBurstTime;
    
    newProcess -> responseTime = -1;
    newProcess -> waitingTime = 0;
    newProcess -> turnaroundTime = -1;

    return newProcess;
}

ProcessHistory* createProcessHistory(Process* executedProcess, ExpressionInformation* executedExpression, TimeFrame timeStart, TimeFrame timeEnd){
    ProcessHistory* newProcessHistory = malloc(sizeof(ProcessHistory));

    newProcessHistory -> executedProcess = executedProcess;

    newProcessHistory -> executedExpression = malloc(sizeof(ExpressionInformation));
    newProcessHistory -> executedExpression -> prefixExpression = executedExpression -> prefixExpression;
    newProcessHistory -> executedExpression -> operandA = executedExpression -> operandA;
    newProcessHistory -> executedExpression -> operandB = executedExpression -> operandB;
    newProcessHistory -> executedExpression -> expressionOperator = executedExpression -> expressionOperator;
    newProcessHistory -> executedExpression -> operandAValue = executedExpression -> operandAValue;
    newProcessHistory -> executedExpression -> operandBValue = executedExpression -> operandBValue;
    newProcessHistory -> executedExpression -> expressionAnswer = executedExpression -> expressionAnswer;
    newProcessHistory -> executedExpression -> operandOverwrite = executedExpression -> operandOverwrite;
    newProcessHistory -> executedExpression -> expressionRepresentation = executedExpression -> expressionRepresentation;
    
    newProcessHistory -> timeStart = timeStart;
    newProcessHistory -> timeEnd = timeEnd;

    return newProcessHistory;
}