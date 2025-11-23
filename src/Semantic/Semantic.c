#include "./includes/Semantic.h"

void logSemantic(const char *messageKey, const char *messageValue) {
  if (LOGS_SEMANTIC == 1) {
    printf("Semantic<[%s]> %s\n", messageKey, messageValue);
  }
}

Semantic *createSemantic(Parser *parser) {
  Semantic *semantic = (Semantic *)malloc(sizeof(Semantic));

  if (semantic == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  semantic->parser = parser;
  return semantic;
}

SymbolTable *createSymbolTable() {
  SymbolTable *symbolTable = (SymbolTable *)malloc(sizeof(SymbolTable));

  if (symbolTable == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  symbolTable->symbols = NULL;
  symbolTable->scopeLevel = 0;
  symbolTable->next = NULL;
  return symbolTable;
}

void SemanticAnalysis(Semantic *semantic) {
  // Initialize the symbol table. Create the first scope
  // and push it to the stack
  SymbolTable *symbolTable = createSymbolTable();
  pushScope(&symbolTable);

  if (!semantic || !semantic->parser || !semantic->parser->ast ||
      !semantic->parser->ast->program) {
    fprintf(stderr, "Erro: AST ou Parser nulo\n");
    exit(1);
  }

  StatementTail *statementTail = semantic->parser->ast->program->statement_tail;
  while (statementTail != NULL) {
    if (statementTail->statement == NULL) {
      break;
    }

    Statement *statement = statementTail->statement;

    // Analyze the statement
    analyzeStatement(symbolTable, statement);

    if (statementTail->next == NULL) {
      break;
    }

    // Move to the next statement
    statementTail = statementTail->next;
  }

  destroySymbolTable(symbolTable);
}

void analyzeStatement(SymbolTable *stack, Statement *statement) {
  if (!statement)
    return;

  switch (statement->type) {
  case VARIABLE_DECLARATION_STATEMENT:
    // Handle variable declaration statement
    analyzeVariableDeclaration(stack, statement->variable_declaration);
    break;
  case ASSIGNMENT_STATEMENT:
    // Handle assignment statement
    analyzeAssignment(stack, statement->assignment);
    break;
  case IF_STATEMENT:
    // Handle if statement
    analyzeIfStatement(stack, statement->if_statement);
    break;
  case PRINT_STATEMENT:
    // Handle print statement
    analyzePrintStatement(stack, statement->print_statement);
    break;
  case BLOCK:
    // Handle block statement
    pushScope(&stack);
    analyzeStatement(stack, statement->block->statement_tail->statement);
    popScope(&stack);
    break;
  default:
    fprintf(stderr, "Semantic > Unknown statement type\n");
    exit(1);
  }
}

void analyzeVariableDeclaration(SymbolTable *stack, VariableDeclaration *vd) {
  logSemantic("SEM#001 - Variable declaration", vd->identifier->name);

  if (lookupSymbol(stack, vd->identifier->name) != NULL) {
    fprintf(stderr, "Error: Variable '%s' already declared in this scope\n",
            vd->identifier->name);
    exit(1);
  }

  insertSymbol(stack, vd->identifier->name, SYMBOL_VARIABLE, vd->type,
               vd->location);
  logSemantic("SEM#001 - Variable declaration: Variable '%s' declared",
              vd->identifier->name);
}

void analyzeAssignment(SymbolTable *stack, Assignment *assignment) {
  logSemantic("SEM#002 - Assignment: %s", assignment->identifier->name);
  Symbol *symbol = lookupSymbol(stack, assignment->identifier->name);
  if (symbol == NULL) {
    fprintf(stderr, "Error: Variable '%s' not declared\n",
            assignment->identifier->name);
    exit(1);
  }

  // Check if the types match
  {
    Type expressionType = inferExpressionType(stack, assignment->expression);

    printf("Inferred expression type: %s\n", typeToString(expressionType));
    printf("Variable '%s' type: %s\n", symbol->name,
           typeToString(symbol->type));

    if (symbol->type != expressionType) {
      fprintf(stderr,
              "Error: Type mismatch in assignment to '%s'. Expected '%s', got "
              "'%s'\n",
              symbol->name, typeToString(symbol->type),
              typeToString(expressionType));
      exit(1);
    }
  }

  // Mark the variable as used
  symbol->isUsed = 1;
  logSemantic("SEM#002 - Assignment: Variable '%s' is used", symbol->name);
}

void analyzeIfStatement(SymbolTable *stack, IfStatement *ifStatement) {
  logSemantic("SEM#003 - If statement", "if");

  Type conditionType =
      inferRelationalExpressionType(stack, ifStatement->expression);

  if (conditionType != TYPE_INT) {
    fprintf(stderr, "Error: Invalid type for if statement\n");
    exit(1);
  }

  pushScope(&stack);
  analyzeStatement(stack, ifStatement->then_statement);
  popScope(&stack);

  if (ifStatement->else_statement != NULL) {
    pushScope(&stack);
    analyzeStatement(stack, ifStatement->else_statement);
    popScope(&stack);
  }

  logSemantic("SEM#003 - If statement: type '%s'", typeToString(conditionType));
}

void analyzePrintStatement(SymbolTable *stack, PrintStatement *printStatement) {
  logSemantic("SEM#007 - Print statement", "print");

  // Check if the expression is valid and infer its type
  Type expressionType = inferExpressionType(stack, printStatement->expression);

  logSemantic("SEM#007 - Print statement: expression type is",
              typeToString(expressionType));
}

Type inferRelationalExpressionType(SymbolTable *stack, Expression *expr) {
  if (!expr || !expr->arithmetic_expression) {
    fprintf(stderr, "Error: invalid relational expression\n");
    exit(1);
  }

  Type left = inferTermType(stack, expr->arithmetic_expression->term);

  if (!expr->operator_relational) {
    // No relational operator, just return the type of the term
    return left;
  }

  Type right = inferTermType(
      stack, expr->operator_relational->arithmetic_expression->term);

  if (left == TYPE_STRING || right == TYPE_STRING) {
    if (left != right) {
      fprintf(stderr, "Error: incompatible types in relational operation "
                      "(string vs non-string)\n");
      exit(1);
    }
  }

  // int vs double, double vs int é ok (implicit conversion)
  // int vs int, double vs double => ok

  return TYPE_INT; // Expression type is always int for relational expressions
}

Type inferExpressionType(SymbolTable *stack, Expression *expr) {
  logSemantic("SEM#003 - Expression", "expression");
  if (!expr || !expr->arithmetic_expression) {
    fprintf(stderr, "Error: invalid expression\n");
    exit(1);
  }

  Type base = inferTermType(stack, expr->arithmetic_expression->term);
  ArithmeticExpressionTail *tail =
      expr->arithmetic_expression->arithmetic_expression_tail;

  while (tail != NULL) {
    Type right = inferTermType(stack, tail->term);

    // SEM#005
    if (base == TYPE_STRING || right == TYPE_STRING) {
      if (tail->add_operator != ADD) {
        fprintf(stderr, "Error: invalid operation on strings\n");
        exit(1);
      }
      base = TYPE_STRING;
    } else if (base == TYPE_DOUBLE || right == TYPE_DOUBLE) {
      base = TYPE_DOUBLE;
    } else {
      base = TYPE_INT;
    }

    tail = tail->next;
  }

  logSemantic("SEM#003 - Expression: type", typeToString(base));
  return base;
}

Type inferTermType(SymbolTable *stack, Term *term) {
  logSemantic("SEM#005 - Arithmetic Expressions", "arithmetic expression");
  if (!term || !term->factor) {
    fprintf(stderr, "Error: invalid term\n");
    exit(1);
  }

  Type base = inferFactorType(stack, term->factor);
  TermTail *tail = term->term_tail;

  while (tail != NULL) {
    Type right = inferFactorType(stack, tail->factor);

    if (base == TYPE_STRING || right == TYPE_STRING) {
      fprintf(stderr,
              "Error: invalid operation on string with '*' '/' or '%%'\n");
      exit(1);
    }

    if (tail->mult_operator == MOD &&
        (base == TYPE_DOUBLE || right == TYPE_DOUBLE)) {
      fprintf(stderr,
              "Error: modulo operator '%%' cannot be used with double\n");
      exit(1);
    }

    if (base == TYPE_DOUBLE || right == TYPE_DOUBLE) {
      base = TYPE_DOUBLE;
    } else {
      base = TYPE_INT;
    }

    tail = tail->next;
  }

  return base;
}

Type inferFactorType(SymbolTable *stack, Factor *factor) {
  logSemantic("SEM#006 - Factor", "factor");
  if (!factor) {
    fprintf(stderr, "Error: null factor\n");
    exit(1);
  }

  if (factor->number != NULL) {
    if (factor->number->type == TYPE_INT) {
      return TYPE_INT;
    } else if (factor->number->type == TYPE_DOUBLE) {
      return TYPE_DOUBLE;
    } else {
      fprintf(stderr, "Error: unknown number type\n");
      exit(1);
    }
  }

  if (factor->string != NULL) {
    return TYPE_STRING;
  }

  if (factor->identifier != NULL) {
    Symbol *symbol = lookupSymbol(stack, factor->identifier->name);
    if (!symbol) {
      fprintf(stderr, "Error: variable '%s' not declared\n",
              factor->identifier->name);
      exit(1);
    }
    symbol->isUsed = 1; // (SEM#006)
    return symbol->type;
  }

  if (factor->expression != NULL) {
    return inferExpressionType(stack, factor->expression);
  }

  if (factor->unary_operator != NULL && factor->factor != NULL) {
    return inferFactorType(stack, factor->factor);
  }

  fprintf(stderr, "Error: unable to infer factor type\n");
  exit(1);
}

Symbol *lookupSymbol(SymbolTable *stack, char *name) {
  SymbolTable *currentScope = stack;
  while (currentScope != NULL) {
    Symbol *currentSymbol = currentScope->symbols;
    while (currentSymbol != NULL) {
      if (strcmp(currentSymbol->name, name) == 0) {
        return currentSymbol;
      }
      currentSymbol = currentSymbol->next;
    }

    currentScope = currentScope->next;
  }

  return NULL;
}

const char *typeToString(Type type) {
  switch (type) {
  case TYPE_INT:
    return "int";
  case TYPE_DOUBLE:
    return "double";
  case TYPE_STRING:
    return "string";
  default:
    return "unknown";
  }
}

void insertSymbol(SymbolTable *symbolTable, char *name, SymbolKind kind,
                  Type type, Location *location) {
  Symbol *newSymbol = (Symbol *)malloc(sizeof(Symbol));

  if (newSymbol == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  newSymbol->name = strdup(name);
  newSymbol->kind = kind;
  newSymbol->type = type;
  newSymbol->isUsed = 0;

  Location *copiedLocation = malloc(sizeof(Location));
  copiedLocation->fileName = strdup(location->fileName);
  copiedLocation->line = location->line;
  copiedLocation->column = location->column;

  // Swap the next symbol to the new symbol in symbolTable
  newSymbol->next = symbolTable->symbols;
  symbolTable->symbols = newSymbol;
}

void pushScope(SymbolTable **stack) {
  // Swap the current scope
  SymbolTable *newScope = createSymbolTable();

  newScope->symbols = NULL;
  newScope->scopeLevel = *stack ? (*stack)->scopeLevel + 1 : 0;
  newScope->next = *stack;
  *stack = newScope;
}

void popScope(SymbolTable **symbolTable) {
  if (*symbolTable == NULL) {
    fprintf(stderr, "Error: No scope to pop\n");
    return;
  }

  SymbolTable *oldScope = *symbolTable;
  *symbolTable = oldScope->next;

  destroySymbolTable(oldScope);
}

void destroySymbolTable(SymbolTable *symbolTable) {
  if (!symbolTable)
    return;
  Symbol *current = symbolTable->symbols;
  Symbol *next;

  while (current != NULL) {
    next = current->next;
    if (current->name)
      free(current->name);
    if (current->location)
      free(current->location);
    free(current);
    current = next;
  }

  free(symbolTable);
}

void destroySemantic(Semantic *semantic) { free(semantic); }
