#include "./includes/AstJson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cJSON *checkLocation(Location *Location)
{
  if (Location == NULL)
  {
    printf("Location without Location\n");
    exit(1);
  }

  if (Location->fileName == NULL)
  {
    printf("Location without fileName\n");
    exit(1);
  }

  if (Location->line == 0)
  {
    printf("Location without line\n");
    exit(1);
  }

  if (Location->column == 0)
  {
    printf("Location without column\n");
    exit(1);
  }

  cJSON *jsonLocation = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonLocation, "FileName", cJSON_CreateString(Location->fileName));
  cJSON_AddItemToObject(jsonLocation, "line", cJSON_CreateNumber(Location->line));
  cJSON_AddItemToObject(jsonLocation, "column", cJSON_CreateNumber(Location->column));

  return jsonLocation;
}

cJSON *AstConsumerFactor(Factor *factor)
{
  if (factor == NULL)
  {
    printf("Factor without factor\n");
    exit(1);
  }

  cJSON *jsonFactor = cJSON_CreateObject();

  if (factor->expression != NULL)
  {
    cJSON_AddItemToObject(jsonFactor, "Expression", AstConsumerExpression(factor->expression));
  }
  else if (factor->number != NULL)
  {
    cJSON_AddItemToObject(jsonFactor, "Number", cJSON_CreateNumber(factor->number->value));
  }
  else if (factor->identifier != NULL)
  {
    cJSON_AddItemToObject(jsonFactor, "Identifier", AstConsumerIdentifier(factor->identifier));
  }
  else if (factor->string != NULL)
  {
    cJSON_AddItemToObject(jsonFactor, "String", cJSON_CreateString(factor->string->value));
  }
  else
  {
    printf("Factor without factor\n");
    exit(1);
  }

  cJSON_AddItemToObject(jsonFactor, "Location", checkLocation(factor->location));

  return jsonFactor;
}

cJSON *AstConsumerTermTail(TermTail *tt)
{
  if (tt == NULL)
  {
    printf("TermTail without term_tail\n");
    exit(1);
  }

  cJSON *jsonTt = cJSON_CreateObject();

  if (tt->factor == NULL)
  {
    printf("TermTail without factor\n");
    exit(1);
  }

  cJSON_AddItemToObject(jsonTt, "Factor", AstConsumerFactor(tt->factor));
  cJSON_AddItemToObject(jsonTt, "Location", checkLocation(tt->location));

  return jsonTt;
}

cJSON *AstConsumerTerm(Term *term)
{
  if (term == NULL)
  {
    printf("Term without term\n");
    exit(1);
  }

  cJSON *jsonTerm = cJSON_CreateObject();

  checkLocation(term->location);

  cJSON_AddItemToObject(jsonTerm, "Factor", AstConsumerFactor(term->factor));
  if (term->term_tail != NULL) cJSON_AddItemToObject(jsonTerm, "TermTail", AstConsumerTermTail(term->term_tail));
  cJSON_AddItemToObject(jsonTerm, "Location", checkLocation(term->location));

  return jsonTerm;
}

cJSON *AstConsumerIdentifier(Identifier *identifier)
{
  if (identifier == NULL)
  {
    printf("Identifier without identifier\n");
    exit(1);
  }

  cJSON *jsonIdentifier = cJSON_CreateObject();

  cJSON_AddItemToObject(jsonIdentifier, "Name", cJSON_CreateString(identifier->name));
  cJSON_AddItemToObject(jsonIdentifier, "Location", checkLocation(identifier->location));

  return jsonIdentifier;
}

cJSON *AstConsumerArithmeticExpressionTail(ArithmeticExpressionTail *aet)
{
  if (aet == NULL)
  {
    printf("ArithmeticExpressionTail without arithmetic_expression_tail\n");
    exit(1);
  }

  cJSON *jsonAet = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonAet, "AddOperator", cJSON_CreateNumber(aet->add_operator));
  cJSON_AddItemToObject(jsonAet, "Term", AstConsumerTerm(aet->term));
  if (aet->next != NULL) cJSON_AddItemToObject(jsonAet, "ArithmeticExpressionTail", AstConsumerArithmeticExpressionTail(aet->next));
  cJSON_AddItemToObject(jsonAet, "Location", checkLocation(aet->location));

  return jsonAet;
}

cJSON *AstConsumerArithmeticExpression(ArithmeticExpression *ae)
{
  if (ae == NULL)
  {
    printf("ArithmeticExpression without arithmetic_expression\n");
    exit(1);
  }

  cJSON *jsonAe = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonAe, "Term", AstConsumerTerm(ae->term));
  if (ae->arithmetic_expression_tail != NULL) cJSON_AddItemToObject(jsonAe, "ArithmeticExpressionTail", AstConsumerArithmeticExpressionTail(ae->arithmetic_expression_tail));
  cJSON_AddItemToObject(jsonAe, "Location", checkLocation(ae->location));

  return jsonAe;
}

cJSON *AstConsumerOperatorRelational(OperatorRelational *or)
{
  if (or == NULL)
  {
    printf("OperatorRelational without operator_relational\n");
    exit(1);
  }

  cJSON *jsonOr = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonOr, "Operator", cJSON_CreateNumber(or->relational_operator));
  cJSON_AddItemToObject(jsonOr, "ArithmeticExpression", AstConsumerArithmeticExpression(or->arithmetic_expression));
  cJSON_AddItemToObject(jsonOr, "Location", checkLocation(or->location));

  return jsonOr;
}

cJSON *AstConsumerExpression(Expression *expr)
{
  if (expr == NULL)
  {
    printf("Expression without expression\n");
    exit(1);
  }

  cJSON *jsonExpr = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonExpr, "ArithmeticExpression", AstConsumerArithmeticExpression(expr->arithmetic_expression));
  if (expr->operator_relational != NULL) cJSON_AddItemToObject(jsonExpr, "OperatorRelational", AstConsumerOperatorRelational(expr->operator_relational));
  cJSON_AddItemToObject(jsonExpr, "Location", checkLocation(expr->location));

  return jsonExpr;
}

cJSON *AstConsumerPrintStatement(PrintStatement *ps)
{
  if (ps == NULL)
  {
    printf("PrintStatement without print_statement2\n");
    exit(1);
  }

  cJSON *jsonPs = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonPs, "Expression", AstConsumerExpression(ps->expression));
  cJSON_AddItemToObject(jsonPs, "Location", checkLocation(ps->location));

  return jsonPs;
}

cJSON *AstConsumerVariableDeclarationStatement(VariableDeclaration *vd)
{
  if (vd == NULL)
  {
    printf("VariableDeclarationStatement without variable_declaration\n");
    exit(1);
  }

  cJSON *jsonVd = cJSON_CreateObject();
  cJSON_AddNumberToObject(jsonVd, "Type", vd->type);
  cJSON_AddItemToObject(jsonVd, "Identifier", AstConsumerIdentifier(vd->identifier));
  cJSON_AddItemToObject(jsonVd, "Location", checkLocation(vd->location));

  return jsonVd;
}

cJSON *AstConsumerAssignmentStatement(Assignment *ag)
{
  if (ag == NULL)
  {
    printf("AssignmentStatement without assignment\n");
    exit(1);
  }

  cJSON *jsonAg = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonAg, "Identifier", AstConsumerIdentifier(ag->identifier));
  cJSON_AddItemToObject(jsonAg, "Expression", AstConsumerExpression(ag->expression));
  cJSON_AddItemToObject(jsonAg, "Location", checkLocation(ag->location));

  return jsonAg;
}

void createOutputFile(cJSON *json, char *fileOutputAst)
{
  FILE *file = fopen(fileOutputAst, "w");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  fprintf(file, "%s", cJSON_Print(json));
  fclose(file);
  printf("File output: %s", fileOutputAst);
}

void AstJsonConsumer(Program program, char *fileOutputAst)
{

  if (program.statement_tail->statement == NULL)
  {
    printf("Program without statements\n");
    exit(1);
  }

  cJSON *jsonProgram = cJSON_CreateObject();
  cJSON *jsonStatements = cJSON_CreateArray();

  cJSON_AddItemToObject(jsonProgram, "Statements", jsonStatements);

  StatementTail *currentStatementTail = program.statement_tail;

  while (currentStatementTail->statement != NULL)
  {
    Statement *currentStatement = currentStatementTail->statement;

    cJSON *jsonStatement = cJSON_CreateObject();
    switch (currentStatement->type)
    {
    case PRINT_STATEMENT:
      if (currentStatement->print_statement == NULL)
      {
        printf("PrintStatement without print_statement1\n");
        exit(1);
      }

      cJSON *jsonPrintStatement = AstConsumerPrintStatement(currentStatement->print_statement);
      cJSON_AddItemToObject(jsonStatement, "PrintStatement", jsonPrintStatement);

      currentStatementTail = currentStatementTail->next;
      break;

    case VARIABLE_DECLARATION_STATEMENT:
      if (currentStatement->variable_declaration == NULL)
      {
        printf("VariableDeclarationStatement without variable_declaration\n");
        exit(1);
      }

      cJSON *jsonVariableDeclaration = AstConsumerVariableDeclarationStatement(currentStatement->variable_declaration);
      cJSON_AddItemToObject(jsonStatement, "VariableDeclaration", jsonVariableDeclaration);

      currentStatementTail = currentStatementTail->next;
      break;
    case ASSIGNMENT_STATEMENT:
      if (currentStatement->assignment == NULL)
      {
        printf("AssignmentStatement without assignment\n");
        exit(1);
      }

      cJSON *jsonAssignment = AstConsumerAssignmentStatement(currentStatement->assignment);
      cJSON_AddItemToObject(jsonStatement, "Assignment", jsonAssignment);

      currentStatementTail = currentStatementTail->next;
      break;
    default:
      printf("Statement type unknow\n");
      exit(1);
      break;
    }

    cJSON_AddItemToArray(jsonStatements, jsonStatement);
  }

  cJSON *jsonLocation = checkLocation(program.location);
  cJSON_AddItemToObject(jsonProgram, "Location", jsonLocation);
  createOutputFile(jsonProgram,  fileOutputAst);
  cJSON_Delete(jsonProgram);
}
