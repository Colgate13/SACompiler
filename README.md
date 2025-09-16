<h1 align="center">SACompiler</h1>

<p align="center">
   <img src="https://raw.githubusercontent.com/Colgate13/SACompiler/main/utils/parsing-overview.png" alt="SACompiler" width="800"/>
</p>

<p align="center">	
  <img alt="Repository size" src="https://img.shields.io/github/repo-size/Colgate13/SACompiler?color=774DD6">

  <a href="https://github.com/Colgate13/SACompiler/commits">
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/Colgate13/SACompiler?color=774DD6">
  </a> 
  <img alt="License" src="https://img.shields.io/badge/license-MIT-8257E5">
  <a href="https://github.com/Colgate13/SACompiler/stargazers">
    <img alt="Stargazers" src="https://img.shields.io/github/stars/Colgate13/SACompiler?color=8257E5&logo=github">
  </a>
</p>

<p align="center">
    <a href="README.md">English</a>
 </p>

  <p align="center">
  <a href="#about">About</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
  <a href="#technologies">Technologies</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
  <a href="#features">Features</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
  <a href="#docs">Docs</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
  <a href="#todo">TODO</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
  <a href="#license">License</a>
</p>

## :notebook: About

<div id="about"></div>

This project is an my implementation of a basic a Syntactic and lexical SACompiler developed in C using the Clang compiler, designed to construct Abstract Syntax Trees (AST). This project is ideal for students, educators, and developers interested in understanding and applying fundamental concepts of syntactic analysis in compilers.

Implements this automata/state machine for lexical
<p align="center">
   <img src="https://raw.githubusercontent.com/Colgate13/SACompiler/main/utils/Lexical/LexicalAnalizerMachine.png" alt="lexial" width="400"/>
</p>

For Parser using this [grammar](https://github.com/Colgate13/SACompiler/blob/main/utils/Parser/gram.txt):

```
<program>            --> "program" <statement_tail> "end;"

<statement_tail>    --> <statement> <statement_tail>
                     | ε

<statement>          --> <print_statement>
                    | <variable_declaration>
                    | <assignment>
                    | <if_statement>

<block>             --> "{" <statement_tail> "}"

// Statements Grammar
<variable_declaration> --> "var" <type> <identifier> ";"
<assignment>         --> <identifier> "=" <expression> ";"
<print_statement>    --> "print(" <expression> ");"
<if_statement>      --> "if" "(" <expression> ")" <block>

// Expression Grammar (Contais the arithmetic operations and the relational operators)
<expression>         --> <arithmetic_expression> <operator_relational>

// Relational Operator Grammar
<operator_relational> --> <relational_operator> <arithmetic_expression>
                    | ε

// Relational Operator Tokens
<relational_operator> --> "==" | "!=" | "<" | "<=" | ">" | ">="

// Arithmetic Expression Grammar
<arithmetic_expression> --> <term> <arithmetic_expression_tail>
<arithmetic_expression_tail> --> <add_operator> <term> <arithmetic_expression_tail>
                    | ε

// Arithmetic Operator Tokens
<add_operator>       --> "+" | "-" // Addition and Subtraction
<mult_operator>      --> "*" | "/" | "%" // Multiplication, Division and Modulus

<term>               --> <factor> <term_tail>
<term_tail>          --> <mult_operator> <factor> <term_tail>
                    | ε

<factor>             --> <number>
                    | <identifier>
                    | <string>
                    | "(" <expression> ")"

// Basic Tokens
<number>             --> [0-9]+
<string>             --> '"' [a-zA-Z0-9_]* '"'
<identifier>         --> [a-zA-Z_][a-zA-Z0-9_]*
<type>               --> "int"
                    | "double"
                    | "string"


// Exemples of code that can be parsed by this grammar
if (identifier1 == identifier2) {
  print("They are equal");
}

// if (<expression>) { <block> }
  // -> if ({<arithmetic_expression>} {<operator_relational>}) { <block> }
    // -> if ({<term> <arithmetic_expression_tail>} {<relational_operator> <arithmetic_expression>}) { <block> }
      // -> if ({<factor> <term_tail>} {"=" <arithmetic_expression>}) { <block> }
        // -> if ({<identifier>} {"=" {<term> <arithmetic_expression_tail>}}) { <block> }
          // -> if ({<identifier>} {"=" {<factor> <term_tail>}}) { <block> }
            // -> if ({<identifier>} {"=" {<identifier>}}) { <block> }
              // -> if (<identifier> "=" <identifier>) { <block> } // This is a valid statement
```

## Technologies 🐱‍🏍🎂

<div id="technologies"></div>

- [C](https://www.gnu.org/software/gnu-c-manual/) - C
- [Clang](https://clang.llvm.org/) - C/C++ Compiler
- [Make | Makefile](https://www.gnu.org/software/make/) - Make and Makefile

### Features

<div id="features"></div>

- [x] Lexical Analyzer
- [x] Construct Abstract Syntax Trees (AST)
- [x] Error handling
- [x] Output AST in JSON format
- [x] Output AST in file

## Docs

<div id="docs"></div>

### Required

```sh
clang
```

### Build and Run

```sh
git clone https://github.com/Colgate13/SACompiler.git
cd SACompiler
make

./bin/SACompiler ./utils/Parser/exemples/general/exemple.code ./ast-output.json
```

## 🚧 TODO - Funcionalidades e Bugs Encontrados

<div id="todo"></div>

Durante o desenvolvimento e testes do SACompiler, foram identificadas várias limitações e bugs que precisam ser corrigidos ou implementados. Esta seção documenta todos os problemas encontrados e funcionalidades que precisam ser desenvolvidas.

### 🔥 Bugs Críticos

#### 1. **Comentários Não Suportados**
- **Problema**: O lexer reconhece `//` como operador em vez de comentário
- **Erro**: `Expected print_statement, variable_declaration or assignment`
- **Solução**: Implementar suporte a comentários de linha (`//`) e bloco (`/* */`) no lexer
- **Prioridade**: Alta

#### 3. **Números Negativos Não Suportados**
- **Problema**: Lexer não reconhece números negativos como `-5`
- **Erro**: Trata `-` como operador separado em vez de prefixo numérico
- **Solução**: Modificar lexer para reconhecer números negativos ou implementar operador unário
- **Prioridade**: Média

### 🛠️ Funcionalidades em Falta

#### 5. **Suporte a Else em Condicionais**
- **Funcionalidade**: Implementar `if-else` statements
- **Gramática Necessária**: `<if_statement> --> "if" "(" <expression> ")" <block> ["else" <block>]`
- **Prioridade**: Média

#### 6. **Loops (for, while)**
- **Funcionalidade**: Implementar estruturas de repetição
- **Gramática Necessária**: 
  - `<while_statement> --> "while" "(" <expression> ")" <block>`
  - `<for_statement> --> "for" "(" <assignment> ";" <expression> ";" <assignment> ")" <block>`
- **Prioridade**: Baixa

#### 7. **Funções Definidas pelo Usuário**
- **Funcionalidade**: Permitir definição e chamada de funções
- **Gramática Necessária**: `<function> --> "function" <identifier> "(" [<parameters>] ")" <block>`
- **Prioridade**: Baixa

#### 8. **Arrays/Vetores**
- **Funcionalidade**: Suporte a estruturas de dados indexadas
- **Gramática Necessária**: `<array_declaration> --> "var" <type> <identifier> "[" <number> "]"`
- **Prioridade**: Baixa

### 🐛 Bugs de Parser/Semântico

#### 9. **Operadores Unários Não Suportados**
- **Problema**: Não há suporte para `!`, `-`, `+` como operadores unários
- **Exemplo**: `!condition`, `-variable`
- **Prioridade**: Média

#### 10. **Verificação de Tipos Incompleta**
- **Problema**: Mistura de tipos em operações não é adequadamente verificada
- **Exemplo**: `int + string` deveria gerar erro
- **Prioridade**: Média

#### 11. **Escopo de Variáveis Não Implementado**
- **Problema**: Todas as variáveis são globais
- **Impacto**: Variáveis declaradas em blocos não têm escopo local
- **Prioridade**: Baixa

### 🔧 Melhorias de Código

#### 12. **Mensagens de Erro Mais Detalhadas**
- **Problema**: Mensagens de erro são genéricas
- **Melhoria**: Fornecer contexto mais específico sobre o erro
- **Prioridade**: Baixa

#### 13. **Otimização do AST**
- **Problema**: AST pode ser otimizado para melhor performance
- **Melhoria**: Implementar folding de constantes e outras otimizações
- **Prioridade**: Baixa

#### 14. **Suporte a Strings Multilinha**
- **Funcionalidade**: Permitir strings que quebram linha
- **Prioridade**: Baixa

### 📊 Status dos Testes

| Funcionalidade | Status | Observações | Arquivo de Teste |
|---------------|--------|-------------|------------------|
| Declaração de variáveis | ✅ | Funciona corretamente | N/A |
| Operações aritméticas básicas | ✅ | `+`, `-`, `*`, `/`, `%` funcionam | N/A |
| Operadores relacionais | ✅ | Todos os 6 operadores funcionam | N/A |
| Condicionais if | ✅ | Funcionam sem parênteses complexos | N/A |
| Print statements | ✅ | Funciona com strings e variáveis | N/A |
| Precedência de operadores | ✅ | Implementada corretamente | N/A |
| **Comentários** | ❌ | **FALHA**: `//` tratado como operador, `/* */` causa erro de parsing | `test_comments.code` |
| **Parênteses em expressões** | ✅ | **SUCESSO**: Parênteses funcionam corretamente! | `test_parentheses.code` |
| **Números negativos** | ⚠️ | **PARCIAL**: Unário funciona mas decimais negativos têm erro de tipo | `test_negative_numbers.code` |
| **Literais double** | ✅ | **SUCESSO**: Decimais são reconhecidos como double! | `test_decimal_double.code` |
| Condicionais aninhados | ✅ | Funcionam com parênteses agora | N/A |

### 📋 Resultados Detalhados dos Testes

#### 1. Teste de Comentários (`test_comments.code`)
```
❌ FALHA: Error (1): Expected print_statement, variable_declaration or assignment
```
- **Problema Confirmado**: O lexer trata `//` como TOKEN_TYPE_OPERATOR
- **Problema Adicional**: Comentários de bloco `/* */` causam erro de parsing
- **Status**: Bug crítico confirmado

#### 2. Teste de Parênteses (`test_parentheses.code`)
```
✅ SUCESSO: Compilação bem-sucedida com AST gerado
```
- **Descoberta**: Parênteses funcionam corretamente!
- **Status**: Bug reportado incorretamente - funcionalidade está implementada

#### 3. Teste de Números Negativos (`test_negative_numbers.code`)
```
⚠️ PARCIAL: Error: Type mismatch in assignment to 'negDouble'. Expected 'double', got 'int'
```
- **Problema Confirmado**: `-3.14` é tratado como `int` em vez de `double`
- **Descoberta**: Operador unário `-` funciona para inteiros
- **Status**: Bug parcialmente confirmado - problema específico com decimais negativos

#### 4. Teste de Literais Double (`test_decimal_double.code`)
```
✅ SUCESSO: Compilação bem-sucedida, decimais reconhecidos como double
```
- **Descoberta**: Literais double funcionam corretamente!
- **Tokens**: `3.14` é corretamente reconhecido como `TOKEN_TYPE_NUMBER` e inferido como `double`
- **Status**: Bug reportado incorretamente - funcionalidade está implementada

### 🎯 Roadmap de Desenvolvimento

## License

<div id="license"></div>

MIT [LICENSE](LICENSE.md)
