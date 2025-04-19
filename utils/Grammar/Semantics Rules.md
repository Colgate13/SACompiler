# 📘 Regras Semânticas - SA Language

Este arquivo documenta as **regras semânticas** da linguagem SA, separadas da gramática. Cada regra é numerada e associada a um trecho da gramática.

## 🗂 Notação Utilizada
- ✅ **AÇÃO**: comportamento que o analisador deve aplicar
- ❌ **ERRO**: caso que deve lançar erro semântico
- ⚠️ **AVISO**: comportamento que pode gerar warning (opcional)
- 🔍 **BUSCA**: uso de símbolo existente na tabela
- ➕ **INSERE**: adiciona novo símbolo na tabela
- 📥 **ESCOPO**: entrada e saída de escopos (push/pop)

---

## 🔢 [SEM#001] - Declaração de Variável
**Gramática**: `<variable_declaration> ::= "var" <type> <identifier> ";"`

- 🔍 Verifica se o identificador já foi declarado no escopo atual
  - ❌ Se sim: erro de variável duplicada
- ➕ Insere símbolo com nome, tipo, escopo, localização, estado de uso = não usado

---

## 🔢 [SEM#002] - Atribuição
**Gramática**: `<assignment> ::= <identifier> "=" <expression> ";"`

- 🔍 Verifica se o identificador foi declarado
  - ❌ Se não declarado: erro
- ✅ Infere tipo da expressão
- ❌ Verifica compatibilidade entre tipo do identificador e da expressão
- ✅ Marca variável como usada

---

## 🔢 [SEM#003] - Expressão
**Gramática**: `<expression> ::= <arithmetic_expression> <operator_relational>`

- ✅ Avalia tipo da `arithmetic_expression`
- ✅ Se houver `operator_relational`:
  - ✅ Avalia tipo do lado direito
  - ❌ Se tipos incompatíveis: erro
  - ✅ Resultado final: tipo `int`
- ✅ Senão: herda tipo da expressão aritmética

---

## 🔢 [SEM#004] - Operadores Relacionais
**Gramática**: `<operator_relational> ::= <relational_operator> <arithmetic_expression>`

- ✅ Verifica compatibilidade de tipos nos dois lados
  - ❌ `string == int`, `float != string`, etc. → erro
  - ✅ `int == float` → permitido
- ✅ Resultado da operação: `int`

---

## 🔢 [SEM#005] - Operadores Aritméticos
**Gramática**: `<arithmetic_expression>`, `<term>`, `<factor>`

- ✅ Valida combinações de tipo:
  - `int + int → int`
  - `int + float → float`
  - ⚠️ `string + string → string` (caso suportado)
  - ❌ `%` com `float` → erro
- ✅ Tipo resultante: mais abrangente

---

## 🔢 [SEM#006] - Uso de Identificadores
**Gramática**: `<identifier>` (em qualquer lugar)

- 🔍 Verifica se o identificador está na tabela de símbolos
  - ❌ Se não existir: erro
- ✅ Retorna tipo
- ✅ Marca como usado

---

## 🔢 [SEM#007] - Print
**Gramática**: `<print_statement> ::= print(<expression>);`

- 🔍 Verifica se todas as variáveis da expressão estão declaradas
  - ❌ Se não estiverem: erro
- ✅ Marca cada uma como usada

---

## 🔢 [SEM#008] - If Statement
**Gramática**: `<if_statement> ::= if (<expression>) <block>`

- ✅ Avalia tipo da expressão
  - ❌ Se não for `int`: erro (só permite booleano como `int`)
- 📥 Cria novo escopo (pushScope)
- ✅ Analisa statements internos
- 📥 Sai do escopo (popScope)

---

## 🔢 [SEM#009] - Escopos
**Gramática**: `<block>` e estruturas aninhadas

- 📥 Ao entrar num bloco: `pushScope()`
- 📥 Ao sair: `popScope()`
- ✅ Permite shadowing (redeclaração em escopo interno)

---

## 🔢 [SEM#010] - Declarações Redundantes
**Regra transversal**

- ❌ Se mesma variável for declarada mais de uma vez no mesmo escopo → erro
- ✅ Permite que uma variável de escopo interno tenha o mesmo nome (shadowing)

---
