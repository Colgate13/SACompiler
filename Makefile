# Compiler and Linker
CC = clang

# Directories
BIN_DIR = bin
LEXICAL_DIR = src/Lexical
PARSER_DIR = src/Parser
ERROR_DIR = src/Error

# Paths to include headers and libraries
LIB_INCLUDE = -I/usr/local/include
LIB_PATH = -L/usr/local/lib
LIBS_ALL = -lcjson

# Compiler flags
CFLAGS = -g -Wall -Wextra $(LIB_INCLUDE) $(LIBS_ALL)

# Source files for different components
LEXICAL_SOURCES = $(LEXICAL_DIR)/Scanners.c $(LEXICAL_DIR)/Token.c $(LEXICAL_DIR)/Utils.c $(LEXICAL_DIR)/lexicalAnalyzer.c
PARSER_SOURCES = $(PARSER_DIR)/Parser.c $(PARSER_DIR)/Ast.c $(PARSER_DIR)/Utils.c
ERROR_SOURCES = $(ERROR_DIR)/Error.c
SACompiler = $(ERROR_SOURCES) $(LEXICAL_SOURCES) $(PARSER_SOURCES) src/main.c

# Top-level rule to create the binary directory and build the main program
all: $(BIN_DIR) SACompiler

# Build the main compiler executable
SACompiler: $(SACompiler)
	@echo "Building SACompiler..."
	$(CC) $(CFLAGS) -o $(BIN_DIR)/SACompiler $^

# Build the lexical analyzer standalone executable
lexical: $(ERROR_SOURCES) $(LEXICAL_SOURCES) $(LEXICAL_DIR)/main.c
	@echo "Building lexical analyzer..."
	$(CC) $(CFLAGS) -o $(BIN_DIR)/lexical $^

# Build the parser standalone executable
parser: $(ERROR_SOURCES) $(LEXICAL_SOURCES) $(PARSER_SOURCES) $(PARSER_DIR)/main.c
	@echo "Building parser..."
	$(CC) $(CFLAGS) -o $(BIN_DIR)/parser $^

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
