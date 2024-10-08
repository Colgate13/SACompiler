#include <stdio.h>
#include <stdlib.h>
#include "./Lexical/includes/lexicalAnalyzer.h"
#include "./Error/includes/Error.h"
#include "./Parser/includes/Parser.h"
#include "./Parser/includes/Ast.h"

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

  // Create lexical analyzer
  LexicalAnalyzer *lexicalAnalyzer = createLexicalAnalyzer(argv[1]);

  // Create parser using lexical analyzer
  Parser *parser = createParser(lexicalAnalyzer);
  ParserProgram(parser);

  // Create output file (ast-output.json) only for test and debug
  if (LOGS == 1)
  {
    AstConsumer(*parser->ast->program, argv[2]);
  }

  // Close lexical analyzer
  closeLexicalAnalyzer(lexicalAnalyzer);

  return 0;
}
