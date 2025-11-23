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

### 🛠️ Funcionalidades em Falta

#### **Mensagens de Erro Mais Detalhadas**
- **Problema**: Mensagens de erro são genéricas
- **Melhoria**: Fornecer contexto mais específico sobre o erro. Principalmente no semantico esta bem ruim
- **Prioridade**: Baixa

#### **Otimização do AST**
- **Problema**: AST pode ser otimizado para melhor performance
- **Melhoria**: Implementar folding de constantes e outras otimizações
- **Prioridade**: Baixa

#### **Loops (for, while)**
- **Funcionalidade**: Implementar estruturas de repetição
- **Gramática Necessária**: 
  - `<while_statement> --> "while" "(" <expression> ")" <block>`
  - `<for_statement> --> "for" "(" <assignment> ";" <expression> ";" <assignment> ")" <block>`
- **Prioridade**: Baixa

#### **Funções Definidas pelo Usuário**
- **Funcionalidade**: Permitir definição e chamada de funções.
- **Gramática Necessária**: `<function> --> "function" <identifier> "(" [<parameters>] ")" <block>`
- **Prioridade**: Baixa

#### **Transformar printf em uma funcao invez de statement**
- **Funcionalidade**: Printf como funcao
- **Prioridade**: Baixa

#### **Arrays/Vetores**
- **Funcionalidade**: Suporte a estruturas de dados indexadas
- **Gramática Necessária**: `<array_declaration> --> "var" <type> <identifier> "[" <number> "]"`
- **Prioridade**: Baixa

#### **Variable declaration with assignment**
- **Funcionalidade**: Suporte a declaração de variáveis com atribuição inicial
- **Gramática Necessária**: `<variable_declaration> --> "var" <type> <identifier> ";" | "var" <type> <identifier> "=" <expression> ";"`
- **Prioridade**: Baixa

## License

<div id="license"></div>

MIT [LICENSE](LICENSE.md)
