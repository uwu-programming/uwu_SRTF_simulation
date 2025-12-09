// equation_notation.h
// A library that help converts mathematic expression from one notation type to another.

#ifndef __EXPRESSION_NOTATION__
#define __EXPRESSION_NOTATION__

typedef char* operand;
typedef char* expression;
typedef enum NOTATION_TYPE notationType;

enum NOTATION_TYPE{
    PREFIX = -1,
    INFIX = 0,
    POSTFIX = 1
};

void RemoveSpaces(expression e);

int PrecedenceCheck(operand x);

void ReverseInfix(expression e);

void ReverseBrackets(expression e);

void InfixToPrefix(expression e);

expression notationConversion(expression e, notationType fromNotation, notationType toNotation);

#endif