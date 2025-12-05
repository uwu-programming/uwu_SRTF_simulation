#include "process_data_dependency.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

expression pickIndependentExpression(expression prefixExpression){
    expression independentExpression = NULL;

    if (!isalnum(prefixExpression[0]) && (isalnum(prefixExpression[1]) && isalnum(prefixExpression[2]))){
        independentExpression = malloc(sizeof(char) * 4);
        for (int i = 0; i < 3; i++){
            independentExpression[i] = prefixExpression[i];
        }
        independentExpression[3] = '\0';
    }

    return independentExpression;
}

int calculateDependency(expression prefixExpression){
    int expressionLength = strlen(prefixExpression);
    int operatorAmount = 0;
    int independentCalculation = 0;

    for (int i = 0; i < expressionLength - 2; i++){
        if (pickIndependentExpression(&(prefixExpression[i])) != NULL)
            independentCalculation++;

        if (!isalnum(prefixExpression[i]))
            operatorAmount++;
    }

    return operatorAmount - independentCalculation;
}

Node* getIndependentCalculation(expression prefixExpression){
    void* dummyCollector = NULL;

    Node* independentCalculationList = createNode(sizeof(ExpressionInformation), (void*)(&dummyCollector));

    int expressionLength = strlen(prefixExpression);
    expression independentExpression = NULL;

    for (int i = 0; i < expressionLength - 2; i++){
        independentExpression = pickIndependentExpression(&(prefixExpression[i]));
        if (independentExpression != NULL){
            ExpressionInformation* expressionInformation = createExpressionInformation(independentExpression);

            linkedListAddNext(independentCalculationList, sizeof(ExpressionInformation**), (void*)(&dummyCollector));
            
            *((ExpressionInformation**)dummyCollector) = expressionInformation;
        }
    }

    return independentCalculationList;
}