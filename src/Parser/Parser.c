#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// #include "/home/gabriel/projects/SACompiler/src/Lexical/includes/lexicalAnalyzer.h"
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
    "int",
    "float",
    "string"
};

Parser *createParser(LexicalAnalyzer *lexicalAnalyzer)
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));

    if (parser == NULL)
    {
        return NULL;
    }

    parser->lexicalAnalyzer = lexicalAnalyzer;
    parser->ast = (Ast *)malloc(sizeof(Ast));

    if (parser->ast == NULL)
    {
        return NULL;
    }

    return parser;
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
    Program program;
    program = createProgram(createLocation("*file*", 1, 1));
    parser->ast->program = &program;
    parser->token = nextToken(parser->lexicalAnalyzer);

    if (
        checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0 ||
        strcmp(parser->token.value, keywords[PROGRAM]) == 0)
    {
        logToken(parser);
        parser->ast->program->statements = ParserStatement(parser);
        if ((parser->token.value && strcmp(parser->token.value, keywords[END]) == 0) || checkToken(parser, "TOKEN_TYPE_END") == 0)
        {
            return;
        }

        Statement *currentStatement = parser->ast->program->statements;
        while (1)
        {
            Statement *newStatement = ParserStatement(parser);

            if (newStatement == NULL)
            {
                return;
            }

            currentStatement->next = newStatement;
            currentStatement = currentStatement->next;
            if ((parser->token.value && strcmp(parser->token.value, keywords[END]) == 0) || checkToken(parser, "TOKEN_TYPE_END") == 0)
            {
                return;
            }
        }
    }
}

/**
 * @details Implements <statement>
 */
Statement *ParserStatement(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    if ((parser->token.value && strcmp(parser->token.value, keywords[END]) == 0) || checkToken(parser, "TOKEN_TYPE_END") == 0)
    {
        return NULL;
    }

    if (strcmp(parser->token.value, keywords[PRINT]) == 0)
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
        throwError(1, "Expected print_statement , variable_declaration or assignment\n");
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
            throwError(1, "Expected )\n");
            exit(1);
        }
    }
    else
    {
        throwError(1, "Expected identifier\n");
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
        throwError(1, "Expected type\n");
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

    throwError(1, "Expected TOKEN_TYPE_IDENTIFIER\n");
    exit(1);
}

Expression *ParserExpression(Parser *parser)
{
    controlNextToken(parser);
    logToken(parser);

    if (
        checkToken(parser, "TOKEN_TYPE_NUMBER") == 0 || checkToken(parser, "TOKEN_TYPE_STRING") == 0 || checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0)
    {
        Term *firstTerm = ParserTerm(parser);
        controlNextToken(parser);
        logToken(parser);

        if (checkToken(parser, "TOKEN_TYPE_OPERATOR") == 0)
        {
            Expression *exprTermTail = createExpression_Term_ExpressionTail(
                cl(parser), firstTerm, ParserExpressionTail(parser, 0));

            return exprTermTail;
        }

        Expression *exprTerm = createExpression_Term(cl(parser), firstTerm);
        return exprTerm;
    }
    else
    {
        throwError(1, "Expected TOKEN_TYPE_NUMBER\n");
        exit(1);
    }
}

ExpressionTail *ParserExpressionTail(Parser *parser, unsigned short int recursiveControl)
{
    if (recursiveControl == 1)
    {
        controlNextToken(parser);
        logToken(parser);
    }

    if (checkToken(parser, "TOKEN_TYPE_OPERATOR") == 0)
    {
        char op = *parser->token.value;
        controlNextToken(parser);
        logToken(parser);

        // <term> --> <number> | <string> |<identifier>
        if (checkToken(parser, "TOKEN_TYPE_NUMBER") == 0 || checkToken(parser, "TOKEN_TYPE_STRING") == 0 || checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0)
        {
            ExpressionTail *exprTail = createExpressionTail(
                cl(parser), op, ParserTerm(parser), ParserExpressionTail(parser, 1));
            return exprTail;
        }
        else
        {
            throwError(1, "Expected TOKEN_TYPE_NUMBER, TOKEN_TYPE_STRING or TOKEN_TYPE_IDENTIFIER\n");
            exit(1);
        }
    }

    return NULL;
}

Term *ParserTerm(Parser *parser)
{
    if (checkToken(parser, "TOKEN_TYPE_NUMBER") == 0)
    {
        Number *number = strchr(parser->token.value, '.') != NULL
                             ? createNumber(cl(parser), atof(parser->token.value))
                             : createNumber(cl(parser), atoi(parser->token.value));

        return createTerm_number(
            cl(parser), number);
    }
    else if (checkToken(parser, "TOKEN_TYPE_STRING") == 0)
    {
        return createTerm_string(
            cl(parser), createString(cl(parser), removeQuotes(parser->token.value)));
    }
    else if (checkToken(parser, "TOKEN_TYPE_IDENTIFIER") == 0)
    {
        return createTerm_identifier(
            cl(parser), createIdentifier(cl(parser), parser->token.value));
    }
    else
    {
        throwError(1, "Expected TOKEN_TYPE_NUMBER, TOKEN_TYPE_STRING or TOKEN_TYPE_IDENTIFIER\n");
        exit(1);
    }
}
