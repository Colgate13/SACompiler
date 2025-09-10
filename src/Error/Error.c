#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./includes/Error.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

void removeNewline(char *str) {
  unsigned long length = strlen(str);
  if (length > 0 && str[length - 1] == '\n') {
    str[length - 1] = '\0';
  }
}

void printArrow(int column) {
  for (int i = 0; i < column; i++) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, ANSI_COLOR_RED "^" ANSI_COLOR_RESET "\n");
}

void throwError(int code, char *message) {

  Error Error = {.code = code, .message = message};

  printf("<SACompiler> Error: %d with message: %s\n", Error.code,
         Error.message);
  exit(Error.code);
}

void throwParserError(int code, char *message, int line, int column,
                      char *contentLine) {
  removeNewline(contentLine);
  ErrorMetadata errorMetadata = {
      .column = column, .line = line, .lexeme = contentLine};

  ParserError Error = {
      .code = code, .message = message, .metadata = errorMetadata};

  fprintf(stderr, ANSI_COLOR_RED "<SACompiler::Parser> Error (%d): %s\n",
          Error.code, Error.message);
  fprintf(stderr, ANSI_COLOR_RESET "Position: Line %d, Column %d\n",
          Error.metadata.line + 1, Error.metadata.column + 1);
  fprintf(stderr, "%s\n", contentLine);
  printArrow(Error.metadata.column - 1);

  exit(Error.code);
}

void throwLexicalError(int code, char *message, int line, int column,
                       char *contentLine) {
  removeNewline(contentLine);
  ErrorMetadata errorMetadata = {
      .column = column, .line = line, .lexeme = contentLine};

  LexicalError Error = {
      .code = code, .message = message, .metadata = errorMetadata};

  fprintf(stderr, ANSI_COLOR_RED "<SACompiler::Lexical> Error (%d): %s\n",
          Error.code, Error.message);
  fprintf(stderr, ANSI_COLOR_RESET "Position: Line %d, Column %d\n",
          Error.metadata.line + 1, Error.metadata.column + 1);
  fprintf(stderr, "%s\n", contentLine);
  printArrow(Error.metadata.column);

  exit(Error.code);
}
