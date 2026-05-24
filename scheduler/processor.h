#ifndef __PROCESSOR__
#define __PROCESSOR__

#include "linked_list.h"
#include "expression_notation.h"
#include "process_scheduler.h"
#include "virtual_process.h"
#include <pthread.h>

typedef enum PROCESSOR_STATE ProcessorState;
typedef struct Processor Processor;
typedef Node ProcessHistoryList;
typedef struct ProcessorExecutionArgument ProcessorExecutionArgument;

enum PROCESSOR_STATE{
    FREE,
    OCCUPIED
};

struct Processor{
    // mutex for protecting Processor's data being overwritten by other thread when it is accessing the data itself
    pthread_mutex_t m_processorData;

    ProcessScheduler* parentScheduler;
    int processorID;

    ProcessorState processorState;
    // Process* for identifying a process to be executed (primary to get its processID for documenting purpose)
    Process* executingProcess;
    // ExpressionInformation* for getting an actual expression to calculate from [process -> dependencyInformation -> independentCalculationList]
    ExpressionInformation* executingExpression;
    TimeFrame startTime;
    TimeFrame endTime;

    ProcessHistoryList* processHistoryList;
};

struct ProcessorExecutionArgument{
    Processor* processor;
    Process* process;
    ExpressionInformation* executingExpression;
    TimeFrame startTime;
    TimeFrame endTime;
};

Processor* createProcessor(ProcessScheduler* parentScheduler, int processorID);

void processorExecuteProcessThread(void* processorExecutionArgument);

void processorAddProcessHistory(Processor* processor, ProcessHistory* processHistory);

#endif