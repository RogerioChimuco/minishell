# 02 — Tokenização e Análise Léxica (Lexer)

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que é um Lexer?](#21-o-que-é-um-lexer)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que o Lexer resolve](#24-problema-que-o-lexer-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 Teoria Profunda — O Processo de Tokenização](#31-teoria-profunda--o-processo-de-tokenização)
  - [3.2 Tipos de Tokens no Shell POSIX](#32-tipos-de-tokens-no-shell-posix)
  - [3.3 Regras de Quoteamento](#33-regras-de-quoteamento)
  - [3.4 Algoritmo de Reconhecimento de Tokens (POSIX §2.3)](#34-algoritmo-de-reconhecimento-de-tokens-posix-23)
  - [3.5 Limitações e Regras — Contexto 42 / Norminette](#35-limitações-e-regras--contexto-42--norminette)
  - [3.6 Funções Permitidas Relevantes](#36-funções-permitidas-relevantes)
  - [3.7 Metodologia de Aplicação (Pseudocódigo)](#37-metodologia-de-aplicação-pseudocódigo)
  - [3.8 Metodologia de Testes](#38-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que é um Lexer?

Um **Lexer** (Lexical Analyzer / Scanner) é o componente de um intérprete ou compilador responsável por converter uma sequência bruta de caracteres — a string de input do utilizador — numa sequência de **tokens**, que são unidades lógicas com tipo e valor bem definidos.

No contexto do shell, a string:
```
echo "hello world" | cat -n > output.txt
```
não é processada como um bloco opaco de texto. O Lexer transforma-a numa lista ordenada de tokens tipados:

```
[WORD:"echo"] [WORD:"hello world"] [PIPE:"|"] [WORD:"cat"]
[WORD:"-n"] [REDIR_OUT:">"] [WORD:"output.txt"]
```

Esta transformação é essencial para que as fases seguintes (Parser, Expansão, Execução) trabalhem com **unidades semânticas** em vez de caracteres individuais.

### 2.2 Contexto Histórico e Computacional

O conceito de análise léxica nasceu na teoria da computação formal dos anos 1950-60. Noam Chomsky formalizou as **gramáticas regulares** que descrevem a estrutura dos tokens; a **notação BNF** (Backus-Naur Form, 1960) permitiu descrever a gramática dos tokens de forma precisa.

A ferramenta `lex` (1975, AT&T Bell Labs) e o seu sucessor `flex` automatizaram a geração de lexers a partir de expressões regulares. No entanto, para o Minishell da 42, escreverás o teu lexer **manualmente em C**, o que é pedagogicamente mais valioso.

A gramática do shell Bash segue a especificação **POSIX.1-2017, Secção 2.3 (Token Recognition)**, que define as regras exatas de tokenização de forma determinista.

### 2.3 Relevância no Projeto Minishell da 42

O Lexer é **a primeira transformação real do input**. Os seus erros propagam-se para todas as fases seguintes: um token truncado ou mal tipado causará comportamentos incorretos no Parser e erros na execução. Além disso:

- As **regras de quoteamento** (`'`, `"`) são tratadas no Lexer — um dos pontos mais testados nas avaliações da 42
- A deteção de **tokens inválidos** (como `|` no início ou `||>`) deve produzir mensagens de erro corretas
- A gestão de memória de cada token (alocado no heap) deve ser perfeita para evitar leaks

### 2.4 Problema que o Lexer resolve

O Lexer resolve o problema da **ambiguidade semântica** do texto. Considere:

```
echo $HOME>/tmp/test
```

Sem tokenização, é impossível saber se `$HOME>/tmp/test` é uma única palavra ou uma expansão de variável seguida de redirecionamento. O Lexer, seguindo as regras POSIX, decompõe corretamente em:

```
[WORD:"echo"] [WORD:"$HOME"] [REDIR_OUT:">"] [WORD:"/tmp/test"]
```

---

## 3. Desenvolvimento

### 3.1 Teoria Profunda — O Processo de Tokenização

O processo de tokenização é uma **máquina de estados finita**: lê os caracteres um a um, mantém um estado interno, e emite um token quando encontra um delimitador.

Os **estados** da máquina do Lexer do shell são:

```
ESTADO_INICIAL
├── Espaço/Tab        → ESTADO_INICIAL (ignorar espaços entre tokens)
├── Comentário '#'    → ESTADO_COMENTÁRIO (ignorar até '\n')
├── Aspas simples '\''→ ESTADO_SINGLE_QUOTE
├── Aspas duplas '"'  → ESTADO_DOUBLE_QUOTE
├── '$'               → ESTADO_DOLLAR (início de variável ou subshell)
├── '<' ou '>'        → ESTADO_REDIRECTION
├── '|'               → ESTADO_PIPE
├── ';'               → ESTADO_SEMICOLON
├── '\n' / '\0'       → EMITIR TOKEN_EOF
└── Outro             → ESTADO_WORD
```

A saída do Lexer é tipicamente uma **lista ligada** de tokens. Cada nó contém:
- `type` : o tipo do token (enum)
- `value`: a string do token (alocada no heap)

### 3.2 Tipos de Tokens no Shell POSIX

A especificação POSIX (§2.3, §2.10) define os seguintes tipos de token relevantes para o Minishell:

```
TOKEN TYPE        EXEMPLOS               DESCRIÇÃO
─────────────────────────────────────────────────────────────────
WORD              echo, hello, -n, /bin  Palavra/argumento
PIPE              |                      Separador de comandos em pipeline
REDIR_IN          <                      Redirecionamento de input
REDIR_OUT         >                      Redirecionamento de output
REDIR_APPEND      >>                     Redirecionamento append
HEREDOC           <<                     Here-document
SEMICOLON         ;                      Separador de command tables
AND_IF            &&                     Execução condicional AND (bonus)
OR_IF             ||                     Execução condicional OR  (bonus)
NEWLINE           \n                     Fim de linha
IO_NUMBER         0, 1, 2                Número de file descriptor antes de redir
EOF               \0                     Fim de input
```

> **Nota para o Minishell básico**: O Subject da 42 não exige `&&` e `||` no modo obrigatório, mas requer `|`, `<`, `>`, `>>`, `<<`.

### 3.3 Regras de Quoteamento

O quoteamento é a funcionalidade mais complexa do Lexer. Segundo o POSIX (§2.2):

#### 3.3.1 Backslash (`\`)

O backslash **fora de aspas** preserva o valor literal do próximo caractere:
```bash
echo hel\lo      # output: hello (\ preserva o 'l' e desaparece)
echo hel\\lo     # output: hel\lo (\ preserva o segundo \)
echo \|          # output: | (o pipe perde o seu significado especial)
```

> **No Minishell**: o Subject da 42 **não requer** suporte a backslash fora de aspas duplas, exceto nos padrões de expansão. Verifica o teu Subject específico.

#### 3.3.2 Aspas Simples (`'...'`)

Preservam o **valor literal** de TODOS os caracteres entre elas. Nenhum caractere especial tem efeito:
```bash
echo '$HOME'     # output: $HOME (variável NÃO expandida)
echo '|'         # output: | (pipe NÃO é operador)
echo 'it'\''s'  # output: it's (única forma de ter ' dentro de '')
```

Regra POSIX: **não pode existir uma `'` dentro de aspas simples**. Para incluir `'` num string em single-quotes, deve-se sair das aspas, adicionar `\'`, e reentrar.

#### 3.3.3 Aspas Duplas (`"..."`)

Preservam o valor literal de todos os caracteres **exceto**:
- `$` — expansão de parâmetros/variáveis permanece ativa
- `` ` `` — substituição de comandos (backtick) permanece ativa
- `\` — backslash mantém significado especial quando precede `$`, `` ` ``, `"`, `\`, ou newline

```bash
echo "Hello $USER"    # output: Hello john (variável expandida)
echo "pipe: |"        # output: pipe: | (| é literal)
echo "quote: \""       # output: quote: " (backslash escapa a aspas)
```

#### 3.3.4 Casos de Quoteamento Complexo

O Lexer deve suportar a **concatenação de segmentos quoted/unquoted**:

```bash
echo hello"wor'ld"'te"st'
# Tokens:
#   "hello" (unquoted)
#   "wor'ld" (double-quoted — ' é literal)
#   'te"st' (single-quoted — " é literal)
# Resultado: um único WORD com valor: hellowor'ldte"st
```

Este comportamento é obrigatório no Minishell da 42. O token resultante é a **concatenação** dos segmentos, com as aspas removidas (**quote removal** — POSIX §2.6.7).

### 3.4 Algoritmo de Reconhecimento de Tokens (POSIX §2.3)

O POSIX define 10 regras ordenadas para o reconhecimento de tokens. As mais relevantes para o Minishell:

```
ALGORITMO:
  Para cada caractere 'c' no input:

  REGRA 1: Se 'c' é EOF → delimitar token atual e emitir EOF
  REGRA 2: Se char anterior era parte de operador E 'c' pode estender esse operador
             → 'c' faz parte do operador (ex: '>' + '>' → '>>')
  REGRA 3: Se char anterior era parte de operador E 'c' NÃO pode estender
             → delimitar o operador
  REGRA 4: Se 'c' é '\', '\'' ou '"' (não quoted) 
             → afeta quoteamento do que se segue
  REGRA 5: Se 'c' é '$' ou '`' (não quoted)
             → início de expansão (parâmetro, comando, aritmética)
  REGRA 6: Se 'c' é operador não quoted (|, <, >, ;)
             → delimitar token atual, iniciar token operador
  REGRA 7: Se 'c' é espaço/tab não quoted
             → delimitar token atual (descartar espaço)
  REGRA 8: Se token já em construção
             → adicionar 'c' ao token atual
  REGRA 9: Se 'c' é '#' não quoted
             → tudo até '\n' é comentário, descartar
  REGRA 10: 'c' inicia novo WORD
```

### 3.5 Limitações e Regras — Contexto 42 / Norminette

**Edge cases que os avaliadores da 42 testam:**

| Caso | Input | Comportamento Esperado |
|------|-------|------------------------|
| Aspas não fechadas | `echo "hello` | Erro de sintaxe: `unexpected EOF` |
| Aspas simples dentro de duplas | `echo "it's"` | Output: `it's` (o `'` é literal) |
| Aspas duplas dentro de simples | `echo 'say "hi"'` | Output: `say "hi"` |
| `$` no final | `echo test$` | Output: `test` (`$` sem nome seguinte é literalmente `$` ou ignorado — testa com bash) |
| Token vazio com aspas | `echo ""` | Output: ` ` (linha vazia — argumento vazio é válido) |
| Pipe sem comando | `\| echo test` | Erro de sintaxe |
| Redirecionamento sem ficheiro | `echo test >` | Erro: `syntax error near unexpected token 'newline'` |
| Múltiplas aspas | `echo "a""b"'c'` | Output: `abc` (concatenação) |

**Norminette:**
- A função de tokenização deve ser dividida em funções auxiliares ≤ 25 linhas
- Proibido usar `strtok()` (modifica a string original — pode causar problemas com o buffer do readline)
- Preferir iterar com índice (`int i`) ou ponteiros, voltando à forma Norminette-compatible

### 3.6 Funções Permitidas Relevantes

#### `malloc` / `free`
```c
void *malloc(size_t size);
void  free(void *ptr);
```
- Cada `value` de um token é uma string alocada no heap com `malloc`/`ft_strdup`
- Deve ser libertada quando o token é destruído

#### `ft_strdup` / `ft_substr` (da tua libft)
```c
char *ft_strdup(const char *s);
char *ft_substr(char const *s, unsigned int start, size_t len);
```
- Essenciais para copiar segmentos do input para o `value` do token sem modificar o buffer original

#### `isspace` (atenção: não permitida — usar lógica própria)
```c
// Implementar manualmente:
int ft_isspace(char c) { return (c == ' ' || c == '\t' || c == '\n'); }
```
> O Subject da 42 tipicamente não inclui `isspace`. Usa a tua libft ou implementa inline.

#### `write` (para mensagens de erro)
```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```
- Mensagens de erro de sintaxe devem ser escritas em `STDERR` (fd=2)

### 3.7 Metodologia de Aplicação (Pseudocódigo)

```
ESTRUTURA:
  typedef enum token_type:
    TOKEN_WORD, TOKEN_PIPE, TOKEN_REDIR_IN, TOKEN_REDIR_OUT,
    TOKEN_REDIR_APPEND, TOKEN_HEREDOC, TOKEN_SEMICOLON,
    TOKEN_AND_IF, TOKEN_OR_IF, TOKEN_EOF

  typedef struct token:
    token_type  type
    char       *value   // string alocada no heap
    struct token *next  // lista ligada


FUNÇÃO tokenizar(input: string) → lista_de_tokens:
    i = 0
    lista = NULL
    
    ENQUANTO input[i] != '\0':
        IGNORAR_ESPAÇOS(input, &i)
        SE input[i] == '\0': BREAK
        
        SE input[i] == '#':                    // comentário
            ENQUANTO input[i] != '\n' E input[i] != '\0': i++
            CONTINUAR
        
        SE input[i] == '|':
            SE input[i+1] == '|':
                adicionar_token(&lista, TOKEN_OR_IF, "||")
                i += 2
            SENÃO:
                adicionar_token(&lista, TOKEN_PIPE, "|")
                i++
        
        SENÃO SE input[i] == '>':
            SE input[i+1] == '>':
                adicionar_token(&lista, TOKEN_REDIR_APPEND, ">>")
                i += 2
            SENÃO:
                adicionar_token(&lista, TOKEN_REDIR_OUT, ">")
                i++
        
        SENÃO SE input[i] == '<':
            SE input[i+1] == '<':
                adicionar_token(&lista, TOKEN_HEREDOC, "<<")
                i += 2
            SENÃO:
                adicionar_token(&lista, TOKEN_REDIR_IN, "<")
                i++
        
        SENÃO SE input[i] == ';':
            adicionar_token(&lista, TOKEN_SEMICOLON, ";")
            i++
        
        SENÃO:  // início de WORD
            início = i
            ENQUANTO input[i] não é delimitador E não é '\0':
                SE input[i] == '\'':
                    AVANÇAR ATÉ ENCONTRAR '\'' (single quote mode)
                SENÃO SE input[i] == '"':
                    AVANÇAR ATÉ ENCONTRAR '"' (double quote mode)
                SENÃO:
                    i++
            word = extrair_e_processar_aspas(input, início, i)
            adicionar_token(&lista, TOKEN_WORD, word)
    
    adicionar_token(&lista, TOKEN_EOF, NULL)
    RETORNAR lista


FUNÇÃO verificar_aspas_fechadas(input: string) → booleano:
    state = NORMAL
    PARA CADA c EM input:
        SE state == NORMAL:
            SE c == '\'': state = SINGLE_QUOTE
            SE c == '"':  state = DOUBLE_QUOTE
        SENÃO SE state == SINGLE_QUOTE:
            SE c == '\'': state = NORMAL
        SENÃO SE state == DOUBLE_QUOTE:
            SE c == '"':  state = NORMAL
    RETORNAR state == NORMAL  // false = aspas não fechadas
```

**Ponto crítico — Quote Removal:**

Após extrair o valor de um token WORD, as aspas devem ser removidas do valor final:
```
Input:  echo "hello world" 'test'
Token WORD value ANTES de quote removal: "hello world"'test'  (com aspas)
Token WORD value DEPOIS de quote removal: hello world test    (sem aspas, concatenado? Não!)

Na realidade, são 2 tokens WORD separados pelo espaço entre eles:
Token[0]: value = "hello world" → "hello world" (com espaço interno preservado, aspas removidas)
Token[1]: value = 'test' → "test"
```

### 3.8 Metodologia de Testes

**Teste 1 — Operadores simples:**
```bash
# Input: "echo hello | cat"
# Esperado: [WORD:echo] [WORD:hello] [PIPE:|] [WORD:cat] [EOF]
```

**Teste 2 — Operadores compostos:**
```bash
# Input: "echo >> file"
# Esperado: [WORD:echo] [REDIR_APPEND:>>] [WORD:file] [EOF]
# ERRADO seria: [WORD:echo] [REDIR_OUT:>] [REDIR_OUT:>] [WORD:file]
```

**Teste 3 — Aspas simples:**
```bash
echo 'hello | world'
# Output esperado: hello | world (pipe NÃO separou)
# Token único: WORD:"hello | world"
```

**Teste 4 — Aspas duplas com variável:**
```bash
echo "Hello $USER"
# Lexer produz: WORD com value="Hello $USER" (com $ preservado)
# A expansão de $USER ocorre MAIS TARDE (fase de expansão)
```

**Teste 5 — Aspas não fechadas:**
```bash
echo "hello
# Esperado: minishell: syntax error: unclosed quotes
```

**Teste unitário (Valgrind):**
```bash
# Após cada tokenização, destruir toda a lista de tokens e verificar 0 leaks
valgrind --leak-check=full ./minishell
```

---

## 4. Conclusão

### 4.1 Síntese

O Lexer é a primeira barreira de segurança e transformação do Minishell. Ele converte texto bruto em tokens semânticos, aplicando as regras de quoteamento POSIX. A sua implementação correta é fundamental para a robustez de todas as fases seguintes.

### 4.2 Armadilhas Comuns (Pitfalls)

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Modificar o buffer do readline** | `strtok()` insere `\0` no input original | Nunca modificar o `input` — extrair cópias com `ft_substr` |
| **Não tratar aspas aninhadas** | `echo "a'b"c` falha | Seguir as regras de estado POSIX rigorosamente |
| **Ignorar o estado de quoteamento nos operadores** | `echo "a|b"` divide em 2 tokens | Verificar se está em modo quote antes de tratar `|` como operador |
| **Memory leak nos tokens** | Tokens criados mas nunca libertados | Função `free_token_list()` deve ser chamada em todos os caminhos de código |
| **Tratar `>>` como dois `>`** | Iterar por caractere sem lookahead | Verificar sempre `input[i+1]` antes de classificar operadores de 1 caractere |
| **Aspas não fechadas não detetadas** | O lexer processa indefinidamente | Verificar estado de quoteamento antes de aceitar o input |

### 4.3 Integração no Projeto Final

O Lexer produz **a lista de tokens** que é o input direto do **Parser (Módulo 03)**. A separação clara entre Lexer e Parser (não misturar as duas fases) é um princípio de design que facilita enormemente o debugging. O Lexer não sabe nada sobre a estrutura gramatical — ele apenas classifica os símbolos.

---

## 5. Bibliografia

### Documentação Oficial

- [POSIX.1-2017 — §2.2 Quoting](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_02)
- [POSIX.1-2017 — §2.3 Token Recognition](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_03)
- [POSIX.1-2017 — §2.6.7 Quote Removal](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_06_07)
- `man 3 isspace`, `man 3 strchr`, `man 3 strdup`

### Livros Clássicos

- **Alfred V. Aho, Monica S. Lam, Ravi Sethi, Jeffrey D. Ullman** — *Compilers: Principles, Techniques, and Tools* ("Dragon Book"), 2nd Ed. (2006)  
  Capítulo 3: Lexical Analysis — teoria formal de autômatos para análise léxica

- **W. Richard Stevens** — *Advanced Programming in the UNIX Environment*  
  Capítulo 2: UNIX Standardization and Implementations

### Artigos e Recursos Web

- **Stephen Brennan** — *Write a Shell in C* — Seção "Parsing the line"  
  https://brennan.io/2015/01/16/write-a-shell-in-c/

- **GNU Bash Manual** — §3.1.2 Quoting  
  https://www.gnu.org/software/bash/manual/bash.html#Quoting

- **Wikipedia** — LL parser, LR parser, Lexical analysis  
  https://en.wikipedia.org/wiki/Lexical_analysis

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
