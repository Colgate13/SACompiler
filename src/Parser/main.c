#include <stdio.h>
#include <stdlib.h>

#include "../Lexical/includes/lexicalAnalyzer.h"
#include "../Error/includes/Error.h"

#include "./includes/Parser.h"
#include "./includes/Ast.h"

#define MAX_LINE_SIZE 1024

int main(int argc, char *argv[])
{
  if (LOGS == 1)
  {
    printf("Parser\n");
    printf("File attach: %s\n", argv[1]);
    printf("Output file ast: %s\n", argv[2]);
  }

  if (argc < 2)
  {
    throwError(1, "File attach not found\n");
    exit(1);
  }

  if (argc < 3)
  {
    throwError(1, "Output file ast not found\n");
    exit(1);
  }

  LexicalAnalyzer *lexicalAnalyzer = createLexicalAnalyzer(argv[1]);
  Parser *parser = createParser(lexicalAnalyzer);
  ParserProgram(parser);
  AstConsumer(*parser->ast->program, argv[2]);
  closeLexicalAnalyzer(lexicalAnalyzer);

  return 0;
}
