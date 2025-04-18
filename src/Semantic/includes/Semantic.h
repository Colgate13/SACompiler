#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Parser/includes/Parser.h"

typedef enum { 
  SYMBOL_VARIABLE,
  SYMBOL_CONSTANT,
} SymbolKind;

typedef struct Symbol {
  char *name;       // name of the symbol
  SymbolKind kind;  // kind of the symbol (variable, constant)
  int scopeLevel;   // scope level of the symbol

  Type type;        // type of the symbol (Variable type, constant type, function return type. ONLY)

  // Location of the symbol
  Location *location;

  struct Symbol *next; // next symbol in the same scope
} Symbol;

typedef struct SymbolTable {
  Symbol *symbols;  // list of symbols in the current scope
  unsigned short int scopeLevel;   // current scope level
  struct SymbolTable *next; // next symbol table (for nested scopes)
} SymbolTable;

typedef struct
{
  Parser *parser;
} Semantic;

Semantic *createSemantic(Parser *parser);
void SemanticAnalysis(Semantic *semantic);
void analyzeVariableDeclaration(SymbolTable *stack, VariableDeclaration *vd);

SymbolTable *createSymbolTable();
void destroySymbolTable(SymbolTable *symbolTable);
void pushScope(SymbolTable **symbolTable);
void popScope(SymbolTable **symbolTable);
void insertSymbol(SymbolTable *stack, char *name, SymbolKind kind, Type type, Location *location);
Symbol *lookupSymbol(SymbolTable *stack, char *name);
// Symbol *findSymbol(SymbolTable *symbolTable, const char *name);
// void checkVariableDeclaration(Semantic *semantic, VariableDeclaration *varDecl);
// void checkAssignment(Semantic *semantic, Assignment *assignment);
// void checkPrintStatement(Semantic *semantic, PrintStatement *printStmt);
void destroySemantic(Semantic *semantic);

#endif