#ifndef AST_JSON_H
#define AST_JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "./Parser.h"
#include "../../../libs/includes/cJSON.h"

// Declaração das funções de conversão
cJSON *AstConsumerTerm(Term *term);
cJSON *AstConsumerIdentifier(Identifier *identifier);
cJSON *AstConsumerExpression(Expression *expr);
cJSON *AstConsumerArithmeticExpression(ArithmeticExpression *ae);
cJSON *AstConsumerOperatorRelational(OperatorRelational *or);
cJSON *AstConsumerFactor(Factor *factor);
cJSON *AstConsumerTermTail(TermTail *tt);
cJSON *AstConsumerArithmeticExpressionTail(ArithmeticExpressionTail *aet);
cJSON *AstConsumerPrintStatement(PrintStatement *ps);
cJSON *AstConsumerVariableDeclarationStatement(VariableDeclaration *vd);
cJSON *AstConsumerAssignmentStatement(Assignment *ag);
cJSON *AstConsumerIfStatement(IfStatement *is);
cJSON *AstConsumerBlock(Block *block);
void AstJsonConsumer(Program program, char *fileOutputAst);

#endif // AST_JSON_H
