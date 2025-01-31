#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "../Lexical/includes/lexicalAnalyzer.h"

#include "../Error/includes/Error.h"
#include "./includes/Parser.h"
#include "./includes/Ast.h"
#include "./includes/Utils.h"

const char *keywords[] = {
    "program",
    "end",
    "var",
    "print",
    "if",
    "int",
    "float",
    "string"
};

Parser *createParser(LexicalAnalyzer *lexicalAnalyzer)
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));

    if (parser == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    parser->lexicalAnalyzer = lexicalAnalyzer;
    parser->ast = (Ast *)malloc(sizeof(Ast));

    if (parser->ast == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    return parser;
}

void destroyParser(Parser *parser)
{
    free(parser->ast);
    free(parser);
}

void controlNextToken(Parser *parser)
{
    parser->token = nextToken(parser->lexicalAnalyzer);

    if (checkToken(parser, "TOKEN_TYPE_END_LINE") == 0)
        parser->token = nextToken(parser->lexicalAnalyzer);
}

/**
 * @details Implements <program>
 *
 * Rules -> Terminals initialize with lowercase
 *       -> Non-terminals initialize with uppercase
 */
void ParserProgram(Parser *parser)
{
    parser->ast->program = createProgram(createLocation("*file*", 1, 1));
    parser->token = nextToken(parser->lexicalAnalyzer);

    if (checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0 &&
        strcmp(parser->token.value, keywords[PROGRAM]) == 0)
    {
        parser->ast->program->statement_tail = ParserStatementTail(parser);
    }
    else
    {
        throwParserError(1, "Expected program\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }
}

/**
 * @details Implements <statement_tail>
 */
StatementTail *ParserStatementTail(Parser *parser)
{
    StatementTail *statementTail = createStatementTail(
        cl(parser), ParserStatement(parser));

    if (statementTail->statement == NULL)
    {
        return statementTail;
    }
    else
    {
        statementTail->next = ParserStatementTail(parser);
        return statementTail;
    }
}

/**
 * @details Implements <block>
 */
Block *ParserBlock(Parser *parser)
{
    if (checkToken(parser, "TOKEN_TYPE_LEFT_BRACES") == 0)
    {
        StatementTail *statementTail = ParserStatementTail(parser);

        if (checkToken(parser, "TOKEN_TYPE_RIGHT_BRACES") == 0)
        {
            return createBlock(
                cl(parser), statementTail);
        }
        else
        {
            throwParserError(1, "Expected }\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
            exit(1);
        }
    }
    else
    {
        throwParserError(1, "Expected {\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }
}

/**
 * @details Implements <statement>
 */
Statement *ParserStatement(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    // Exit condition for <statement_tail> if the token is "end"
    if ((parser->token.value && strcmp(parser->token.value, keywords[END]) == 0) || checkToken(parser, "TOKEN_TYPE_END") == 0)
    {
        return NULL;
    }

    // Exit condition for <statement_tail> if the token is "}"
    if (checkToken(parser, "TOKEN_TYPE_RIGHT_BRACES") == 0)
    {
        return NULL;
    }

    if (strcmp(parser->token.value, keywords[IF]) == 0)
    {
        return createStatement_IfStatement(
            cl(parser),
            ParserIfStatement(parser));
    }
    else if (strcmp(parser->token.value, keywords[PRINT]) == 0)
    {
        return createStatement_PrintStatement(
            cl(parser),
            ParserPrintStatement(parser));
    }
    else if (strcmp(parser->token.value, keywords[VAR]) == 0)
    {
        return createStatement_VariableDeclaration(
            cl(parser),
            ParserVariableDeclaration(parser));
    }
    else if (checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0)
    {
        return createStatement_Assignment(
            cl(parser),
            ParserAssignment(parser));
    }
    else
    {
        throwParserError(1, "Expected print_statement , variable_declaration or assignment\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }
}

/**
 * @details Implements <print_statement>
 */
PrintStatement *ParserPrintStatement(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    if (checkToken(parser, "TOKEN_TYPE_LEFT_PARENTHESIS") == 0)
    {

        PrintStatement *printStatement = createPrintStatement(
            cl(parser), ParserExpression(parser));

        if (checkToken(parser, "TOKEN_TYPE_RIGHT_PARENTHESIS") == 0)
        {
            controlNextToken(parser);
            logToken(parser);
            return printStatement;
        }
        else
        {
            throwParserError(1, "Expected )\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
            exit(1);
        }
    }
    else
    {
        throwParserError(1, "Expected identifier\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }
}

/**
 * @details Implements <variable_declaration>
 */
VariableDeclaration *ParserVariableDeclaration(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    unsigned short int type;
    if (strcmp(parser->token.value, keywords[INT]) == 0)
    {
        type = TYPE_INT;
    }
    else if (strcmp(parser->token.value, keywords[FLOAT]) == 0)
    {
        type = TYPE_FLOAT;
    }
    else if (strcmp(parser->token.value, keywords[STRING]) == 0)
    {
        type = TYPE_STRING;
    }
    else
    {
        throwParserError(1, "Expected type\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }

    controlNextToken(parser);
    logToken(parser);

    Identifier *identifier = createIdentifier(
        cl(parser), parser->token.value);

    VariableDeclaration *variableDeclaration = createVariableDeclaration(
        cl(parser), type, identifier);

    controlNextToken(parser);
    logToken(parser);

    return variableDeclaration;
}

/**
 * @details Implements <assignment>
 */
Assignment *ParserAssignment(Parser *parser)
{
    if (checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0)
    {
        Identifier *identifier = createIdentifier(
            cl(parser), parser->token.value);

        controlNextToken(parser);
        logToken(parser);

        if (checkToken(parser, "TOKEN_TYPE_OPERATOR") == 0 &&
            strcmp(parser->token.value, "=") == 0)
        {
            Expression *expr = ParserExpression(parser);

            return createAssignment(
                cl(parser), identifier, expr);
        }
    }

    throwParserError(1, "Expected TOKEN_TYPE_IDENTIFIER\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
    exit(1);
}

/**
 * @details Implements <if_statement>
 */
IfStatement *ParserIfStatement(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    if (checkToken(parser, "TOKEN_TYPE_LEFT_PARENTHESIS") == 0)
    {
        Expression *expr = ParserExpression(parser);

        if (checkToken(parser, "TOKEN_TYPE_RIGHT_PARENTHESIS") == 0)
        {
            controlNextToken(parser);
            logToken(parser);

            Block *block = ParserBlock(parser);

            return createIfStatement(
                cl(parser), expr, block);
        }
        else
        {
            throwParserError(1, "Expected )\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
            exit(1);
        }
    }
    else
    {
        throwParserError(1, "Expected (\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }
}

Expression *ParserExpression(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    return createExpression(
        cl(parser), ParserArithmeticExpression(parser), ParserOperatorRelational(parser));
}

OperatorRelational *ParserOperatorRelational(Parser *parser)
{
    if (checkToken(parser, "TOKEN_TYPE_OPERATOR") == 0 &&
        (strcmp(parser->token.value, "==") == 0 || strcmp(parser->token.value, "!=") == 0 || strcmp(parser->token.value, ">") == 0 || strcmp(parser->token.value, "<") == 0 || strcmp(parser->token.value, ">=") == 0 || strcmp(parser->token.value, "<=") == 0))
    {
        char *operator = parser->token.value;

        controlNextToken(parser);
        logToken(parser);

        return createOperatorRelational(
            cl(parser), operator, ParserArithmeticExpression(parser));
    }

    return NULL;
}

ArithmeticExpression *ParserArithmeticExpression(Parser *parser)
{
    return createArithmeticExpression(
        cl(parser), ParserTerm(parser), ParserArithmeticExpressionTail(parser));
}

ArithmeticExpressionTail *ParserArithmeticExpressionTail(Parser *parser)
{
    if (checkToken(parser, "TOKEN_TYPE_OPERATOR") == 0 &&
        (strcmp(parser->token.value, "+") == 0 || strcmp(parser->token.value, "-") == 0))
    {
        char *operator = parser->token.value;

        controlNextToken(parser);
        logToken(parser);

        return createArithmeticExpressionTail(
            cl(parser), operator, ParserTerm(parser), ParserArithmeticExpressionTail(parser));
    }

    return NULL;
}

Term *ParserTerm(Parser *parser)
{
    return createTerm(
        cl(parser), ParserFactor(parser), ParserTermTail(parser));
}

Factor *ParserFactor(Parser *parser)
{
    // "(" <expression> ")"
    if (checkToken(parser, "TOKEN_TYPE_LEFT_PARENTHESIS") == 0)
    {
        controlNextToken(parser);
        logToken(parser);

        Expression *expr = ParserExpression(parser);

        if (checkToken(parser, "TOKEN_TYPE_RIGHT_PARENTHESIS") == 0)
        {
            controlNextToken(parser);
            logToken(parser);

            return createFactor_Expression(
                cl(parser), expr);
        }
        else
        {
            throwParserError(1, "Expected )\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
            exit(1);
        }
    }
    // <number>
    else if (checkToken(parser, "TOKEN_TYPE_NUMBER") == 0)
    {
        /**
         * @important
         * 
         * The atoi() function converts a string to an integer.
         * 
         * Floating point numbers are truncated to integer.
         */
        Number *number = createNumber(
            cl(parser), atoi(parser->token.value));

        controlNextToken(parser);
        logToken(parser);

        return createFactor_Number(
            cl(parser), number);
    }
    // <identifier>
    else if (checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0)
    {
        Identifier *identifier = createIdentifier(
            cl(parser), parser->token.value);

        controlNextToken(parser);
        logToken(parser);

        return createFactor_Identifier(
            cl(parser), identifier);
    }
    // <string>
    else if (checkToken(parser, "TOKEN_TYPE_STRING") == 0)
    {
        String *string = createString(
            cl(parser), parser->token.value);

        controlNextToken(parser);
        logToken(parser);

        return createFactor_String(
            cl(parser), string);
    }
    else
    {
        throwParserError(1, "Expected number, identifier or string\n", parser->lexicalAnalyzer->lineCount, parser->lexicalAnalyzer->positionCount, parser->lexicalAnalyzer->line);
        exit(1);
    }
}

TermTail *ParserTermTail(Parser *parser)
{
    if (checkToken(parser, "TOKEN_TYPE_OPERATOR") == 0 &&
        (strcmp(parser->token.value, "*") == 0 || strcmp(parser->token.value, "/") == 0 || strcmp(parser->token.value, "%") == 0))
    {
        char *operator = parser->token.value;

        controlNextToken(parser);
        logToken(parser);

        return createTermTail(
            cl(parser), operator, ParserFactor(parser), ParserTermTail(parser));
    }

    return NULL;
}