#include "process_scheduler.h"
#include "processor.h"
#include <stdio.h>

ProcessScheduler* createProcessScheduler(int processorCoreAmount, MultiThreadingSetting multiThreadingSetting){
    ProcessScheduler* processScheduler = malloc(sizeof(ProcessScheduler));
    
    processScheduler -> currentTimeFrame = 0;

    processScheduler -> processorCoreAmount = processorCoreAmount;
    processScheduler -> multiThreadingSetting = multiThreadingSetting;

    void* dummyCollector = NULL;
    processScheduler -> processorList = createNode(sizeof(Processor*), (void*)(&dummyCollector));

    for (int i = 0; i < processScheduler -> processorCoreAmount; i++){
        Processor* newProcessor = createProcessor(processScheduler, i+1);

        linkedListAddNext(processScheduler -> processorList, sizeof(newProcessor), (void*)(&dummyCollector));
        *((Processor**)(dummyCollector)) = newProcessor;
    }

    processScheduler -> processAmount = 0;
    processScheduler -> processList = createNode(sizeof(Process*), (void*)(&dummyCollector));

    return processScheduler;
}

void addProcess(ProcessScheduler* processSceduler, expression infixExpression){
    Process* newProcess = createProcess(infixExpression, ++(processSceduler -> processAmount), processSceduler -> currentTimeFrame);

    void* dummyCollector = NULL;
    linkedListAddNext(processSceduler -> processList, sizeof(newProcess), (void*)(&dummyCollector));
    *((Process**)(dummyCollector)) = newProcess;
}