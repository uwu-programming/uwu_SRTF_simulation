#ifndef __PROCESS_DATA_DEPENDENCY__
#define __PROCESS_DATA_DEPENDENCY__

#include "linked_list.h"
#include "expression_notation.h"

struct DependencyInformation{
    expression infixExpression;

    int dependencyAmount;
    Node* independentCalculation;
};

int calculateDependency(expression prefixExpression);

Node* calculateIndependentCalculation(expression prefixExpression);

#endif