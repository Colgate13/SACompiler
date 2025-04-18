#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Error/includes/Error.h"
#include "./includes/Scanners.h"
#include "./includes/Token.h"
#include "./includes/Utils.h"
#include "./includes/lexicalAnalyzer.h"

Token nextStageState(LexicalAnalyzer *lexicalAnalyzer)
{
  if (lexicalAnalyzer->newLine)
  {
    lexicalAnalyzer->positionCount = 0;
    lexicalAnalyzer->lineCount++;
  }

  char *term = (char *)malloc(MAX_TERM_SIZE * sizeof(char));

  if (term == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  unsigned short int currentState = INITIAL_STATE;

  while (lexicalAnalyzer->positionCount <= strlen(lexicalAnalyzer->line) + 1)
  {
    char currentChar = lexicalAnalyzer->line[lexicalAnalyzer->positionCount];

    switch (currentState)
    {
    case INITIAL_STATE:
      if (isLetter(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 1;
      }
      else if (isDigit(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 3;
      }
      else if (isOperator(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 5;
      }
      else if (isSemicolon(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 8;
      }
      else if (isSpace(currentChar))
      {
        currentState = 0;
      }
      else if (isNewLine(currentChar))
      {
        currentState = 9;
      }
      else if (isString(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 11;
      }
      else if (isRightParenthesis(currentChar))
      {
        strncat(term, &currentChar, 1);

        currentState = 13;
      }
      else if (isLeftParenthesis(currentChar))

      {
        strncat(term, &currentChar, 1);
        currentState = 14;
      }
      else if (isRightBrace(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 15;
      }
      else if (isLeftBrace(currentChar))
      {
        strncat(term, &currentChar, 1);
        currentState = 16;
      }
      else if (isEOF(currentChar))
      {
        currentState = 9;
      }
      else
      {
        throwLexicalError(1, "Error: Invalid character", lexicalAnalyzer->lineCount, lexicalAnalyzer->positionCount, lexicalAnalyzer->line);
        exit(1);
      }
      break;
    case 1:
      if (isLetter(currentChar) || isDigit(currentChar))
      {
        currentState = 1;
        strncat(term, &currentChar, 1);
      }
      else
      {
        currentState = 2;
      }
      break;
    case 3:
      if (isDigit(currentChar) || currentChar == '.')
      {
        currentState = 3;
        strncat(term, &currentChar, 1);
      }
      else
      {
        currentState = 4;
      }
      break;
    case 5:
      if (isOperator(currentChar))
      {
        currentState = 7;
        strncat(term, &currentChar, 1);
      }
      else
      {
        currentState = 6;
      }
      break;
    case 11:
      if (isString(currentChar))
      {
        strncat(term, &currentChar, 1);

        currentState = 12;
      }
      else
      {
        currentState = 11;
        strncat(term, &currentChar, 1);
      }
      break;
    case 2:
      lexicalAnalyzer->positionCount--;
      return constructToken(TOKEN_TYPE_IDENTIFIER, term);
      break;
    case 4:
      lexicalAnalyzer->positionCount--;
      return constructToken(TOKEN_TYPE_NUMBER, term);
      break;
    case 6:
      lexicalAnalyzer->positionCount--;
      return constructToken(TOKEN_TYPE_OPERATOR, term);
      break;
    case 7:
      return constructToken(TOKEN_TYPE_OPERATOR, term);
      break;
    case 8:
      return constructToken(TOKEN_TYPE_SEMICOLON, term);
      break;
    case 9:
      return constructToken(TOKEN_TYPE_END_LINE, NULL);
      break;
    case 12:
      return constructToken(TOKEN_TYPE_STRING, term);
      break;
    case 13:
      return constructToken(TOKEN_TYPE_RIGHT_PARENTHESIS, term);
      break;
    case 14:
      return constructToken(TOKEN_TYPE_LEFT_PARENTHESIS, term);
      break;
    case 15:
      return constructToken(TOKEN_TYPE_RIGHT_BRACES, term);
      break;
    case 16:
      return constructToken(TOKEN_TYPE_LEFT_BRACES, term);
      break;
    default:
      throwLexicalError(1, "Error: Invalid character", lexicalAnalyzer->lineCount, lexicalAnalyzer->positionCount, term);
      exit(1);
    }

    lexicalAnalyzer->positionCount++;
  }

  free(term);
  return constructToken(TOKEN_TYPE_END_LINE, NULL);
}

Token nextToken(LexicalAnalyzer *lexicalAnalyzer)
{

  if (lexicalAnalyzer->isEOF)
  {
    Token token = constructToken(TOKEN_TYPE_END, NULL);
    return token;
  }

  if (lexicalAnalyzer->globalTokensCount == 0 && !lexicalAnalyzer->line)
  {
    lexicalAnalyzer->line = readLine(lexicalAnalyzer->file, MAX_LINE_SIZE);
  }

  Token token = token = nextStageState(lexicalAnalyzer);

  if (token.type == TOKEN_TYPE_END_LINE)
  {
    char *line = (char *)malloc(MAX_LINE_SIZE * sizeof(char));

    if (line == NULL)
    {
      fprintf(stderr, "Memory allocation error\n");
      exit(1);
    }

    line = readLine(lexicalAnalyzer->file, MAX_LINE_SIZE);

    if (line == NULL)
    {
      lexicalAnalyzer->isEOF = 1;
    }

    lexicalAnalyzer->line = line;
    lexicalAnalyzer->newLine = 1;
    token = nextToken(lexicalAnalyzer);
  }
  else
  {
    lexicalAnalyzer->newLine = 0;
  }

  lexicalAnalyzer->globalTokensCount++;

  return token;
}

LexicalAnalyzer *createLexicalAnalyzer(const char *filePath)
{
  FILE *attachFile = fopen(filePath, "r");

  if (attachFile == NULL)
  {
    throwError(1, "Error: File not found\n");
    exit(1);
  }

  LexicalAnalyzer *lexicalAnalyzer = (LexicalAnalyzer *)malloc(sizeof(LexicalAnalyzer));

  if (lexicalAnalyzer == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  lexicalAnalyzer->file = attachFile;
  lexicalAnalyzer->newLine = 0;
  lexicalAnalyzer->globalTokensCount = 0;
  lexicalAnalyzer->line = NULL;
  lexicalAnalyzer->isEOF = 0;

  return lexicalAnalyzer;
}

void destroyLexicalAnalyzer(LexicalAnalyzer *lexicalAnalyzer)
{
  fclose(lexicalAnalyzer->file);
  free(lexicalAnalyzer->line);
  free(lexicalAnalyzer);
}
