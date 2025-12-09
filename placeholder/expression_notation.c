#include "expression_notation.h"
#include "../data_structure/stack.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


void RemoveSpaces(expression e){
    int i, j = 0;
    char temp[strlen(e) + 1];
    for (i = 0; i < strlen(e); i++) {
        if (e[i] != ' ') {
            temp[j++] = e[i];
        }
    }
    temp[j] = '\0'; 
    strcpy(e, temp);
}

int PrecedenceCheck(operand x){
    if (strcmp(x, "+") == 0 || strcmp(x, "-") == 0)
        return 1;
    if (strcmp(x, "*") == 0 || strcmp(x, "/") == 0)
        return 2;
    return 0;
}

void ReverseInfix(expression e){
    int i, j;
    char temp;
    for (i = 0, j = strlen(e) - 1; i < j; i++, j--) {
        temp = e[i];
        e[i] = e[j];
        e[j] = temp;
    }
}

void ReverseBrackets(expression e){
    int i;
    for (i = 0; i < strlen(e); i++) {
        if (e[i] == '(')
            e[i] = ')';
        else if (e[i] == ')')
            e[i] = '(';
    }
}

void InfixToPrefix(expression e){

    RemoveSpaces(e);
    ReverseInfix(e);
    ReverseBrackets(e);
    printf("Reversed Infix: %s\n", e);

    Stack* stack = createStack();
    expression result = malloc(strlen(e) + 1);
    result[0] = '\0';

    for (int i = 0; i < strlen(e); i++) {
        char c = e[i];
        if (isalnum(c)) {
            strncat(result, &c, 1);
        } else if (c == '(') {
            char* op = malloc(sizeof(char));
            pushStack(stack, sizeof(char), (void*)(&op));
            *op = c;
            printf("Push1: %c\n", c);
            
        } else if (c == ')') {
            char* topOp = malloc(sizeof(char));
            popStack(stack, (void*)(&topOp));
            printf("Pop1check: %s\n", topOp);
            while (topOp != NULL && *topOp != '(') {
                strncat(result, topOp, 1);
                free(topOp);
                popStack(stack, (void*)(&topOp));
                printf("Pop1untilNull: %s\n", topOp);
            }
            if (topOp != NULL)
                free(topOp);
            
        } else {
            char* topOp = malloc(sizeof(char));
            char* cStr = malloc(sizeof(char));
            popStack(stack, (void*)(&topOp));
            printf("Pop2check: %s\n", topOp);
            if (topOp != NULL) {
                pushStack(stack, sizeof(char), (void*)(&cStr));
                *cStr = *topOp;
                printf("Push2back: %c\n", *topOp);
                while (topOp != NULL && PrecedenceCheck(topOp) > PrecedenceCheck(&c)) {
                    strncat(result, topOp, 1);
                    free(topOp);
                    popStack(stack, (void*)(&topOp));
                    printf("Pop2untilNull: %s\n", topOp);
                }
            }

            char* newOp = malloc(sizeof(char));
            pushStack(stack, sizeof(char), (void*)(&newOp));
            *newOp = c;
            printf("Push3: %c\n", c);
        }
        printf("Intermediate Result: %s\n", result);
    }
    char* topOp = malloc(sizeof(char));
    popStack(stack, (void*)(&topOp));
    printf("Pop3check: %s\n", topOp);
    while (topOp != NULL) {
        strncat(result, topOp, 1);
        free(topOp);
        popStack(stack, (void*)(&topOp));
        printf("Pop3untilNull: %s\n", topOp);
    }
    printf("Prefix: %s\n", result);
    ReverseInfix(result);
    printf("Final Prefix: %s\n", result);
}

expression notationConversion(expression e, notationType fromNotation, notationType toNotation){

}

