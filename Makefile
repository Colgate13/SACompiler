# Compiler and Linker
CC = clang

# Directories
BIN_DIR = bin
LEXICAL_DIR = src/Lexical
PARSER_DIR = src/Parser
SEMANTIC_DIR = src/Semantic
ERROR_DIR = src/Error

# Paths to include headers and libraries
LIBS_ARGS = -Ilibs/includes -Llibs -lcjson

# Compiler flags
CFLAGS = -g -Wall -Wextra

# Source files for different components
LEXICAL_SOURCES = $(LEXICAL_DIR)/Scanners.c $(LEXICAL_DIR)/Token.c $(LEXICAL_DIR)/Utils.c $(LEXICAL_DIR)/lexicalAnalyzer.c
PARSER_SOURCES = $(PARSER_DIR)/Parser.c $(PARSER_DIR)/Ast.c $(PARSER_DIR)/AstJson.c $(PARSER_DIR)/Utils.c
SEMANTIC_SOURCES = $(SEMANTIC_DIR)/Semantic.c
ERROR_SOURCES = $(ERROR_DIR)/Error.c
SACompiler = $(ERROR_SOURCES) $(LEXICAL_SOURCES) $(PARSER_SOURCES) $(SEMANTIC_SOURCES) src/CodeGen/CodeGen.c src/main.c

# Top-level rule to create the binary directory and build the main program
all: $(BIN_DIR) SACompiler

# Build the main compiler executable
SACompiler: $(SACompiler)
	@echo "Building SACompiler..."
	$(CC) $(CFLAGS) -o $(BIN_DIR)/SACompiler $^ $(LIBS_ARGS)

# Rule to create the binary directory if it does not exist
$(BIN_DIR):
	@echo "Creating binary directory..."
	mkdir -p $@

# Clean up build artifacts
clean:
	@echo "Cleaning up..."
	rm -rf $(BIN_DIR)

# Refresh shared library links
cleanCache:
	@echo "Refreshing shared library cache..."
	sudo ldconfig
