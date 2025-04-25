#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Parser/includes/Parser.h"

#define LOGS_SEMANTIC 0

typedef enum
{
  SYMBOL_VARIABLE,
  SYMBOL_CONSTANT,
} SymbolKind;

typedef struct Symbol
{
  char *name;      // name of the symbol
  SymbolKind kind; // kind of the symbol (variable, constant)

  Type type; // type of the symbol (Variable type, constant type, function return type. ONLY)
  unsigned short int isUsed;

  // Location of the symbol
  Location *location;

  struct Symbol *next; // next symbol in the same scope
} Symbol;

typedef struct SymbolTable
{
  Symbol *symbols;               // list of symbols in the current scope
  unsigned short int scopeLevel; // current scope level
  struct SymbolTable *next;      // next symbol table (for nested scopes)
} SymbolTable;

typedef struct
{
  Parser *parser;
} Semantic;

void logSemantic(const char *messageKey, const char *messageValue);
void SemanticAnalysis(Semantic *semantic);
void analyzeStatement(SymbolTable *stack, Statement *stmt);
void analyzeVariableDeclaration(SymbolTable *stack, VariableDeclaration *vd);
void analyzeAssignment(SymbolTable *stack, Assignment *assignment);
void analyzeIfStatement(SymbolTable *stack, IfStatement *ifStatement);
void analyzePrintStatement(SymbolTable *stack, PrintStatement *printStatement);

SymbolTable *createSymbolTable();
Symbol *lookupSymbol(SymbolTable *stack, char *name);
Semantic *createSemantic(Parser *parser);

Type inferExpressionType(SymbolTable *stack, Expression *expr);
Type inferTermType(SymbolTable *stack, Term *term);
Type inferFactorType(SymbolTable *stack, Factor *factor);
Type inferRelationalExpressionType(SymbolTable *stack, Expression *expr);

const char *typeToString(Type type);
void destroySymbolTable(SymbolTable *symbolTable);
void pushScope(SymbolTable **symbolTable);
void popScope(SymbolTable **symbolTable);
void insertSymbol(SymbolTable *stack, char *name, SymbolKind kind, Type type, Location *location);
void destroySemantic(Semantic *semantic);

#endif