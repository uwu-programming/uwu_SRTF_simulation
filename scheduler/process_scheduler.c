#include "process_scheduler.h"
#include "processor.h"
#include "expression_notation.h"
#include <stdio.h>

ProcessScheduler* createProcessScheduler(int processorCoreAmount, MultiThreadingSetting multiThreadingSetting){
    ProcessScheduler* processScheduler = malloc(sizeof(ProcessScheduler));
    
    processScheduler -> currentTimeFrame = 0;

    processScheduler -> processorCoreAmount = processorCoreAmount;
    processScheduler -> multiThreadingSetting = multiThreadingSetting;

    void* dummyCollector = NULL;
    processScheduler -> processorList = createNode(sizeof(Processor*), (void*)(&dummyCollector));

    for (int i = processorCoreAmount; i > 0; i--){
        Processor* newProcessor = createProcessor(processScheduler, i);

        linkedListAddNext(processScheduler -> processorList, sizeof(newProcessor), (void*)(&dummyCollector));
        *((Processor**)(dummyCollector)) = newProcessor;
    }

    processScheduler -> processAmount = 0;
    processScheduler -> processList = createNode(sizeof(Process*), (void*)(&dummyCollector));

    return processScheduler;
}

void addProcess(ProcessScheduler* processScheduler, expression infixExpression){
    Process* newProcess = createProcess(infixExpression, ++(processScheduler -> processAmount), processScheduler -> currentTimeFrame);

    void* dummyCollector = NULL;
    linkedListAddNext(processScheduler -> processList, sizeof(newProcess), (void*)(&dummyCollector));
    *((Process**)(dummyCollector)) = newProcess;
}

void sortProcessPriority(ProcessScheduler* processScheduler){
    void* dummyI = NULL;
    void* dummyJ = NULL;
    void* dummyCollector = NULL;

    TimeFrame arrivalTime;
    ProcessTime hoveringBurstTime;

    dummyI = processScheduler -> processList;
    while (((Node*)dummyI) -> next != NULL){
        dummyI = ((Node*)dummyI) -> next;
        dummyJ = dummyI;

        hoveringBurstTime = (*(Process**)(((Node*)dummyI) -> data)) -> remainingBurstTime;
        while (((Node*)dummyJ) -> next != NULL){
            dummyJ = ((Node*)dummyJ) -> next;

            if ((*(Process**)(((Node*)dummyJ) -> data)) -> remainingBurstTime < hoveringBurstTime){
                ((Node*)dummyJ) -> previous -> next = ((Node*)dummyJ) -> next;
                ((Node*)dummyJ) -> next -> previous = ((Node*)dummyJ) -> previous;

                ((Node*)dummyI) -> previous -> next = (Node*)dummyJ;
                ((Node*)dummyJ) -> next = ((Node*)dummyI);
                ((Node*)dummyJ) -> previous = ((Node*)dummyI) -> previous;
                ((Node*)dummyI) -> previous = (Node*)dummyJ;
            }
        }
    }

    dummyI = processScheduler -> processList;
    while (((Node*)dummyI) -> next != NULL){
        dummyI = ((Node*)dummyI) -> next;
        dummyJ = dummyI;

        arrivalTime = (*(Process**)(((Node*)dummyI) -> data)) -> arrivalTime;
        while (((Node*)dummyJ) -> next != NULL){
            dummyJ = ((Node*)dummyJ) -> next;

            if ((*(Process**)(((Node*)dummyJ) -> data)) -> arrivalTime < arrivalTime && (*(Process**)(((Node*)dummyJ) -> data)) -> remainingBurstTime <= (*(Process**)(((Node*)dummyI) -> data)) -> remainingBurstTime){
                ((Node*)dummyJ) -> previous -> next = ((Node*)dummyJ) -> next;
                ((Node*)dummyJ) -> next -> previous = ((Node*)dummyJ) -> previous;

                ((Node*)dummyI) -> previous -> next = (Node*)dummyJ;
                ((Node*)dummyJ) -> next = ((Node*)dummyI);
                ((Node*)dummyJ) -> previous = ((Node*)dummyI) -> previous;
                ((Node*)dummyI) -> previous = (Node*)dummyJ;
            }
        }
    }
}

void processSchedulerNextTimeframe(ProcessScheduler* processScheduler){
    // lock the process scheduler's data and sort the priority of its processes
    pthread_mutex_lock(&(processScheduler -> m_processSchedulerData));

    sortProcessPriority(processScheduler);
    
    Node* dummyProcessNode = NULL;
    Node* dummyProcessorNode = NULL;
    dummyProcessNode = processScheduler -> processList;
    dummyProcessorNode = processScheduler -> processorList;

    // test print
    // while(dummyProcessorNode->next != NULL){
    //     dummyProcessorNode = dummyProcessorNode -> next;
    //     printf("core: %d\n", (*(Processor**)(dummyProcessorNode -> data)) -> processorID);
    // }

    // check if there is any process that need to be executed, and if there is available processor for the process
    while (dummyProcessNode -> next != NULL && dummyProcessorNode -> next != NULL){
        dummyProcessNode = dummyProcessNode -> next; // get first process among process list

        if ((*(Process**)(dummyProcessNode -> data)) -> processState != TERMINATED){
            Process* process = (*(Process**)(dummyProcessNode -> data));
            DependencyInformation* processDependency = (*(Process**)(dummyProcessNode -> data)) -> dependencyInformation;
            Node* independentCalculationNode = processDependency -> independentCalculationList;

            while(independentCalculationNode -> next != NULL){
                printf("new calculation\n");

                independentCalculationNode = independentCalculationNode -> next;
                ExpressionInformation* executingExpression = *(ExpressionInformation**)(independentCalculationNode -> data);
                printf("expression: %s\n", executingExpression -> prefixExpression);

                if (dummyProcessorNode -> next != NULL){
                    // remove the executed expression from the process' DependencyInformation's independentCalculationList
                    independentCalculationNode -> previous -> next = independentCalculationNode -> next;
                    if (independentCalculationNode -> next != NULL)
                        independentCalculationNode -> next -> previous = independentCalculationNode -> previous;

                    dummyProcessorNode = dummyProcessorNode -> next;
                    ProcessorExecutionArgument* processorExecutionArgument = malloc(sizeof(ProcessorExecutionArgument));
                    processorExecutionArgument -> processor = *((Processor**)(dummyProcessorNode -> data));
                    processorExecutionArgument -> process = (*(Process**)(dummyProcessNode -> data));
                    processorExecutionArgument -> executingExpression = executingExpression;
                    processorExecutionArgument -> startTime = processScheduler -> currentTimeFrame;
                    processorExecutionArgument -> endTime = processScheduler -> currentTimeFrame + 1;

                    processorExecuteProcessThread(processorExecutionArgument);
                }
                printf("done executed\n");
            }
        }
    }

    pthread_mutex_unlock(&(processScheduler -> m_processSchedulerData));
}