#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "./Parser.h"
#include "../../../libs/includes/cJSON.h"

Program *createProgram(Location *location);
Location *createLocation(char *fileName, size_t line, size_t column);
Statement *createStatement_Assignment(Location *location, Assignment *assignment);
Statement *createStatement_VariableDeclaration(Location *Location, VariableDeclaration *vd);
Statement *createStatement_PrintStatement(Location *location, PrintStatement *ps);
VariableDeclaration *createVariableDeclaration(Location *location, Type type, Identifier *identifier);
Assignment *createAssignment(Location *location, Identifier *identifier, Expression *expression);
PrintStatement *createPrintStatement(Location *location, Expression *expression);
Expression *createExpression_Term_ExpressionTail(Location *location, Term *term, ExpressionTail *tail);
Expression *createExpression_Term(Location *location, Term *term);
ExpressionTail *createExpressionTail(Location *location, char op, Term *term, ExpressionTail *next);
Term *createTerm_number(Location *location, Number *number);
Term *createTerm_identifier(Location *location, Identifier *identifier);
Term *createTerm_string(Location *location, String *string);
String *createString(Location *location, char *value);
Number *createNumber(Location *location, int value);
Identifier *createIdentifier(Location *location, char *name);
Type getLiteralType(char *searchType);

#endif