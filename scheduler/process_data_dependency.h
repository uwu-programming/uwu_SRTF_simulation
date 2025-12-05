#ifndef __PROCESS_DATA_DEPENDENCY__
#define __PROCESS_DATA_DEPENDENCY__

#include "linked_list.h"
#include "expression_notation.h"

typedef struct DependencyInformation DependencyInformation;

struct DependencyInformation{
    expression infixExpression;

    int dependencyAmount;
    Node* independentCalculationList;
};

expression pickIndependentExpression(expression prefixExpression);

int calculateDependency(expression prefixExpression);

Node* getIndependentCalculation(expression prefixExpression);

#endif