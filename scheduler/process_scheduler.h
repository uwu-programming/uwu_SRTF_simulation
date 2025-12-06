#ifndef __PROCESS_SCHEDULER__
#define __PROCESS_SCHEDULER__

#include "linked_list.h"
#include "process.h"

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
    TimeFrame currentTimeFrame;

    int processorCoreAmount;
    MultiThreadingSetting multiThreadingSetting;

    ProcessorList* processorList;
    int processAmount;
    ProcessList* processList;
};

ProcessScheduler* createProcessScheduler();

void addProcess(ProcessScheduler* processSceduler, expression infixExpression);

void sortProcessPriority(ProcessScheduler* ProcessScheduler);

#endif