// equation_notation.h
// A library that help converts mathematic expression from one notation type to another.

#ifndef __EXPRESSION_NOTATION__
#define __EXPRESSION_NOTATION__

typedef char* operand;
typedef char operator;
typedef char* expression;
typedef enum NOTATION_TYPE notationType;
typedef struct ExpressionInformation ExpressionInformation;

enum NOTATION_TYPE{
    PREFIX = -1,
    INFIX = 0,
    POSTFIX = 1
};

struct ExpressionInformation{
    expression prefixExpression;
    operand operandA;
    operand operandB;
    operator expressionOperator;

    int operandAValue;
    int operandBValue;
    int expressionAnswer;

    operand operandOverwrite;
    operand expressionRepresentation;
};

expression removeSpaces(expression e);

expression reverseExpression(expression e);

expression reverseParenthesis(expression e);

int checkPrecedence(operator o);

expression infixToPrefix(expression e);

ExpressionInformation* createExpressionInformation(expression independentExpression);

#endif