#include "./includes/Ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @Program
 */
Program *createProgram(Location *location) {
  Program *program = malloc(sizeof(Program));

  if (program == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  program->location = location;
  program->statement_tail = NULL;

  return program;
}

/**
 * @Location
 */
Location *createLocation(char *fileName, size_t line, size_t column) {
  Location *Location = malloc(sizeof(Location));

  if (Location == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  Location->fileName = strdup(fileName);
  Location->line = line;
  Location->column = column;
  return Location;
}

StatementTail *createStatementTail(Location *location, Statement *statement) {
  StatementTail *st = malloc(sizeof(StatementTail));

  if (st == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  st->location = location;
  st->statement = statement;
  st->next = NULL;
  return st;
}

/**
 * @Block
 */
Block *createBlock(Location *location, StatementTail *statementTail) {
  Block *block = malloc(sizeof(Block));

  if (block == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  block->location = location;
  block->statement_tail = statementTail;
  return block;
}

/**
 * @Statement
 */
Statement *createStatement_Assignment(Location *location,
                                      Assignment *assignment) {
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL) {
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
Statement *createStatement_VariableDeclaration(Location *Location,
                                               VariableDeclaration *vd) {
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL) {
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
Statement *createStatement_PrintStatement(Location *location,
                                          PrintStatement *ps) {
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL) {
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
 * @Statement
 */
Statement *createStatement_IfStatement(Location *location, IfStatement *is) {
  Statement *s = malloc(sizeof(Statement));

  if (s == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  s->location = location;
  s->type = IF_STATEMENT;

  s->assignment = NULL;
  s->variable_declaration = NULL;
  s->print_statement = NULL;
  s->if_statement = is;
  s->next = NULL;
  return s;
}

/**
 * @VariableDeclaration
 */
VariableDeclaration *createVariableDeclaration(Location *location, Type type,
                                               Identifier *identifier) {
  VariableDeclaration *vd = malloc(sizeof(VariableDeclaration));

  if (vd == NULL) {
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
Assignment *createAssignment(Location *location, Identifier *identifier,
                             Expression *expression) {
  Assignment *a = malloc(sizeof(Assignment));

  if (a == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  a->location = location;

  a->identifier = identifier;
  a->expression = expression;
  return a;
}

/**
 * @IfStatement
 */
IfStatement *createIfStatement(Location *location, Expression *expression,
                               Block *block) {
  IfStatement *is = malloc(sizeof(IfStatement));

  if (is == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  is->location = location;
  is->expression = expression;
  is->block = block;

  return is;
}

/**
 * @PrintStatement
 */
PrintStatement *createPrintStatement(Location *location,
                                     Expression *expression) {
  PrintStatement *ps = malloc(sizeof(PrintStatement));

  if (ps == NULL) {
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
Expression *createExpression(Location *location,
                             ArithmeticExpression *arithmeticExpression,
                             OperatorRelational *operatorRelational) {
  Expression *expr = malloc(sizeof(Expression));

  if (expr == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  expr->location = location;
  expr->arithmetic_expression = arithmeticExpression;
  expr->operator_relational = operatorRelational;

  return expr;
}

/**
 * @OperatorRelational
 */
OperatorRelational *
createOperatorRelational(Location *location, char *operator,
                         ArithmeticExpression *arithmeticExpression) {
  OperatorRelational *or = malloc(sizeof(OperatorRelational));

  if (or == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  or->location = location;
  or->arithmetic_expression = arithmeticExpression;

  if (strcmp(operator, "==") == 0) {
    or->relational_operator = EQUALS;
  } else if (strcmp(operator, "!=") == 0) {
    or->relational_operator = DIFFERENT;
  } else if (strcmp(operator, "<") == 0) {
    or->relational_operator = LESS;
  } else if (strcmp(operator, "<=") == 0) {
    or->relational_operator = LESS_EQUALS;
  } else if (strcmp(operator, ">") == 0) {
    or->relational_operator = GREATER;
  } else if (strcmp(operator, ">=") == 0) {
    or->relational_operator = GREATER_EQUALS;
  } else {
    fprintf(stderr, "Unknown operator: %s\n", operator);
    exit(1);
  }

  return or;
}

/**
 * @ArithmeticExpression
 */
ArithmeticExpression *
createArithmeticExpression(Location *location, Term *term,
                           ArithmeticExpressionTail *tail) {
  ArithmeticExpression *ae = malloc(sizeof(ArithmeticExpression));

  if (ae == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  ae->location = location;
  ae->term = term;
  ae->arithmetic_expression_tail = tail;
  return ae;
}

/**
 * @ArithmeticExpressionTail
 */
ArithmeticExpressionTail *
createArithmeticExpressionTail(Location *location, char *op, Term *term,
                               ArithmeticExpressionTail *tail) {
  ArithmeticExpressionTail *aeTail = malloc(sizeof(ArithmeticExpressionTail));

  if (aeTail == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  aeTail->location = location;
  aeTail->term = term;
  aeTail->next = tail;

  if (strcmp(op, "+") == 0) {
    aeTail->add_operator = ADD;
  } else if (strcmp(op, "-") == 0) {
    aeTail->add_operator = SUB;
  } else {
    fprintf(stderr, "Unknown operator: %s\n", op);
    exit(1);
  }

  return aeTail;
}

/**
 * @Term
 */
Term *createTerm(Location *location, Factor *factor, TermTail *tail) {
  Term *term = malloc(sizeof(Term));

  if (term == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  term->location = location;
  term->factor = factor;
  term->term_tail = tail;
  return term;
}

/**
 * @Factor
 */
Factor *createFactor_Expression(Location *location, Expression *expression) {
  Factor *factor = malloc(sizeof(Factor));

  if (factor == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  factor->location = location;
  factor->expression = expression;
  factor->number = NULL;
  factor->identifier = NULL;
  factor->string = NULL;
  return factor;
}

/**
 * @Factor
 */
Factor *createFactor_Number(Location *location, Number *number) {
  Factor *factor = malloc(sizeof(Factor));

  if (factor == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  factor->location = location;
  factor->expression = NULL;
  factor->number = number;
  factor->identifier = NULL;
  factor->string = NULL;
  return factor;
}

/**
 * @Factor
 */
Factor *createFactor_Identifier(Location *location, Identifier *identifier) {
  Factor *factor = malloc(sizeof(Factor));

  if (factor == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  factor->location = location;
  factor->expression = NULL;
  factor->number = NULL;
  factor->identifier = identifier;
  factor->string = NULL;
  return factor;
}

/**
 * @Factor
 */
Factor *createFactor_String(Location *location, String *string) {
  Factor *factor = malloc(sizeof(Factor));

  if (factor == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  factor->location = location;
  factor->expression = NULL;
  factor->number = NULL;
  factor->identifier = NULL;
  factor->string = string;
  return factor;
}

/**
 * @TermTail
 */
TermTail *createTermTail(Location *location, char *op, Factor *factor,
                         TermTail *tail) {
  TermTail *termTail = malloc(sizeof(TermTail));

  if (termTail == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  termTail->location = location;
  termTail->factor = factor;
  termTail->next = tail;

  if (strcmp(op, "*") == 0) {
    termTail->mult_operator = MULT;
  } else if (strcmp(op, "/") == 0) {
    termTail->mult_operator = DIV;
  } else if (strcmp(op, "%") == 0) {
    termTail->mult_operator = MOD;
  } else {
    fprintf(stderr, "Unknown operator: %s\n", op);
    exit(1);
  }

  return termTail;
}

/**
 * @String
 */
String *createString(Location *location, char *value) {
  String *str = malloc(sizeof(String));

  if (str == NULL) {
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
Number *createNumber(Location *location, int value) {
  Number *number = malloc(sizeof(Number));

  if (number == NULL) {
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
Identifier *createIdentifier(Location *location, char *name) {
  Identifier *identifier = malloc(sizeof(Identifier));

  if (identifier == NULL) {
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
Type getLiteralType(char *searchType) {
  if (strcmp(searchType, "int") == 0) {
    return TYPE_INT;
  } else if (strcmp(searchType, "float") == 0) {
    return TYPE_FLOAT;
  } else if (strcmp(searchType, "string") == 0) {
    return TYPE_STRING;
  } else {
    printf("Type unknow: %s\n", searchType);
    exit(1);
  }
}
