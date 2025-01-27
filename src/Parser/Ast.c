#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./includes/Ast.h"

/**
 * @Program
 */
Program *createProgram(Location *location)
{
  Program *program = malloc(sizeof(Program));

  if (program == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  program->location = location;
  program->statements = NULL;

  return program;
}

/**
 * @Location
 */
Location *createLocation(char *fileName, size_t line, size_t column)
{
  Location *Location = malloc(sizeof(Location));

  if (Location == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  Location->fileName = strdup(fileName);
  Location->line = line;
  Location->column = column;
  return Location;
}

/**
 * @Statement
 */
Statement *createStatement_Assignment(Location *location, Assignment *assignment)
{
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  s->location = location;
  s->type = ASSIGNMENT_STATEMENT;

  s->assignment = assignment;
  s->variable_declaration = NULL;
  s->print_statement = NULL;
  s->next = NULL;
  return s;
}

/**
 * @Statement
 */
Statement *createStatement_VariableDeclaration(Location *Location, VariableDeclaration *vd)
{
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  s->location = Location;
  s->type = VARIABLE_DECLARATION_STATEMENT;

  s->assignment = NULL;
  s->variable_declaration = vd;
  s->print_statement = NULL;
  s->next = NULL;
  return s;
}

/**
 * @Statement
 */
Statement *createStatement_PrintStatement(Location *location, PrintStatement *ps)
{
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  s->location = location;
  s->type = PRINT_STATEMENT;

  s->assignment = NULL;
  s->variable_declaration = NULL;
  s->print_statement = ps;
  s->next = NULL;
  return s;
}

/**
 * @VariableDeclaration
 */
VariableDeclaration *createVariableDeclaration(Location *location, Type type, Identifier *identifier)
{
  VariableDeclaration *vd = malloc(sizeof(VariableDeclaration));

  if (vd == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  vd->location = location;

  vd->type = type;
  vd->identifier = identifier;
  return vd;
}

/**
 * @Assignment
 */
Assignment *createAssignment(Location *location, Identifier *identifier, Expression *expression)
{
  Assignment *a = malloc(sizeof(Assignment));

  if (a == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  a->location = location;

  a->identifier = identifier;
  a->expression = expression;
  return a;
}

/**
 * @PrintStatement
 */
PrintStatement *createPrintStatement(Location *location, Expression *expression)
{
  PrintStatement *ps = malloc(sizeof(PrintStatement));

  if (ps == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  ps->location = location;
  ps->expression = expression;
  return ps;
}

/**
 * @Expression
 */
Expression *createExpression_Term_ExpressionTail(Location *location, Term *term, ExpressionTail *tail)
{
  Expression *expr = malloc(sizeof(Expression));

  if (expr == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  expr->location = location;

  expr->term = term;
  expr->expression_tail = tail;
  return expr;
}

/**
 * @Expression
 */
Expression *createExpression_Term(Location *location, Term *term)
{
  Expression *expr = malloc(sizeof(Expression));

  if (expr == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  expr->location = location;

  expr->term = term;
  expr->expression_tail = NULL;
  return expr;
}

/**
 * @ExpressionTail
 */
ExpressionTail *createExpressionTail(Location *location, char op, Term *term, ExpressionTail *next)
{
  ExpressionTail *tail = malloc(sizeof(ExpressionTail));

  if (tail == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  tail->location = location;

  tail->op = op;
  tail->term = term;
  tail->next = next;
  return tail;
}

/**
 * @Term
 */
Term *createTerm_number(Location *location, Number *number)
{
  Term *term = malloc(sizeof(Term));

  if (term == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  term->location = location;

  term->number = number;
  term->identifier = NULL;
  term->string = NULL;
  return term;
}

/**
 * @Term
 */
Term *createTerm_identifier(Location *location, Identifier *identifier)
{
  Term *term = malloc(sizeof(Term));

  if (term == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  term->location = location;

  term->number = NULL;
  term->identifier = identifier;
  term->string = NULL;
  return term;
}

/**
 * @Term
 */
Term *createTerm_string(Location *location, String *string)
{
  Term *term = malloc(sizeof(Term));

  if (term == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  term->location = location;

  term->number = NULL;
  term->identifier = NULL;
  term->string = string;
  return term;
}

/**
 * @String
 */
String *createString(Location *location, char *value)
{
  String *str = malloc(sizeof(String));

  if (str == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  str->location = location;

  str->value = strdup(value);
  return str;
}

/**
 * @Number
 */
Number *createNumber(Location *location, int value)
{
  Number *number = malloc(sizeof(Number));

  if (number == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  number->location = location;

  number->value = value;
  return number;
}

/**
 * @Identifier
 */
Identifier *createIdentifier(Location *location, char *name)
{
  Identifier *identifier = malloc(sizeof(Identifier));

  if (identifier == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  identifier->location = location;

  identifier->name = strdup(name);
  return identifier;
}

/**
 * @Type
 */
Type getLiteralType(char *searchType)
{
  if (strcmp(searchType, "int") == 0)
  {
    return TYPE_INT;
  }
  else if (strcmp(searchType, "float") == 0)
  {
    return TYPE_FLOAT;
  }
  else if (strcmp(searchType, "string") == 0)
  {
    return TYPE_STRING;
  }
  else
  {
    printf("Type unknow: %s\n", searchType);
    exit(1);
  }
}
