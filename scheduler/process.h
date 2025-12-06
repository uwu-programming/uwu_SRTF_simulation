#ifndef __PROCESS__
#define __PROCESS__

#include "process_data_dependency.h"

typedef enum PROCESS_STATE ProcessState;
typedef struct Process Process;
typedef struct ProcessHistory ProcessHistory;
typedef int ProcessTime;
typedef int TimeFrame;

enum PROCESS_STATE{
    NEW,        // newly created process, haven't done data dependency and thread calculate
    READY,      // done dependency and thread check, no waiting dependency and ready to get assigned to processor
    RUNNING,    // the process is running
    WAITING,    // the process is waiting for dependency data
    TERMINATED  // the process has completed
};

struct Process{
    ProcessState processState;
    int processID;

    // the information of the process (the expression's information)
    DependencyInformation* dependencyInformation;
    TimeFrame arrivalTime;
    ProcessTime initialBurstTime;
    ProcessTime remainingBurstTime;
    ProcessTime waitingTime;
};

struct ProcessHistory{
    Process* executedProcess;

    expression executedExpression;
    
    TimeFrame timeStart;
    TimeFrame timeEnd;
};

Process* createProcess(expression infixExpression, int processID, TimeFrame arrivalTime);

#endif