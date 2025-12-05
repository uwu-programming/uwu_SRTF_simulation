#include "process_data_dependency.h"
#include <string.h>
#include <ctype.h>

int calculateDependency(expression prefixExpression){
    int expressionLength = strlen(prefixExpression);
    int operatorAmount = 0;
    int independentCalculation = 0;

    for (int i = 0; i < expressionLength - 3;){
        if (!isalnum(prefixExpression[i++])){
            operatorAmount++;
            if (isalnum(prefixExpression[i++]))
                if (isalnum(prefixExpression[i++])) 
                    independentCalculation++;
                else
                    i--;
            else
                i--;
        }
    }

    return operatorAmount - independentCalculation;
}