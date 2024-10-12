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

cJSON *AstConsumerTerm(Term *term)
{
  if (term == NULL)
  {
    printf("Term without term\n");
    exit(1);
  }

  cJSON *jsonTerm = cJSON_CreateObject();

  checkLocation(term->location);

  if (term->number == NULL && term->identifier == NULL && term->string == NULL)
  {
    printf("Term without number, identifier or string\n");
    exit(1);
  }

  if (term->number != NULL)
  {
    // printf("Term\n");
    // printf("Number\n");
    // printf("Value: %d\n", term->number->value);
    cJSON_AddNumberToObject(jsonTerm, "Number", term->number->value);
  }

  if (term->identifier != NULL)
  {
    // printf("Term\n");
    // printf("Identifier\n");
    // printf("Value: %s\n", term->identifier->name);
    cJSON_AddStringToObject(jsonTerm, "Identifier", term->identifier->name);
  }

  if (term->string != NULL)
  {
    // printf("Term\n");
    // printf("String\n");
    // printf("Value: %s\n", term->string->value);
    cJSON_AddStringToObject(jsonTerm, "String", term->string->value);
  }

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

cJSON *AstConsumerExpressionTail(ExpressionTail *exprTail)
{
  if (exprTail == NULL)
  {
    printf("ExpressionTail without expression_tail\n");
    exit(1);
  }

  cJSON *jsonExprTail = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonExprTail, "Op", cJSON_CreateString(&exprTail->op));
  cJSON_AddItemToObject(jsonExprTail, "Term", AstConsumerTerm(exprTail->term));

  if (exprTail->next != NULL)
  {
    cJSON_AddItemToObject(jsonExprTail, "Next", AstConsumerExpressionTail(exprTail->next));
  }

  cJSON_AddItemToObject(jsonExprTail, "Location", checkLocation(exprTail->location));

  return jsonExprTail;
}

cJSON *AstConsumerExpression(Expression *expr)
{
  if (expr == NULL)
  {
    printf("Expression without expression\n");
    exit(1);
  }

  cJSON *jsonExpr = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonExpr, "Term", AstConsumerTerm(expr->term));

  if (expr->expression_tail != NULL)
  {
    cJSON_AddItemToObject(jsonExpr, "ExpressionTail", AstConsumerExpressionTail(expr->expression_tail));
  }

  cJSON_AddItemToObject(jsonExpr, "Location", checkLocation(expr->location));

  return jsonExpr;
}

cJSON *AstConsumerPrintStatement(PrintStatement *ps)
{
  if (ps == NULL)
  {
    printf("PrintStatement without print_statement\n");
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

  if (program.statements == NULL)
  {
    printf("Program without statements\n");
    exit(1);
  }

  cJSON *jsonProgram = cJSON_CreateObject();
  cJSON *jsonStatements = cJSON_CreateArray();

  cJSON_AddItemToObject(jsonProgram, "Statements", jsonStatements);

  Statement *currentStatement = program.statements;

  while (currentStatement != NULL)
  {

    cJSON *jsonStatement = cJSON_CreateObject();
    switch (currentStatement->type)
    {
    case PRINT_STATEMENT:
      if (currentStatement->print_statement == NULL)
      {
        printf("PrintStatement without print_statement\n");
        exit(1);
      }

      cJSON *jsonPrintStatement = AstConsumerPrintStatement(currentStatement->print_statement);
      cJSON_AddItemToObject(jsonStatement, "PrintStatement", jsonPrintStatement);

      currentStatement = currentStatement->next;
      break;

    case VARIABLE_DECLARATION_STATEMENT:
      if (currentStatement->variable_declaration == NULL)
      {
        printf("VariableDeclarationStatement without variable_declaration\n");
        exit(1);
      }

      cJSON *jsonVariableDeclaration = AstConsumerVariableDeclarationStatement(currentStatement->variable_declaration);
      cJSON_AddItemToObject(jsonStatement, "VariableDeclaration", jsonVariableDeclaration);

      currentStatement = currentStatement->next;
      break;
    case ASSIGNMENT_STATEMENT:
      if (currentStatement->assignment == NULL)
      {
        printf("AssignmentStatement without assignment\n");
        exit(1);
      }

      cJSON *jsonAssignment = AstConsumerAssignmentStatement(currentStatement->assignment);
      cJSON_AddItemToObject(jsonStatement, "Assignment", jsonAssignment);

      currentStatement = currentStatement->next;
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
