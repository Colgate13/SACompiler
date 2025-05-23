#include "./includes/Token.h"
#include <stdlib.h>

Token constructToken(unsigned short int type, char *value)
{
    Token token;
    token.type = type;
    token.value = value;

    return token;
}

// #static function
char *tokenTypeName(unsigned short int typeId)
{
    switch (typeId)
    {
        case 0:
            return "TOKEN_TYPE_IDENTIFIER";
        break;
        case 1:
            return "TOKEN_TYPE_NUMBER";
        break;
        case 2:
            return "TOKEN_TYPE_OPERATOR";
        break;
        case 3:
            return "TOKEN_TYPE_SEMICOLON";
        break;
        case 4:
            return "TOKEN_TYPE_ASSIGN";
        break;
        case 5:
            return "TOKEN_TYPE_KEYWORDS";
        break;
        case 6:
            return "TOKEN_TYPE_STRING";
        break;
        case 7:
            return "TOKEN_TYPE_RIGHT_PARENTHESIS";
        break;
        case 8:
            return "TOKEN_TYPE_LEFT_PARENTHESIS";
        case 9:
            return "TOKEN_TYPE_LEFT_BRACES";
        case 10:
            return "TOKEN_TYPE_RIGHT_BRACES";
        break;
        case 1000:
            return "TOKEN_TYPE_INITIALIZE";
        break;
        case 1001:
            return "TOKEN_TYPE_END";
        break;
        case 1002:
            return "TOKEN_TYPE_END_LINE";
        break;
        default:
            return "INVALID_TOKEN_TYPE";
        break;
    }
}

void destructToken(Token *token)
{
    free(token);
}

void destructTokenNode(Token *token)
{
    free(token->value);
}

unsigned short int getType(Token *token)
{
    return token->type;
}

void setType(Token *token, unsigned short int type)
{
    token->type = type;
}

char *getValue(Token *token)
{
    return token->value;
}

void setValue(Token *token, char *value)
{
    token->value = value;
}
