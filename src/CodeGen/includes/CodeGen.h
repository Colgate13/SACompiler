#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Parser/includes/Parser.h"

enum ECodeGenEstrategies
{
    CODE_GEN_STRATEGY_C = 1
};

void CodeGen(Ast *ast, const char *outputFileName, unsigned short int strategy);

#endif