#ifndef __PROCESSOR__
#define __PROCESSOR__

#include "linked_list.h"
#include "process_scheduler.h"
#include "process.h"

typedef enum PROCESSOR_STATE ProcessorState;
typedef struct Processor Processor;
typedef Node ProcessHistoryList;

enum PROCESSOR_STATE{
    FREE,
    OCCUPIED
};

struct Processor{
    ProcessScheduler* parentScheduler;
    int processorID;

    ProcessorState processorState;
    Process* executingProcess;
    TimeFrame startTime;
    TimeFrame endTime;

    ProcessHistoryList* processHistoryList;
};

Processor* createProcessor(ProcessScheduler* parentScheduler, int processorID);

#endif