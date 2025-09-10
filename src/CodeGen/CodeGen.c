#include "./includes/CodeGen.h"

void CodeGen(Ast *ast, const char *outputFileName,
             unsigned short int strategy) {
  if (strategy == CODE_GEN_STRATEGY_C) {
    FILE *outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
      fprintf(stderr, "Error opening file %s\n", outputFileName);
      exit(1);
    }
    fprintf(outputFile, "#include <stdio.h>\n");
    fprintf(outputFile, "#include <stdlib.h>\n");
    fprintf(outputFile, "int main() {\n");
    fprintf(outputFile, "printf(\"Hello, World!\\n\");\n");
    fprintf(outputFile, "return 0;\n");
    fprintf(outputFile, "}\n");
    fclose(outputFile);

    printf("Code generation completed successfully.\n");
    printf("Output file: %s\n", outputFileName);
    printf("Generated code:\n");
  }
}
