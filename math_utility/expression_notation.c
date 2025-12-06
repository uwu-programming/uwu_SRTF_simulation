#include "expression_notation.h"
#include "stack.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

expression removeSpaces(expression e){
    int spaces = 0;

    for (int i = 0; e[i] != '\0'; i++)
        if (e[i] == ' ')
            spaces++;
    
    char* tempString = malloc(sizeof(char) * (strlen(e) - spaces + 1));

    for (int i = 0, j = 0; e[i] != '\0'; i++){
        if (e[i] != ' ')
            tempString[j++] = e[i];
    }

    tempString[strlen(e) - spaces] = '\0';

    return tempString;
}

expression reverseExpression(expression e){
    int expressionLength = strlen(e);
    char* tempString = malloc(sizeof(char) * (expressionLength + 1));
    
    for (int i = 0; e[i] != '\0'; i++)
        tempString[i] = e[expressionLength - i - 1];

    tempString[expressionLength] = '\0';

    return tempString;
}

expression reverseParenthesis(expression e){
    char* tempString = malloc(sizeof(char) * (strlen(e) + 1));

    for (int i = 0; e[i] != '\0'; i++){
        if (e[i] == '(' || e[i] == ')')
            tempString[i] = (e[i] == '(' ? ')' : '(');
        else
            tempString[i] = e[i];
    }

    tempString[strlen(e)] = '\0';

    return tempString;
}

int checkPrecedence(operator o){
    switch(o){
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '(':
            return 5;
        case ')':
            return -5;
        default:
            return 0;
    }
}

expression infixToPrefix(expression e){
    char* filteredExpression = reverseParenthesis(reverseExpression(removeSpaces(e)));
    char* processedExpression = malloc(sizeof(char) * (strlen(filteredExpression) + 1));
    int filteredExpressionLength = strlen(filteredExpression) + 1;

    Stack* operatorStack = createStack();
    operator* operatorPlaceholder = NULL;
    operator currentOperator = '?';
    
    int i = 0, j = 0;
    for (i; i < filteredExpressionLength; i++){
        if (isalnum(filteredExpression[i])){
            processedExpression[j++] = filteredExpression[i];
        }else {
            currentOperator = filteredExpression[i];
            peekStack(operatorStack, (void*)(&operatorPlaceholder));

            if (operatorPlaceholder == NULL){
                pushStack(operatorStack, sizeof(operator), (void*)(&operatorPlaceholder));
                *operatorPlaceholder = currentOperator;
            }else {
                if (checkPrecedence(currentOperator) >= checkPrecedence(*operatorPlaceholder) || *operatorPlaceholder == '('){
                    pushStack(operatorStack, sizeof(operator), (void*)(&operatorPlaceholder));
                    *operatorPlaceholder = currentOperator;
                }else {
                    if (currentOperator == ')'){
                        while(*operatorPlaceholder != '('){
                            popStack(operatorStack, (void*)(&operatorPlaceholder));
                            processedExpression[j++] = *operatorPlaceholder;
                            peekStack(operatorStack, (void*)(&operatorPlaceholder));
                        }
                        popStack(operatorStack, (void*)(&operatorPlaceholder));
                    }else {
                        if (currentOperator != '\0'){
                            popStack(operatorStack, (void*)(&operatorPlaceholder));
                            processedExpression[j++] = *operatorPlaceholder;
                            pushStack(operatorStack, sizeof(operator), (void*)(&operatorPlaceholder));
                            *operatorPlaceholder = currentOperator;
                        }else {
                            while(operatorPlaceholder != NULL){
                                popStack(operatorStack, (void*)(&operatorPlaceholder));
                                processedExpression[j++] = *operatorPlaceholder;
                                peekStack(operatorStack, (void*)(&operatorPlaceholder));
                            }
                        }
                    }
                }
            }
        }
    }

    processedExpression[j] = '\0';
    processedExpression = reverseExpression(processedExpression);

    return processedExpression;
}

ExpressionInformation* createExpressionInformation(expression independentExpression){
    ExpressionInformation* expressionInformation = malloc(sizeof(ExpressionInformation));

    expressionInformation -> prefixExpression = malloc(sizeof(char) * 4);
    for (int i = 0; i < 3; i++){
        expressionInformation -> prefixExpression[i] = independentExpression[i];
    }
    expressionInformation -> prefixExpression[3] = '\0';

    expressionInformation -> operandA = malloc(sizeof(char) * 2);
    expressionInformation -> operandB = malloc(sizeof(char) * 2);

    expressionInformation -> operandA[0] = independentExpression[1];
    expressionInformation -> operandA[1] = '\0';
    expressionInformation -> operandB[0] = independentExpression[2];
    expressionInformation -> operandB[1] = '\0';

    expressionInformation -> expressionOperator = independentExpression[0];

    expressionInformation -> operandAValue = 0;
    expressionInformation -> operandBValue = 0;
    expressionInformation -> expressionAnswer = 0;

    expressionInformation -> operandOverwrite = malloc(sizeof(char) * 2);
    expressionInformation -> expressionRepresentation = NULL;

    return expressionInformation;
}