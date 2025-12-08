#ifndef __PROCESS_DATA_DEPENDENCY__
#define __PROCESS_DATA_DEPENDENCY__

#include "linked_list.h"
#include "expression_notation.h"

typedef struct DependencyInformation DependencyInformation;

struct DependencyInformation{
    expression infixExpression;
    expression prefixExpression;
    expression updatedInfixExpression;
    expression updatedPrefixExpression;

    operand currentNewVariable;

    int threadAmount;
    int dependencyAmount;

    Node* independentCalculationList;
    Node* solvedIndependentCalculationList;
};

expression pickIndependentExpression(expression prefixExpression);

int calculateThread(expression prefixExpression);

int calculateDependency(expression prefixExpression);

Node* getIndependentCalculation(expression prefixExpression);

DependencyInformation* createDepenencyInformation(expression infixExpression);

#endif