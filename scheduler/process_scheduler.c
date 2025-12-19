#include "process_scheduler.h"
#include "processor.h"
#include "expression_notation.h"
#include <stdio.h>

ProcessScheduler* createProcessScheduler(int processorCoreAmount, MultiThreadingSetting multiThreadingSetting){
    ProcessScheduler* processScheduler = malloc(sizeof(ProcessScheduler));
    
    processScheduler -> m_processSchedulerData = (pthread_t)(pthread_t*)(malloc(sizeof(pthread_t)));

    processScheduler -> currentTimeFrame = 0;

    processScheduler -> processorCoreAmount = processorCoreAmount;
    processScheduler -> multiThreadingSetting = multiThreadingSetting;

    void* dummyCollector = NULL;
    processScheduler -> processorList = createNode(sizeof(Processor*), (void*)(&dummyCollector));
    processScheduler -> processorThreadList = createNode(sizeof(pthread_t), (void*)(&dummyCollector));

    // create processor and thread for processor
    for (int i = processorCoreAmount; i > 0; i--){
        Processor* newProcessor = createProcessor(processScheduler, i);

        linkedListAddNext(processScheduler -> processorList, sizeof(newProcessor), (void*)(&dummyCollector));
        *((Processor**)(dummyCollector)) = newProcessor;

        pthread_t* newProcessorThread =  malloc(sizeof(pthread_t));
        linkedListAddNext(processScheduler -> processorThreadList, sizeof(newProcessorThread), (void*)(&dummyCollector));
        *((pthread_t**)(dummyCollector)) = newProcessorThread;
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

            if ((*(Process**)(((Node*)dummyJ) -> data)) -> remainingBurstTime < hoveringBurstTime && (*(Process**)(((Node*)dummyJ) -> data)) -> processState != TERMINATED){
                ((Node*)dummyJ) -> previous -> next = ((Node*)dummyJ) -> next;

                if (((Node*)dummyJ) -> next != NULL)
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
    //pthread_mutex_lock(&(processScheduler -> m_processSchedulerData));

    // check if there is any process that has already been completed and switch the process state to TERMINATED
    Node* terminatedCheckProcessNode = processScheduler -> processList;

    while (terminatedCheckProcessNode -> next != NULL){
        terminatedCheckProcessNode = terminatedCheckProcessNode -> next;
        if ((*(Process**)(terminatedCheckProcessNode -> data)) -> processState != TERMINATED && (*(Process**)(terminatedCheckProcessNode -> data)) -> dependencyInformation -> threadAmount <= 0){
            (*(Process**)(terminatedCheckProcessNode -> data)) -> processState = TERMINATED;
            (*(Process**)(terminatedCheckProcessNode -> data)) -> completionTime = processScheduler -> currentTimeFrame;
            (*(Process**)(terminatedCheckProcessNode -> data)) -> turnaroundTime = processScheduler -> currentTimeFrame - (*(Process**)(terminatedCheckProcessNode -> data)) -> arrivalTime;
        }else if ((*(Process**)(terminatedCheckProcessNode -> data)) -> processState == RUNNING)
            (*(Process**)(terminatedCheckProcessNode -> data)) -> processState = READY;
    }

    sortProcessPriority(processScheduler);

    Node* dummyProcessNode = NULL;
    Node* dummyProcessorNode = NULL;
    Node* dummyProcessorThreadNode = NULL;
    Node* dummyProcessorThreadNode2 = NULL;
    int processorUsed = 0;
    dummyProcessNode = processScheduler -> processList;
    dummyProcessorNode = processScheduler -> processorList;
    dummyProcessorThreadNode = processScheduler -> processorThreadList;
    dummyProcessorThreadNode2 = processScheduler -> processorThreadList;

    // check if there is any process that need to be executed, and if there is available processor for the process
    while (dummyProcessNode -> next != NULL && dummyProcessorNode -> next != NULL){
        dummyProcessNode = dummyProcessNode -> next; // get first process among process list

        if ((*(Process**)(dummyProcessNode -> data)) -> processState != TERMINATED){
            Process* process = (*(Process**)(dummyProcessNode -> data));
            DependencyInformation* processDependency = (*(Process**)(dummyProcessNode -> data)) -> dependencyInformation;
            Node* independentCalculationNode = processDependency -> independentCalculationList;

            while(independentCalculationNode -> next != NULL){
                independentCalculationNode = independentCalculationNode -> next;
                ExpressionInformation* executingExpression = *(ExpressionInformation**)(independentCalculationNode -> data);

                if (dummyProcessorNode -> next != NULL){
                    // remove the executed expression from the process' DependencyInformation's independentCalculationList
                    independentCalculationNode -> previous -> next = independentCalculationNode -> next;
                    if (independentCalculationNode -> next != NULL)
                        independentCalculationNode -> next -> previous = independentCalculationNode -> previous;

                    dummyProcessorNode = dummyProcessorNode -> next;
                    processorUsed++;
                    ProcessorExecutionArgument* processorExecutionArgument = malloc(sizeof(ProcessorExecutionArgument));
                    processorExecutionArgument -> processor = *((Processor**)(dummyProcessorNode -> data));
                    processorExecutionArgument -> process = (*(Process**)(dummyProcessNode -> data));
                    processorExecutionArgument -> executingExpression = executingExpression;
                    processorExecutionArgument -> startTime = processScheduler -> currentTimeFrame;
                    processorExecutionArgument -> endTime = processScheduler -> currentTimeFrame + 1;

                    // create a thread for processor to run the thread execution simulation
                    dummyProcessorThreadNode = dummyProcessorThreadNode -> next;
                    //pthread_create(*((pthread_t**)(dummyProcessorThreadNode -> data)), NULL, (void*)(processorExecuteProcessThread), (void*)(processorExecutionArgument));
                    processorExecuteProcessThread((void*) processorExecutionArgument);
                }
            }
        }
    }

    // add waiting time for processes who didn't get assigned to a processor
    while (dummyProcessNode -> next != NULL){
        dummyProcessNode = dummyProcessNode -> next;
        if ((*(Process**)(dummyProcessNode -> data)) -> processState != TERMINATED)
            (*(Process**)(dummyProcessNode -> data)) -> waitingTime++;
    }

    // for (int i = 0; i < processorUsed; i++){
    //     dummyProcessorThreadNode2 = dummyProcessorThreadNode2 -> next;
    //     printf("try join: %d\n", **((pthread_t**)(dummyProcessorThreadNode2 -> data)));
    //     pthread_join(**((pthread_t**)(dummyProcessorThreadNode2 -> data)), NULL);
    // }

    processScheduler -> currentTimeFrame++;
    //pthread_mutex_unlock(&(processScheduler -> m_processSchedulerData));
}

AverageTime calculateAverageResponseTime(ProcessScheduler* processScheduler){
    int responseAmount = 0;
    ProcessTime totalTime = 0;

    ProcessList* dummyProcessList = processScheduler -> processList;

    while (dummyProcessList -> next != NULL){
        dummyProcessList = dummyProcessList -> next;

        Process* dummyProcess = *(Process**)(dummyProcessList -> data);
        if (dummyProcess -> responseTime >= 0){
            totalTime += dummyProcess -> responseTime;
            responseAmount++;
        }
    }

    processScheduler -> averageResponseTime =  (responseAmount > 0 ? (double)(totalTime) / (double)(responseAmount) : 0.0);
    return (responseAmount > 0 ? (double)(totalTime) / (double)(responseAmount) : 0.0);
}

AverageTime calculateAverageWaitingTime(ProcessScheduler* processScheduler){
    int waitingAmount = 0;
    ProcessTime totalTime = 0;

    ProcessList* dummyProcessList = processScheduler -> processList;

    while (dummyProcessList -> next != NULL){
        dummyProcessList = dummyProcessList -> next;

        Process* dummyProcess = *(Process**)(dummyProcessList -> data);
        totalTime += dummyProcess -> waitingTime;
        waitingAmount++;
    }

    processScheduler -> averageWaitingTime = (waitingAmount > 0 ? (double)(totalTime) / (double)(waitingAmount) : 0.0);
    return (waitingAmount > 0 ? (double)(totalTime) / (double)(waitingAmount) : 0.0);
}

AverageTime calculateAverageTurnaroundTime(ProcessScheduler* processScheduler){
    int turnaroundAmount = 0;
    ProcessTime totalTime = 0;

    ProcessList* dummyProcessList = processScheduler -> processList;

    while (dummyProcessList -> next != NULL){
        dummyProcessList = dummyProcessList -> next;

        Process* dummyProcess = *(Process**)(dummyProcessList -> data);
        if (dummyProcess -> turnaroundTime >= 0){
            totalTime += dummyProcess -> turnaroundTime;
            turnaroundAmount++;
        }
    }

    processScheduler -> averageTurnaroundTime = (turnaroundAmount > 0 ? (double)(totalTime) / (double)(turnaroundAmount) : 0.0);
    return (turnaroundAmount > 0 ? (double)(totalTime) / (double)(turnaroundAmount) : 0.0);
}