#ifndef PARSER_H
#define PARSER_H

#pragma once

#include "../../Lexical/includes/lexicalAnalyzer.h"
#define LOGS 1

enum EKeywords { PROGRAM = 0, END, VAR, PRINT, IF, INT, FLOAT, STRING };

enum EStatementsType {
  PRINT_STATEMENT = 0,
  ASSIGNMENT_STATEMENT,
  VARIABLE_DECLARATION_STATEMENT,
  IF_STATEMENT
};

typedef enum { TYPE_INT = 0, TYPE_FLOAT, TYPE_STRING } Type;

// <add_operator> --> "+" | "-"
typedef enum {
  ADD = 0, // +
  SUB      // -
} AddOperator;

// <mult_operator> --> "*" | "/" | "%"
typedef enum {
  MULT = 0, // *
  DIV,      // /
  MOD       // %
} MultOperator;

// <relational_operator> --> "==" | "!=" | "<" | "<=" | ">" | ">="
typedef enum {
  EQUALS = 0,
  DIFFERENT,
  LESS,
  LESS_EQUALS,
  GREATER,
  GREATER_EQUALS
} RelationalOperators;

// FORWARD DECLARATIONS
typedef struct StatementTail StatementTail;
typedef struct Statement Statement;
typedef struct Expression Expression;
typedef struct OperatorRelational OperatorRelational;
typedef struct ArithmeticExpression ArithmeticExpression;
typedef struct ArithmeticExpressionTail ArithmeticExpressionTail;
typedef struct Term Term;
typedef struct TermTail TermTail;
typedef struct Factor Factor;
typedef struct Block Block;
typedef struct IfStatement IfStatement;
typedef struct PrintStatement PrintStatement;
typedef struct VariableDeclaration VariableDeclaration;
typedef struct Assignment Assignment;
typedef struct Program Program;
typedef struct Ast Ast;
typedef struct Parser Parser;

// Structs Definitions

// Location = { start: 0, end: 0, fileName: "test.txt" }
typedef struct Location {
  size_t line;   // Line
  size_t column; // Column
  char *fileName;
} Location;

// <block> --> "{" <statement_tail> "}"
typedef struct Block {
  StatementTail *statement_tail;
  Location *location;
} Block;

// <identifier> --> [a-zA-Z_][a-zA-Z0-9_]*
typedef struct Identifier {
  char *name;
  Location *location;
} Identifier;

// <number> --> [0-9]+
typedef struct Number {
  int value;
  Location *location;
} Number;

// <string> --> '"' [a-zA-Z0-9_]* '"'
typedef struct String {
  char *value;
  Location *location;
} String;

// Factor = "(" <expression> ")" | <number> | <identifier> | <string>
typedef struct Factor {
  Expression *expression;
  // |
  Number *number;
  // |
  Identifier *identifier;
  // |
  String *string;
  Location *location;
} Factor;

// <term_tail> --> <mult_operator> <factor> <term_tail> | ε
typedef struct TermTail {
  MultOperator mult_operator;
  Factor *factor;
  struct TermTail *next;
  Location *location;
} TermTail;

// Term = <factor> <term_tail>
typedef struct Term {
  Factor *factor;
  TermTail *term_tail;
  Location *location;
} Term;

// <arithmetic_expression_tail> --> <add_operator> <term>
// <arithmetic_expression_tail> | ε
typedef struct ArithmeticExpressionTail {
  AddOperator add_operator;
  Term *term;
  struct ArithmeticExpressionTail *next;
  Location *location;
} ArithmeticExpressionTail;

// <arithmetic_expression> --> <term> <arithmetic_expression_tail>
typedef struct ArithmeticExpression {
  Term *term;
  ArithmeticExpressionTail *arithmetic_expression_tail;
  Location *location;
} ArithmeticExpression;

// <operator_relational> --> <relational_operator> <arithmetic_expression> | ε
typedef struct OperatorRelational {
  RelationalOperators relational_operator;
  ArithmeticExpression *arithmetic_expression;
  Location *location;
} OperatorRelational;

// <expression> --> <arithmetic_expression> <operator_relational>
typedef struct Expression {
  ArithmeticExpression *arithmetic_expression;
  OperatorRelational *operator_relational;
  Location *location;
} Expression;

// <if_statement> --> "if" "(" <expression> ")" <block>
typedef struct IfStatement {
  Expression *expression;
  Block *block;
  Location *location;
} IfStatement;

// <print_statement> --> "print(" <expression> ");"
typedef struct PrintStatement {
  Expression *expression;
  Location *location;
} PrintStatement;

// <variable_declaration> --> "var" <type> <identifier> ";"
typedef struct VariableDeclaration {
  Type type;
  Identifier *identifier;
  Location *location;
} VariableDeclaration;

// <assignment> --> <identifier> "=" <expression> ";"
typedef struct Assignment {
  Identifier *identifier;
  Expression *expression;
  Location *location;
} Assignment;

// <statement> -> <assignment> | <variable_declaration> | <print_statement> |
// <if_statement>
typedef struct Statement {
  Assignment *assignment;
  // |
  VariableDeclaration *variable_declaration;
  // |
  PrintStatement *print_statement;
  // |
  IfStatement *if_statement;

  struct Statement *next;
  Location *location;
  unsigned short int type;
} Statement;

// <statement_tail> -> <statement> <statement_tail> | ε
typedef struct StatementTail {
  Statement *statement;
  struct StatementTail *next;
  Location *location;
} StatementTail;

// <program> -> "program" <statement_tail> "end;"
typedef struct Program {
  StatementTail *statement_tail;
  Location *location;
} Program;

typedef struct Ast {
  Program *program;
} Ast;

typedef struct Parser {
  LexicalAnalyzer *lexicalAnalyzer;
  Ast *ast;
  Token token;
} Parser;

// FUNCTIONS DECLARATIONS
void controlNextToken(Parser *parser);
void ParserProgram(Parser *parser);

Parser *createParser(LexicalAnalyzer *lexicalAnalyzer);
void destroyParser(Parser *parser);

Block *ParserBlock(Parser *parser);
StatementTail *ParserStatementTail(Parser *parser);
Statement *ParserStatement(Parser *parser);
IfStatement *ParserIfStatement(Parser *parser);
PrintStatement *ParserPrintStatement(Parser *parser);
VariableDeclaration *ParserVariableDeclaration(Parser *parser);
Assignment *ParserAssignment(Parser *parser);

Expression *ParserExpression(Parser *parser);
ArithmeticExpression *ParserArithmeticExpression(Parser *parser);
Term *ParserTerm(Parser *parser);
Factor *ParserFactor(Parser *parser);
TermTail *ParserTermTail(Parser *parser);
ArithmeticExpressionTail *ParserArithmeticExpressionTail(Parser *parser);
OperatorRelational *ParserOperatorRelational(Parser *parser);

Location *cl(Parser *parser);
// void ParserParenthesis(Parser *parser); !TODO!

#endif // PARSER_H
