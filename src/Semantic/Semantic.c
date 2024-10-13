#include "./includes/Semantic.h"

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