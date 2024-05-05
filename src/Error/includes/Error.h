#ifndef ERROR_H
#define ERROR_H

typedef struct
{
    int line;
    int column;
    char *lexeme;
} ErrorMetadata;

typedef struct
{
    int code;
    char *message;
    ErrorMetadata metadata;
} LexicalError;

typedef struct
{
    int code;
    char *message;
    ErrorMetadata metadata;
} ParserError;

typedef struct
{
    int code;
    char *message;
} Error;

void throwError(int code, char *message);
void throwParserError(int code, char *message, int line, int column, char *contentLine);
void throwLexicalError(int code, char *message, int line, int column, char *lexeme);

#endif