#include "./includes/Semantic.h"

void SemanticAnalysis(Semantic *semantic)
{
  // Initialize the symbol table
  SymbolTable *symbolTable = createSymbolTable();
  pushScope(&symbolTable);

  if (!semantic || !semantic->parser || !semantic->parser->ast || !semantic->parser->ast->program)
  {
    fprintf(stderr, "Erro: AST ou Parser nulo\n");
    exit(1);
  }

  StatementTail *statementTail = semantic->parser->ast->program->statement_tail;
  while (statementTail != NULL)
  {
    if (statementTail->statement == NULL)
    {
      break;
    }

    Statement *statement = statementTail->statement;

    switch (statement->type)
    {
      case VARIABLE_DECLARATION_STATEMENT:
        // Handle variable declaration statement
        analyzeVariableDeclaration(symbolTable, statement->variable_declaration);
        break;
      case ASSIGNMENT_STATEMENT:
        // Handle assignment statement
        break;
      case IF_STATEMENT:
        // Handle if statement
        break;
      case PRINT_STATEMENT:
        // Handle print statement
        printf("Semantic > Print statement\n");
        break;
      default:
        fprintf(stderr, "Semantic > Unknown statement type\n");
        exit(1);
    }

    if (statementTail->next == NULL)
    {
      break;
    }

    // Move to the next statement
    statementTail = statementTail->next;
  }

  destroySymbolTable(symbolTable);
}

void analyzeVariableDeclaration(SymbolTable *stack, VariableDeclaration *vd) {
  if (lookupSymbol(stack, vd->identifier->name) != NULL)
  {
    fprintf(stderr, "Error: Variable '%s' already declared in this scope\n", vd->identifier->name);
    return;
  }

  insertSymbol(stack, vd->identifier->name, SYMBOL_VARIABLE, vd->type, vd->location);
}

Symbol *lookupSymbol(SymbolTable *stack, char *name)
{
  SymbolTable *currentScope = stack;
  while (currentScope != NULL)
  {
    Symbol *currentSymbol = currentScope->symbols;
    while (currentSymbol != NULL)
    {
      if (strcmp(currentSymbol->name, name) == 0)
      {
        return currentSymbol;
      }
      currentSymbol = currentSymbol->next;
    }

    currentScope = currentScope->next;
  }

  return NULL;
}

Semantic *createSemantic(Parser *parser)
{
  Semantic *semantic = (Semantic *)malloc(sizeof(Semantic));

  if (semantic == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  semantic->parser = parser;
  return semantic;
}

SymbolTable *createSymbolTable()
{
  SymbolTable *symbolTable = (SymbolTable *)malloc(sizeof(SymbolTable));

  if (symbolTable == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  symbolTable->symbols = NULL;
  symbolTable->scopeLevel = 0;
  symbolTable->next = NULL;
  return symbolTable;
}

void insertSymbol(SymbolTable *symbolTable, char *name, SymbolKind kind, Type type, Location *location)
{
  Symbol *newSymbol = (Symbol *)malloc(sizeof(Symbol));

  if (newSymbol == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  newSymbol->name = strdup(name);
  newSymbol->kind = kind;
  newSymbol->type = type;

  Location *copiedLocation = malloc(sizeof(Location));
  copiedLocation->fileName = strdup(location->fileName);
  copiedLocation->line = location->line;
  copiedLocation->column = location->column;


  newSymbol->next = symbolTable->symbols;
  symbolTable->symbols = newSymbol;
}

void pushScope(SymbolTable **stack)
{
  // Swap the current scope
  SymbolTable *newScope = createSymbolTable();

  newScope->symbols = NULL;
  newScope->scopeLevel = *stack ? (*stack)->scopeLevel + 1 : 0;
  newScope->next = *stack;
  *stack = newScope;
}

void popScope(SymbolTable **symbolTable)
{
  if (*symbolTable == NULL)
  {
    fprintf(stderr, "Error: No scope to pop\n");
    return;
  }

  SymbolTable *oldScope = *symbolTable;
  *symbolTable = oldScope->next;

  destroySymbolTable(oldScope);
}

void destroySymbolTable(SymbolTable *symbolTable)
{
  if (!symbolTable)
    return;
  Symbol *current = symbolTable->symbols;
  Symbol *next;

  while (current != NULL)
  {
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

void destroySemantic(Semantic *semantic)
{
  free(semantic);
}
