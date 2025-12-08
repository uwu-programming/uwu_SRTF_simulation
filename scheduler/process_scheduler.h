#ifndef __PROCESS_SCHEDULER__
#define __PROCESS_SCHEDULER__

#include "linked_list.h"
#include "virtual_process.h"
#include <pthread.h>

typedef enum MULTI_THREADING_SETTING MultiThreadingSetting;
typedef Node ProcessList;
typedef Node ProcessorList;
typedef struct ProcessScheduler ProcessScheduler;

extern TimeFrame timeFrame;

enum MULTI_THREADING_SETTING{
    OFF,
    ON
};

struct ProcessScheduler{
    // mutex for protecting ProcessScheduler's data being overwritten by other function when it is accessing the data itself
    pthread_mutex_t m_processSchedulerData;

    TimeFrame currentTimeFrame;

    int processorCoreAmount;
    MultiThreadingSetting multiThreadingSetting;

    ProcessorList* processorList;
    Node* processorThreadList;

    int processAmount;
    ProcessList* processList;
};

ProcessScheduler* createProcessScheduler(int processorCoreAmount, MultiThreadingSetting multiThreadingSetting);

void addProcess(ProcessScheduler* processScheduler, expression infixExpression);

void sortProcessPriority(ProcessScheduler* processScheduler);

void processSchedulerNextTimeframe(ProcessScheduler* processScheduler);

#endif