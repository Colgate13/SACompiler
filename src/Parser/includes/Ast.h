#ifndef AST_H
#define AST_H

#include "../../../libs/includes/cJSON.h"
#include "./Parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Program *createProgram(Location *location);
Location *createLocation(char *fileName, size_t line, size_t column);
StatementTail *createStatementTail(Location *location, Statement *statement);
Block *createBlock(Location *location, StatementTail *statementTail);
Statement *createStatement_IfStatement(Location *location,
                                       IfStatement *ifStatement);
Statement *createStatement_PrintStatement(Location *location,
                                          PrintStatement *ps);
Statement *createStatement_VariableDeclaration(Location *Location,
                                               VariableDeclaration *vd);
Statement *createStatement_Assignment(Location *location,
                                      Assignment *assignment);
VariableDeclaration *createVariableDeclaration(Location *location, Type type,
                                               Identifier *identifier);
Assignment *createAssignment(Location *location, Identifier *identifier,
                             Expression *expression);
PrintStatement *createPrintStatement(Location *location,
                                     Expression *expression);
IfStatement *createIfStatement(Location *location, Expression *expression,
                               Block *block);

Expression *createExpression(Location *location,
                             ArithmeticExpression *arithmeticExpression,
                             OperatorRelational *operatorRelational);
OperatorRelational *createOperatorRelational(Location *location, char *operator,
                                             ArithmeticExpression *
                                             arithmeticExpression);
ArithmeticExpression *
createArithmeticExpression(Location *location, Term *term,
                           ArithmeticExpressionTail *tail);
ArithmeticExpressionTail *
createArithmeticExpressionTail(Location *location, char *op, Term *term,
                               ArithmeticExpressionTail *tail);
Term *createTerm(Location *location, Factor *factor, TermTail *tail);
Factor *createFactor_Expression(Location *location, Expression *expression);
Factor *createFactor_Number(Location *location, Number *number);
Factor *createFactor_Identifier(Location *location, Identifier *identifier);
Factor *createFactor_String(Location *location, String *string);
TermTail *createTermTail(Location *location, char *op, Factor *factor,
                         TermTail *tail);

String *createString(Location *location, char *value);
Number *createNumber(Location *location, int value);
Identifier *createIdentifier(Location *location, char *name);
Type getLiteralType(char *searchType);

#endif
