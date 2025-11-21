# Sistema de Gerenciamento de Memória (Rust-like)

Este documento descreve a arquitetura para implementar um sistema de gerenciamento automático de memória inspirado no Rust, onde variáveis são liberadas automaticamente após seu último uso.

## Visão Geral

O objetivo é inserir `free()` automaticamente no código gerado, liberando memória assim que uma variável não for mais necessária.

```
Source Code → Lexer → Parser → Semantic → Liveness Analysis → CodeGen (com frees)
```

---

## 1. Extensão da Symbol Table

### Estrutura Atual

```c
typedef struct Symbol {
  char *name;
  SymbolKind kind;
  Type type;
  unsigned short int isUsed;
  Location *location;
  struct Symbol *next;
} Symbol;
```

### Estrutura Proposta

```c
typedef struct UsePoint {
  Location *location;           // onde foi usada
  unsigned int statementIndex;  // índice do statement
  struct UsePoint *next;
} UsePoint;

typedef struct Symbol {
  char *name;
  SymbolKind kind;
  Type type;

  // === NOVO: Tracking de uso ===
  UsePoint *uses;               // lista de todos os usos
  UsePoint *lastUse;            // ponteiro para o último uso
  unsigned int useCount;        // quantas vezes foi usada
  unsigned short int isUsed;

  // === NOVO: Memory management ===
  unsigned short int needsFree; // se precisa de free (heap allocated)
  unsigned short int wasFreed;  // se já foi liberada

  Location *location;           // onde foi declarada
  struct Symbol *next;
} Symbol;
```

---

## 2. Coleta de Usos Durante Análise Semântica

### Função para Registrar Uso

```c
void recordUse(Symbol *symbol, Location *loc, unsigned int stmtIndex) {
  UsePoint *use = malloc(sizeof(UsePoint));
  use->location = malloc(sizeof(Location));
  use->location->line = loc->line;
  use->location->column = loc->column;
  use->location->fileName = strdup(loc->fileName);
  use->statementIndex = stmtIndex;
  use->next = NULL;

  // Adiciona no final da lista
  if (symbol->uses == NULL) {
    symbol->uses = use;
  } else {
    UsePoint *current = symbol->uses;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = use;
  }

  symbol->useCount++;
  symbol->lastUse = use;  // sempre atualiza para o mais recente
}
```

### Modificação em `inferFactorType`

```c
Type inferFactorType(SymbolTable *stack, Factor *factor, unsigned int stmtIndex) {
  // ... código existente ...

  if (factor->identifier != NULL) {
    Symbol *symbol = lookupSymbol(stack, factor->identifier->name);
    if (!symbol) {
      fprintf(stderr, "Error: variable '%s' not declared\n",
              factor->identifier->name);
      exit(1);
    }

    // NOVO: Registra o uso
    recordUse(symbol, factor->location, stmtIndex);

    symbol->isUsed = 1;
    return symbol->type;
  }

  // ... resto do código ...
}
```

---

## 3. Determinação de Alocação (Heap vs Stack)

| Tipo | Alocação em C | Precisa `free()`? |
|------|---------------|-------------------|
| `int` | Stack: `int x;` | Não |
| `double` | Stack: `double x;` | Não |
| `string` | Heap: `char *s = strdup(...);` | **Sim** |

### Função para Determinar Necessidade de Free

```c
void determineNeedsFree(Symbol *symbol) {
  switch (symbol->type) {
    case TYPE_STRING:
      symbol->needsFree = 1;
      break;
    case TYPE_INT:
    case TYPE_DOUBLE:
    default:
      symbol->needsFree = 0;
      break;
  }
}
```

---

## 4. Análise de Liveness (Nova Fase)

### Estrutura

```c
typedef struct LivenessAnalyzer {
  SymbolTable *symbolTable;
  Ast *ast;
} LivenessAnalyzer;

LivenessAnalyzer *createLivenessAnalyzer(SymbolTable *st, Ast *ast);
void analyzeLiveness(LivenessAnalyzer *analyzer);
void destroyLivenessAnalyzer(LivenessAnalyzer *analyzer);
```

### Algoritmo Básico

```c
void analyzeLiveness(LivenessAnalyzer *analyzer) {
  // Para cada símbolo na tabela
  SymbolTable *scope = analyzer->symbolTable;

  while (scope != NULL) {
    Symbol *sym = scope->symbols;

    while (sym != NULL) {
      // O lastUse já foi calculado durante semantic analysis
      // Aqui podemos fazer validações adicionais

      if (sym->useCount == 0 && sym->isUsed == 0) {
        printf("Warning: variable '%s' declared but never used\n", sym->name);
      }

      determineNeedsFree(sym);

      sym = sym->next;
    }

    scope = scope->next;
  }
}
```

---

## 5. Code Generation com Frees Automáticos

### Abordagem 1: Free Após Último Uso (Agressivo)

```c
void codegen_statement(FILE *out, Statement *stmt, SymbolTable *symbols,
                       unsigned int stmtIndex) {
  // Emite o código do statement
  switch (stmt->type) {
    case ASSIGNMENT_STATEMENT:
      codegen_assignment(out, stmt->assignment);
      break;
    case PRINT_STATEMENT:
      codegen_print(out, stmt->print_statement);
      break;
    // ...
  }

  // Verifica se alguma variável teve último uso neste statement
  emit_frees_for_statement(out, symbols, stmtIndex);
}

void emit_frees_for_statement(FILE *out, SymbolTable *symbols,
                               unsigned int stmtIndex) {
  SymbolTable *scope = symbols;

  while (scope != NULL) {
    Symbol *sym = scope->symbols;

    while (sym != NULL) {
      if (sym->needsFree &&
          !sym->wasFreed &&
          sym->lastUse != NULL &&
          sym->lastUse->statementIndex == stmtIndex) {

        fprintf(out, "    free(%s); // auto-free após último uso\n", sym->name);
        sym->wasFreed = 1;
      }

      sym = sym->next;
    }

    scope = scope->next;
  }
}
```

### Abordagem 2: Free no Fim do Scope (Mais Simples)

Similar ao RAII do C++. Mais seguro e fácil de implementar:

```c
void codegen_block_end(FILE *out, SymbolTable *scope) {
  Symbol *sym = scope->symbols;

  // Free todas as variáveis declaradas neste scope
  while (sym != NULL) {
    if (sym->needsFree && !sym->wasFreed) {
      fprintf(out, "    free(%s); // auto-free fim do scope\n", sym->name);
      sym->wasFreed = 1;
    }
    sym = sym->next;
  }

  fprintf(out, "}\n");
}
```

---

## 6. Tratamento de Control Flow

### Problema

Com `if` statements, o último uso pode variar dependendo do caminho de execução:

```c
var string msg;
msg = "hello";

if (x) {
    print(msg);  // último uso SE x == true
}

print(msg);      // último uso SE x == false
```

### Solução: Usar o Último Uso no Pior Caso

O free deve vir **após o último uso possível em todos os caminhos**:

```c
// Código gerado:
char *msg = strdup("hello");

if (x) {
    printf("%s\n", msg);
}

printf("%s\n", msg);
free(msg);  // free após o último uso possível
```

### Algoritmo para Control Flow

```c
unsigned int findLastPossibleUse(Symbol *sym, Ast *ast) {
  unsigned int maxIndex = 0;
  UsePoint *use = sym->uses;

  while (use != NULL) {
    if (use->statementIndex > maxIndex) {
      maxIndex = use->statementIndex;
    }
    use = use->next;
  }

  return maxIndex;
}
```

---

## 7. Exemplo Completo

### Input (sua linguagem)

```
program
  var string nome;
  var int idade;

  nome = "Gabriel";
  idade = 25;

  print(nome);
  print(idade);
end;
```

### Output (C gerado com memory management)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *nome;
    int idade;

    nome = strdup("Gabriel");
    idade = 25;

    printf("%s\n", nome);
    printf("%d\n", idade);

    free(nome);  // auto-free: último uso foi no print acima
    // idade não precisa de free (stack allocated)

    return 0;
}
```

---

## 8. Pipeline Completo do Compilador

```
┌─────────────────────────────────────┐
│  1. Lexical Analysis                │
│     Input: Source code              │
│     Output: Token stream            │
└─────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────┐
│  2. Syntax Analysis (Parser)        │
│     Input: Tokens                   │
│     Output: AST                     │
└─────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────┐
│  3. Semantic Analysis               │
│     Input: AST                      │
│     Output: AST + Symbol Table      │
│     - Type checking                 │
│     - Coleta todos os usos          │
│     - Registra lastUse              │
└─────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────┐
│  4. Liveness Analysis (NOVO)        │
│     Input: AST + Symbol Table       │
│     Output: Liveness info           │
│     - Calcula último uso real       │
│     - Considera control flow        │
│     - Marca needsFree               │
└─────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────┐
│  5. Code Generation                 │
│     Input: AST + Symbol Table       │
│            + Liveness info          │
│     Output: Target code (C/ASM)     │
│     - Emite declarações             │
│     - Emite statements              │
│     - Insere free() automático      │
└─────────────────────────────────────┘
```

---

## 9. Arquivos a Criar/Modificar

| Arquivo | Ação | Descrição |
|---------|------|-----------|
| `src/Semantic/includes/Semantic.h` | Modificar | Adicionar `UsePoint`, estender `Symbol` |
| `src/Semantic/Semantic.c` | Modificar | Implementar `recordUse()` |
| `src/Liveness/Liveness.h` | **Criar** | Header da análise de liveness |
| `src/Liveness/Liveness.c` | **Criar** | Implementação da análise |
| `src/CodeGen/CodeGen.c` | Modificar | Usar liveness info para emitir frees |

---

## 10. Referências

- **Liveness Analysis**: https://en.wikipedia.org/wiki/Live_variable_analysis
- **Rust Ownership**: https://doc.rust-lang.org/book/ch04-00-understanding-ownership.html
- **RAII Pattern**: https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization
