#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include "../../Parser/includes/Parser.h"

typedef struct
{
  Parser *parser;
} Semantic;

Semantic *createSemantic(Parser *parser);
void destroySemantic(Semantic *semantic);

#endif