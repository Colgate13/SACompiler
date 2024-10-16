#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "./Parser.h"
#include "./Ast.h"
#include "../../Lexical/includes/lexicalAnalyzer.h"
#include <string.h>

unsigned short int checkToken(Parser *parser, char *tokenType);
unsigned short int checkValue(Parser *parser, char *value);
char *removeQuotes(char *str);
void logToken(Parser *parser);
Location *cl(Parser *parser);

#endif