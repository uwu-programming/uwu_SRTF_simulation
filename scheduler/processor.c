#include "processor.h"
#include <string.h>
#include <stdio.h>

Processor* createProcessor(ProcessScheduler* parentScheduler, int processorID){
    Processor* newProcessor = malloc(sizeof(Processor));
    
    void* dummyCollector = NULL;

    newProcessor -> parentScheduler = parentScheduler;
    newProcessor -> processorID = processorID;

    newProcessor -> processorState = FREE;

    newProcessor -> executingProcess = NULL;
    newProcessor -> executingExpression = NULL;

    newProcessor -> startTime = 0;
    newProcessor -> endTime = 0;

    newProcessor -> processHistoryList = createNode(sizeof(ProcessHistory*), (void*)(&dummyCollector));

    return newProcessor;
}

void processorExecuteProcessThread(ProcessorExecutionArgument* processorExecutionArgument){    
    Processor* processor = processorExecutionArgument -> processor;
    Process* process = processorExecutionArgument -> process;
    ExpressionInformation* executingExpression = processorExecutionArgument -> executingExpression;
    TimeFrame startTime = processorExecutionArgument -> startTime;
    TimeFrame endTime = processorExecutionArgument -> endTime;
    
    processor -> startTime = startTime;
    processor -> endTime = endTime;
    
    int aValue = executingExpression -> operandAValue;
    int bValue = executingExpression -> operandBValue;

    switch (executingExpression -> expressionOperator){
        case '+':
            executingExpression -> expressionAnswer =  aValue + bValue;
            break;
        case '-':
            executingExpression -> expressionAnswer = aValue - bValue;
            break;
        case '*':
            executingExpression -> expressionAnswer = aValue * bValue;
            break;
        case '/':
            executingExpression -> expressionAnswer = aValue;
            break;
    }

    // lock the executed thread's parent process to modify its data
    pthread_mutex_lock(&(process -> m_processData));
    printf("locked\n");

    (process -> remainingBurstTime)--; // reduce its remaining burst time by 1
    executingExpression -> expressionRepresentation[0] = process -> dependencyInformation -> currentNewVariable[0]; // present the executed expression as a new variable
    process -> dependencyInformation -> currentNewVariable[0] = ((int)(process -> dependencyInformation -> currentNewVariable[0])) + 1;
    printf("deduct burst time\n");

    // update the process' DependencyInformation -  updatedPrefixExpression for presenting the original expression with new operand representation
    expression updatedPrefixExpression = process -> dependencyInformation -> updatedPrefixExpression;
    process -> dependencyInformation -> updatedInfixExpression = malloc(sizeof(char) * (strlen(updatedPrefixExpression) - 2 + 1));
    // check where is the previous expression (operand) at
    int operandALocation = 0, operandBLocation = 0, operatorLocation = 0;
    for (int i = 0; i < strlen(updatedPrefixExpression) - 2; i++){
        if (updatedPrefixExpression[i] == executingExpression -> expressionOperator && updatedPrefixExpression[i+1] == executingExpression -> operandA[0] && updatedPrefixExpression[i+2] == executingExpression -> operandB[0]){
            operatorLocation = i;
            operandALocation = i+1;
            operandBLocation = i+2;
        }
    }
    // copy the previous updatedPrefixExpression and represent the operand with new variable
    for (int i = 0, j = 0; i < strlen(updatedPrefixExpression) - 2; i++){
        if (i == operatorLocation){
            process -> dependencyInformation -> updatedPrefixExpression[i] = executingExpression -> expressionRepresentation[0];
            j += 2;
        }else {
            process -> dependencyInformation -> updatedPrefixExpression[i] = updatedPrefixExpression[i+j];
        }
    }
    process -> dependencyInformation -> updatedPrefixExpression[strlen(updatedPrefixExpression) - 2] = '\0';

    printf("updated prefix: %s\n", process -> dependencyInformation -> updatedPrefixExpression);

    ProcessHistory* newProcessHistory = createProcessHistory(process, executingExpression, startTime, endTime);
    processorAddProcessHistory(processor, newProcessHistory);

    // unlock the process' mutex after finishing modifying the data so other thread can access it
    pthread_mutex_unlock(&(process -> m_processData));
}

void processorAddProcessHistory(Processor* processor, ProcessHistory* processHistory){
    void* dummyCollector = NULL;

    linkedListAddNext(processor -> processHistoryList, sizeof(processHistory), (void*)(&dummyCollector));
    *((ProcessHistory**)(dummyCollector)) = processHistory;
}