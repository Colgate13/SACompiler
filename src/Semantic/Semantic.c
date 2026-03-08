#include "./includes/Semantic.h"

// Global context to track current function during analysis
static FunctionContext *currentFunctionContext = NULL;

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
  semantic->currentFunction = NULL;
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
  case FUNCTION_DECLARATION_STATEMENT:
    // Handle function declaration statement
    analyzeFunctionDeclarationStatement(stack, statement->function_declaration);
    break;
  case RETURN_STATEMENT:
    // Handle return statement
    analyzeReturnStatement(stack, statement->return_statement);
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

FunctionContext *createFunctionContext(char *functionName, Type returnType) {
  FunctionContext *context = (FunctionContext *)malloc(sizeof(FunctionContext));

  if (context == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  context->functionName = strdup(functionName);
  context->returnType = returnType;
  context->hasReturn = 0;
  context->parent = currentFunctionContext;

  return context;
}

void destroyFunctionContext(FunctionContext *context) {
  if (context == NULL) {
    return;
  }

  if (context->functionName) {
    free(context->functionName);
  }

  free(context);
}

void analyzeReturnStatement(SymbolTable *stack, Return *returnStmt) {
  logSemantic("SEM#011 - Return statement", "return");

  // SEM#011: Check if we are inside a function
  if (currentFunctionContext == NULL) {
    fprintf(stderr, "Error: Return statement outside of function at %s:%zu:%zu\n",
            returnStmt->location->fileName,
            returnStmt->location->line,
            returnStmt->location->column);
    exit(1);
  }

  // Mark that this function has a return statement
  currentFunctionContext->hasReturn = 1;

  // SEM#011: Check if return expression type matches function return type
  if (returnStmt->expression != NULL) {
    Type returnExprType = inferExpressionType(stack, returnStmt->expression);

    if (returnExprType != currentFunctionContext->returnType) {
      fprintf(stderr,
              "Error: Return type mismatch in function '%s'. Expected '%s', got '%s' at %s:%zu:%zu\n",
              currentFunctionContext->functionName,
              typeToString(currentFunctionContext->returnType),
              typeToString(returnExprType),
              returnStmt->location->fileName,
              returnStmt->location->line,
              returnStmt->location->column);
      exit(1);
    }

    logSemantic("SEM#011 - Return statement: type matches function return type",
                typeToString(returnExprType));
  } else {
    // Return without expression - this would be for void functions if we support them
    fprintf(stderr,
            "Error: Return statement without expression in function '%s' at %s:%zu:%zu\n",
            currentFunctionContext->functionName,
            returnStmt->location->fileName,
            returnStmt->location->line,
            returnStmt->location->column);
    exit(1);
  }

  logSemantic("SEM#011 - Return statement validated", currentFunctionContext->functionName);
}

void analyzeFunctionParameters(SymbolTable *stack, ParameterTail *paramTail, char *functionName) {
  logSemantic("SEM#009 - Analyzing function parameters", functionName);

  if (paramTail == NULL) {
    logSemantic("SEM#009 - No parameters for function", functionName);
    return;
  }

  // Track parameter names to detect duplicates
  int paramCount = 0;
  ParameterTail *current = paramTail;

  while (current != NULL) {
    if (current->parameter != NULL) {
      paramCount++;

      // SEM#009: Check for duplicate parameter names within the same function
      ParameterTail *checker = paramTail;
      int duplicateCount = 0;

      while (checker != current) {
        if (checker->parameter != NULL &&
            strcmp(checker->parameter->identifier->name,
                   current->parameter->identifier->name) == 0) {
          fprintf(stderr,
                  "Error: Duplicate parameter name '%s' in function '%s' at %s:%zu:%zu\n",
                  current->parameter->identifier->name,
                  functionName,
                  current->parameter->location->fileName,
                  current->parameter->location->line,
                  current->parameter->location->column);
          exit(1);
        }
        checker = checker->next;
      }

      logSemantic("SEM#009 - Parameter validated",
                  current->parameter->identifier->name);
    }
    current = current->next;
  }

  logSemantic("SEM#009 - All parameters validated", functionName);
}

void analyzeFunctionDeclarationStatement(SymbolTable *stack, FunctionDeclarationStatement *function) {
  logSemantic("SEM#008 - Function declaration", function->identifier->name);

  // SEM#008: Check if function name is already declared in current scope
  if (lookupSymbol(stack, function->identifier->name) != NULL) {
    fprintf(stderr, "Error: Function '%s' already declared in this scope at %s:%zu:%zu\n",
            function->identifier->name,
            function->location->fileName,
            function->location->line,
            function->location->column);
    exit(1);
  }

  // Insert function symbol in current scope
  insertSymbol(stack, function->identifier->name, SYMBOL_FUNCTION_DECLARATION, function->type, function->location);
  logSemantic("SEM#008 - Function declaration: Function '%s' declared with return type",
              function->identifier->name);

  // SEM#009: Analyze function parameters
  analyzeFunctionParameters(stack, function->parameter_tail, function->identifier->name);

  // Create function context for return statement analysis
  FunctionContext *previousContext = currentFunctionContext;
  currentFunctionContext = createFunctionContext(function->identifier->name, function->type);

  // SEM#010: Analyze function body in its own scope
  pushScope(&stack);

  // Add parameters to function scope
  ParameterTail *paramTail = function->parameter_tail;
  while (paramTail != NULL) {
    if (paramTail->parameter != NULL) {
      insertSymbol(stack, paramTail->parameter->identifier->name,
                   SYMBOL_VARIABLE, paramTail->parameter->type,
                   paramTail->parameter->location);
      logSemantic("SEM#009 - Parameter added to function scope",
                  paramTail->parameter->identifier->name);
    }
    paramTail = paramTail->next;
  }

  // Analyze function body statements
  if (function->block != NULL && function->block->statement_tail != NULL) {
    StatementTail *stmtTail = function->block->statement_tail;
    while (stmtTail != NULL) {
      if (stmtTail->statement != NULL) {
        analyzeStatement(stack, stmtTail->statement);
      }
      if (stmtTail->next == NULL) {
        break;
      }
      stmtTail = stmtTail->next;
    }
  }

  // SEM#011: Check if function has a return statement (required for non-void functions)
  if (!currentFunctionContext->hasReturn) {
    fprintf(stderr,
            "Warning: Function '%s' with return type '%s' may not have a return statement at %s:%zu:%zu\n",
            function->identifier->name,
            typeToString(function->type),
            function->location->fileName,
            function->location->line,
            function->location->column);
  }

  popScope(&stack);

  // Restore previous function context
  destroyFunctionContext(currentFunctionContext);
  currentFunctionContext = previousContext;

  logSemantic("SEM#010 - Function body analyzed", function->identifier->name);
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
