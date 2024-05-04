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
<program>            --> "program" <statement> "end;"

<statement>          --> <print_statement>
                    | <variable_declaration>
                    | <assignment>

<variable_declaration> --> "var" <type> <identifier> ";"

<assignment>         --> <identifier> "=" <expression> ";"

<print_statement>    --> "print(" <expression> ");"

<expression>         --> <term> <expression_tail>
                    | <term>

<expression_tail>    --> "+" <term> <expression_tail>
                    | "-" <term> <expression_tail>
                    | "*" <term> <expression_tail>
                    | "/" <term> <expression_tail>
                    | "%" <term> <expression_tail>
                    | ε

<term>               --> <number>
                    | <identifier>
                    | <string>

<number>             --> [0-9]+

<string>             --> '"' [a-zA-Z0-9_]* '"'

<identifier>         --> [a-zA-Z_][a-zA-Z0-9_]*

<type>               --> "int"
                    | "float"
                    | "string"

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
cJSON
```

### Build and Run

```sh
git clone https://github.com/Colgate13/SACompiler.git
cd SACompiler
make

./bin/SACompiler ./utils/Parser/exemples/general/exemple.code ./output.json
```

## License

<div id="license"></div>

MIT [LICENSE](LICENSE.md)
