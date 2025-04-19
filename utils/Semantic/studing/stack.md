# SACompiler – Análises Semânticas

## Pilha de Escopos (`SymbolTable* stack`)

No **SACompiler** cada bloco léxico (programa, `if`, função, etc.) possui
seu **próprio escopo**. Esses escopos são guardados em uma *pilha* simples
implementada como **lista simplesmente encadeada** onde o **nó‐cabeça** é o
escopo corrente.

```
Topo da pilha → [ Escopo nível 2 ] → [ Escopo nível 1 ] → [ Escopo nível 0 ] → NULL
```

| Operação            | O que faz                                                   |
| ------------------- | ----------------------------------------------------------- |
| `pushScope`         | Aloca um novo nó de `SymbolTable`, aponta `next` para o nó  
                        atual e o define como topo. Incrementa `scopeLevel`.        |
| `insertSymbol`      | Insere símbolo no **escopo atual** (lista ligada de `Symbol`).|
| `lookupSymbol`      | Varre do topo ao fundo até encontrar o símbolo.             |
| `popScope`          | Remove o topo, desaloca todos seus símbolos, retorna ao     
                        escopo imediatamente externo.                               |

### Ciclo típico

1. **`createSymbolTable`** cria o escopo global vazio.
2. **`pushScope`** é chamado no começo do *semantic pass* ou ao entrar em um
   bloco novo.
3. **`insertSymbol`** registra cada declaração de variável/constante.
4. **`lookupSymbol`** é usado para validar usos de identificadores.
5. **`popScope`** é chamado ao sair do bloco.

## Fluxo Simplificado
```
SemanticAnalysis
  ├─ createSymbolTable()          // Cria escopo global
  ├─ pushScope()                  // Empilha escopo global (nível 0)
  ├─ walk(program)                // Percorre AST
  │    ├─ pushScope()             //   ↳ entra em bloco (ex.: if)
  │    ├─ insertSymbol()          //   ↳ declarações
  │    ├─ lookupSymbol()          //   ↳ usos de símbolos
  │    └─ popScope()              //   ↳ sai do bloco
  └─ destroySymbolTable()         // Limpa memória
```

---

## Trecho de Código Anotado

Abaixo seguem apenas as funções relacionadas à pilha com comentários
explicativos. Sinta‑se à vontade para copiar de volta para o código‐fonte.

```c
// ----------------------------------------------------------------------------
// Cria um escopo vazio (não o empilha!).  Sempre comece pelo escopo global.
// ----------------------------------------------------------------------------
SymbolTable *createSymbolTable(void)
{
    SymbolTable *symbolTable = malloc(sizeof(SymbolTable));
    if (!symbolTable) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    symbolTable->symbols    = NULL; // Nenhum símbolo cadastrado ainda
    symbolTable->scopeLevel = 0;    // Será ajustado por pushScope
    symbolTable->next       = NULL; // Próximo escopo na pilha
    return symbolTable;
}

// ----------------------------------------------------------------------------
// Empilha um novo escopo (frame).  O ponteiro *stack SEMPRE aponta para o
// topo; portanto, basta atualizar o head da lista.
// ----------------------------------------------------------------------------
void pushScope(SymbolTable **stack)
{
    SymbolTable *newScope   = createSymbolTable();

    // Se já havia algo na pilha, herdamos o nível +1; caso contrário, é 0.
    newScope->scopeLevel    = *stack ? (*stack)->scopeLevel + 1 : 0;

    newScope->next          = *stack; // Linka ao escopo anterior
    *stack                  = newScope; // Novo topo da pilha
}

// ----------------------------------------------------------------------------
// Remove o escopo corrente, liberando seus símbolos e voltando ao anterior.
// ----------------------------------------------------------------------------
void popScope(SymbolTable **stack)
{
    if (!*stack) {
        fprintf(stderr, "Semantic: tentativa de pop em pilha vazia\n");
        return;
    }

    SymbolTable *oldScope = *stack;
    *stack = oldScope->next;        // "Desempilha"

    destroySymbolTable(oldScope);   // Libera memória do frame removido
}

// ----------------------------------------------------------------------------
// Insere símbolo no escopo do topo.  Como é inserção em lista, O(1).
// ----------------------------------------------------------------------------
void insertSymbol(SymbolTable *currentScope,
                  char *name, SymbolKind kind,
                  Type type, Location *loc)
{
    // Aloca novo nó de símbolo
    Symbol *sym  = malloc(sizeof(Symbol));
    if (!sym) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    sym->name       = strdup(name);
    sym->kind       = kind;
    sym->type       = type;

    // Cópia rasa da localização apenas para exemplo
    sym->location   = malloc(sizeof(Location));
    *sym->location  = *loc;

    // Insere no início da lista de símbolos deste escopo
    sym->next       = currentScope->symbols;
    currentScope->symbols = sym;
}

// ----------------------------------------------------------------------------
// Procura símbolo começando do escopo atual até o global.
// Retorna NULL se não encontrar.
// ----------------------------------------------------------------------------
Symbol *lookupSymbol(SymbolTable *stack, const char *name)
{
    for (SymbolTable *scope = stack; scope; scope = scope->next) {
        for (Symbol *sym = scope->symbols; sym; sym = sym->next) {
            if (strcmp(sym->name, name) == 0)
                return sym; // Achou
        }
    }
    return NULL; // Não encontrado em nenhum escopo
}
```

---

## Boas práticas

* **Empilhe antes de percorrer** a lista de declarações de um bloco e
  **desempilhe ao sair** – mesmo em caminhos de erro (use `goto cleanup` ou
  *RAII* em C++ se preferir).
* **Nunca reuse `Location`** de outra estrutura; sempre copie para evitar
  *use‑after‑free*.
* Mantenha a **destruição simétrica**: todo `pushScope` deve ter um
  `popScope`, e todo `createSymbolTable` deve ser pareado com
  `destroySymbolTable`.

---