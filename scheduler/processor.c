#include "processor.h"

Processor* createProcessor(ProcessScheduler* parentScheduler, int processorID){
    Processor* newProcessor = malloc(sizeof(Processor));
    
    void* dummyCollector = NULL;

    newProcessor -> parentScheduler = parentScheduler;
    newProcessor -> processorID = processorID;

    newProcessor -> processorState = FREE;
    newProcessor -> executingProcess = NULL;
    newProcessor -> startTime = 0;
    newProcessor -> endTime = 0;

    newProcessor -> processHistoryList = createNode(sizeof(ProcessHistory*), (void*)(&dummyCollector));

    return newProcessor;
}