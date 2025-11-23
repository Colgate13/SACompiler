#include "./includes/Utils.h"

unsigned short int checkToken(Parser *parser, char *tokenType) {
  return strcmp(tokenTypeName(parser->token.type), tokenType);
}

unsigned short int checkValue(Parser *parser, char *value) {
  return strcmp(parser->token.value, value);
}

char *removeQuotes(char *str) {
  int i, j;
  for (i = 0, j = 0; str[i] != '\0'; i++) {
    if (str[i] != '\"') {
      str[j++] = str[i];
    }
  }
  str[j] = '\0';
  return str;
}

void logToken(Parser *parser) {
  if (LOGS == 1) {
    printf("Token: %s, value: %s\n", tokenTypeName(parser->token.type),
           parser->token.value);
  }
}

Location *cl(Parser *parser) {
  return createLocation(parser->lexicalAnalyzer->filePath, parser->lexicalAnalyzer->lineCount,
                        parser->lexicalAnalyzer->positionCount);
}
