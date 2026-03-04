# 03 — Análise Sintática e AST (Parser)

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que é um Parser?](#21-o-que-é-um-parser)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que o Parser resolve](#24-problema-que-o-parser-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 Gramática Formal do Shell POSIX](#31-gramática-formal-do-shell-posix)
  - [3.2 Tipos de Estruturas de Dados](#32-tipos-de-estruturas-de-dados)
  - [3.3 A Abstract Syntax Tree (AST)](#33-a-abstract-syntax-tree-ast)
  - [3.4 Algoritmo de Parsing — Descida Recursiva](#34-algoritmo-de-parsing--descida-recursiva)
  - [3.5 Limitações e Regras — Contexto 42 / Norminette](#35-limitações-e-regras--contexto-42--norminette)
  - [3.6 Funções Permitidas Relevantes](#36-funções-permitidas-relevantes)
  - [3.7 Metodologia de Aplicação (Pseudocódigo)](#37-metodologia-de-aplicação-pseudocódigo)
  - [3.8 Metodologia de Testes](#38-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que é um Parser?

Um **Parser** (Analisador Sintático) recebe a lista de tokens produzida pelo Lexer e constrói uma **estrutura de dados hierárquica** que representa a relação gramatical entre esses tokens. Esta estrutura — a **Abstract Syntax Tree (AST)** ou uma representação equivalente — captura a **intenção do utilizador** de uma forma que o executor pode processar.

Onde o Lexer responde à pergunta *"quais são as unidades?"*, o Parser responde à pergunta *"como se relacionam essas unidades?"*.

Exemplo: a linha
```
ls -la | grep test > output.txt
```
gera a estrutura:
```
PIPELINE
├── COMMAND: ls [-la]
└── COMMAND: grep [test]
       └── REDIR_OUT: output.txt
```

### 2.2 Contexto Histórico e Computacional

A teoria da análise sintática alicerça-se nas **gramáticas livres de contexto** de Chomsky (1956) e no formalismo BNF introduzido por John Backus para descrever ALGOL 60 (1960). Os parsers **LL** (top-down, Left-to-right, Leftmost derivation) e **LR** (bottom-up) são os dois paradigmas clássicos.

Para o shell POSIX, a gramática é suficientemente simples para ser implementada através de um **parser de descida recursiva** (recursive descent parser), um método LL(1) escrito manualmente, sem geração automática de código, o que é perfeito para o contexto da 42.

### 2.3 Relevância no Projeto Minishell da 42

O Parser determina **como os comandos se encadeiam**. Sem um parser correto:
- `ls | grep | wc` não seria interpretado como uma pipeline de 3 estágios
- `cmd1 > out < in` teria o input/output invertido
- `echo "a" | cat` trataria a aspa como parte do pipe

A arquitetura do Parser define diretamente a arquitetura do executor na fase 05 (Processos).

### 2.4 Problema que o Parser resolve

O Parser resolve o problema da **precedência e associatividade dos operadores do shell**:

- `|` liga comandos em pipeline (associativo à esquerda)
- `>` e `<` são atributos do comando que os precede
- `;` separa pipelines independentes
- `&&` e `||` (bonus) têm precedência inferior à `|`

---

## 3. Desenvolvimento

### 3.1 Gramática Formal do Shell POSIX

A gramática completa do shell POSIX (§2.10) em forma simplificada para o Minishell:

```
program          : list
                 | ;
                 ;

list             : compound_list
                 ;

compound_list    : pipeline
                 | compound_list NEWLINE pipeline
                 | compound_list AND_IF  pipeline   /* bonus: && */
                 | compound_list OR_IF   pipeline   /* bonus: || */
                 ;

pipeline         : command
                 | pipeline PIPE command
                 ;

command          : simple_command
                 | compound_command
                 ;

simple_command   : cmd_prefix cmd_word cmd_suffix
                 | cmd_prefix cmd_word
                 | cmd_prefix
                 | cmd_word cmd_suffix
                 | cmd_word
                 ;

cmd_prefix       : io_redirect
                 | cmd_prefix io_redirect
                 ;

cmd_suffix       : io_redirect
                 | WORD
                 | cmd_suffix io_redirect
                 | cmd_suffix WORD
                 ;

cmd_word         : WORD
                 ;

io_redirect      : io_file
                 | io_here
                 ;

io_file          : REDIR_IN    filename
                 | REDIR_OUT   filename
                 | REDIR_APPEND filename
                 ;

io_here          : HEREDOC WORD
                 ;

filename         : WORD
                 ;
```

> Esta gramática é **context-free** e pode ser implementada com descida recursiva. A versão completa POSIX inclui subshells `(...)`, grouping `{...}`, `case`, `for`, `while`, `if`, etc., que **não são necessários** no Minishell básico da 42.

### 3.2 Tipos de Estruturas de Dados

A arquitetura de dados do Parser/AST para o Minishell segue o padrão do repositório de referência `DimitriDaSilva/42_minishell`:

```c
/* ─── Redirecionamento ─── */
typedef struct s_redir
{
    int     type;   /* TOKEN_REDIR_IN, TOKEN_REDIR_OUT, TOKEN_REDIR_APPEND, TOKEN_HEREDOC */
    char   *file;   /* nome do ficheiro ou delimitador do heredoc */
}   t_redir;

/* ─── Comando simples ─── */
typedef struct s_cmd
{
    char   **argv;  /* NULL-terminated array: argv[0] = nome, argv[1..n] = args */
    t_list *redirs; /* lista de t_redir */
}   t_cmd;

/* ─── Tabela de comandos (uma pipeline) ─── */
typedef struct s_cmd_table
{
    t_list *cmds;       /* lista de t_cmd (ligados por |) */
    char    delimiter;  /* ';', '&' (background), 0 (normal) */
    int    *pipes;      /* array de file descriptors para os pipes */
}   t_cmd_table;

/* ─── AST (raiz) ─── */
typedef struct s_ast
{
    t_list *cmd_tables; /* lista de t_cmd_table (ligadas por ; ou && ou ||) */
}   t_ast;
```

**Relação hierárquica:**
```
t_ast
└── t_list → t_cmd_table [A ; B ; C]
              └── t_list → t_cmd [cmd1 | cmd2 | cmd3]
                            ├── argv: ["ls", "-la", NULL]
                            └── t_list → t_redir [> output.txt]
```

### 3.3 A Abstract Syntax Tree (AST)

Uma **AST** é uma árvore em que:
- Os **nós internos** representam operadores (`|`, `;`, `&&`, `||`)
- As **folhas** representam comandos simples com os seus argumentos e redirecionamentos

Para o Minishell, pode-se usar uma **estrutura de listas aninhadas** em vez de uma árvore explícita. A hierarquia `t_ast → t_cmd_table → t_cmd` captura a estrutura de forma eficiente:

```
Input:  ls -la | grep foo > out.txt ; echo done

AST (representação textual):
  cmd_table[0]:
    cmd[0]: argv=["ls", "-la"],  redirs=[]
    cmd[1]: argv=["grep", "foo"],redirs=[(REDIR_OUT, "out.txt")]
  cmd_table[1]:
    cmd[0]: argv=["echo", "done"],redirs=[]
```

### 3.4 Algoritmo de Parsing — Descida Recursiva

O parser de **descida recursiva** tem uma função por regra gramatical. Cada função consome tokens da lista e constrói os nós da AST:

```
parse_list()        → chama parse_pipeline() repetidamente até EOF
parse_pipeline()    → chama parse_command(), depois verifica por PIPE
parse_command()     → chama parse_simple_command()
parse_simple_command() → recolhe cmd_word, argumentos e redirecionamentos
parse_io_redirect() → consome o operador de redirecionamento e o filename
```

**Padrão de consumo:**

O parser mantém um **ponteiro para o token atual** na lista. As funções:
1. Inspecionam o tipo do token atual (**peek**)
2. Se corresponde ao esperado, avançam o ponteiro (**consume/advance**)
3. Se não corresponde, retornam `NULL` ou sinalizam erro

```c
/* Padrão típico de uma função de parsing */
t_cmd *parse_simple_command(t_token **tokens)
{
    t_cmd *cmd;

    if (!tokens || !*tokens || (*tokens)->type == TOKEN_EOF)
        return (NULL);
    cmd = init_cmd();
    if (!cmd)
        return (NULL);
    parse_cmd_prefix(tokens, cmd);   /* redirs antes do cmd */
    parse_cmd_word(tokens, cmd);     /* argv[0] */
    parse_cmd_suffix(tokens, cmd);   /* args + redirs depois do cmd */
    if (!cmd->argv)
        return (free_cmd(cmd), NULL);
    return (cmd);
}
```

**Deteção de Erros Sintáticos:**

O parser deve detetar e reportar erros como:
```bash
echo |          # Pipe sem comando à direita
> file          # Redirecionamento sem comando
| | echo test   # Dois pipes consecutivos
```

Mensagem de erro POSIX:
```
minishell: syntax error near unexpected token '|'
```

### 3.5 Limitações e Regras — Contexto 42 / Norminette

**Edge cases críticos:**

| Caso | Input | Comportamento Esperado na AST |
|------|-------|-------------------------------|
| Múltiplos pipes | `a \| b \| c` | 1 cmd_table, 3 cmds |
| Redirecionamento antes do comando | `< input cat` | cmd:cat, redir_in:input |
| Múltiplos redirecionamentos | `cat < a > b` | cmd:cat, redir_in:a, redir_out:b |
| Heredoc | `cat << EOF` | cmd:cat, redir heredoc:EOF |
| Pipeline com redir | `ls \| cat > out` | redir pertence ao cat, não ao ls |

**Norminette:**
- Cada função do parser ≤ 25 linhas
- Sem parâmetros a mais de 4 por função (passar estruturas por ponteiro)
- A lógica de `peek/consume` deve ser abstraída em funções auxiliares

### 3.6 Funções Permitidas Relevantes

#### `malloc` / `free`
- Cada nó `t_cmd`, `t_cmd_table`, `t_ast` é alocado no heap
- Existir sempre uma função `free_ast(t_ast *ast)` que liberta toda a estrutura

#### Funções da tua `libft`
```c
void    *ft_lstnew(void *content);  /* criar nós da t_list */
void     ft_lstadd_back(t_list **alst, t_list *new);
void     ft_lstclear(t_list **lst, void (*del)(void *));
int      ft_lstsize(t_list *lst);
```

#### Nenhuma função de parsing está disponível nas funções permitidas
O sujeito não permite `yacc`, `bison`, ou similares. O parser deve ser escrito puramente em C.

### 3.7 Metodologia de Aplicação (Pseudocódigo)

```
FUNÇÃO parse(token_list: lista_de_tokens) → t_ast:
    ast = alocar t_ast
    token_atual = &token_list[0]
    
    ENQUANTO token_atual.type != TOKEN_EOF:
        cmd_table = parse_pipeline(&token_atual)
        SE cmd_table == NULL:
            SINALIZAR ERRO
            LIBERTAR ast
            RETORNAR NULL
        ft_lstadd_back(&ast->cmd_tables, ft_lstnew(cmd_table))
        
        SE token_atual.type == TOKEN_SEMICOLON:
            cmd_table->delimiter = ';'
            AVANÇAR token_atual
    
    RETORNAR ast


FUNÇÃO parse_pipeline(token: **t_token) → t_cmd_table:
    cmd_table = alocar t_cmd_table
    
    ENQUANTO token.type != TOKEN_SEMICOLON E != TOKEN_EOF:
        cmd = parse_simple_command(token)
        SE cmd == NULL:
            ERRO SINTÁTICO e BREAK
        ft_lstadd_back(&cmd_table->cmds, ft_lstnew(cmd))
        
        SE token.type == TOKEN_PIPE:
            AVANÇAR token
            SE próximo token é EOF ou PIPE:
                ERRO: "syntax error near unexpected token '|'"
                LIBERTAR e RETORNAR NULL
    
    RETORNAR cmd_table


FUNÇÃO parse_simple_command(token: **t_token) → t_cmd:
    cmd = alocar t_cmd
    args = lista vazia
    
    // Redirecionamentos podem aparecer antes, durante ou depois dos args
    ENQUANTO token.type != PIPE E != SEMICOLON E != EOF:
        SE token.type == REDIR_IN ou REDIR_OUT ou REDIR_APPEND ou HEREDOC:
            redir = parse_redirection(token)  // consume o operador E o filename
            adicionar_redir(&cmd->redirs, redir)
        SENÃO SE token.type == WORD:
            adicionar_arg(&args, token.value)
            AVANÇAR token
        SENÃO:
            BREAK
    
    cmd->argv = lista_para_array_null_terminated(args)
    RETORNAR cmd


FUNÇÃO parse_redirection(token: **t_token) → t_redir:
    redir = alocar t_redir
    redir->type = token.type    // REDIR_IN, REDIR_OUT, etc.
    AVANÇAR token               // consumir o operador
    
    SE token.type != WORD:
        ERRO: "syntax error near unexpected token 'newline'"
        RETORNAR NULL
    
    redir->file = ft_strdup(token.value)
    AVANÇAR token               // consumir o filename
    RETORNAR redir
```

### 3.8 Metodologia de Testes

**Teste 1 — Pipeline simples:**
```bash
# Input: "ls | cat | wc"
# AST: 1 cmd_table, 3 cmds
# cmd[0].argv = ["ls", NULL]
# cmd[1].argv = ["cat", NULL]
# cmd[2].argv = ["wc", NULL]
```

**Teste 2 — Redirecionamentos:**
```bash
# Input: "< input.txt cat > output.txt"
# cmd[0].argv = ["cat", NULL]
# cmd[0].redirs = [(REDIR_IN, "input.txt"), (REDIR_OUT, "output.txt")]
```

**Teste 3 — Multiple cmd_tables:**
```bash
# Input: "ls ; echo done ; pwd"
# AST: 3 cmd_tables, cada uma com 1 cmd
```

**Teste 4 — Erros sintáticos:**
```bash
# Input: "|"              → erro: syntax error near unexpected token '|'
# Input: "echo >"         → erro: syntax error near unexpected token 'newline'
# Input: "ls | | cat"     → erro: syntax error near unexpected token '|'
```

**Debug visual — imprimir a AST:**
```c
/* Implementar função de debug para visualizar a AST antes de executar */
void debug_print_ast(t_ast *ast);
```

---

## 4. Conclusão

### 4.1 Síntese

O Parser transforma a sequência plana de tokens numa estrutura hierárquica (`t_ast`) que captura a intenção do utilizador com precisão. A descida recursiva é a abordagem ideal para o Minishell: é simples de implementar, fácil de depurar, e tem complexidade O(n) no tamanho do input.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Redirs podem aparecer em qualquer posição** | `< in cmd > out arg` é válido | O parser deve aceitar redirs intercalados com os argumentos |
| **Pipe no final** | `echo test \|` provoca acesso ao token seguinte que é NULL | Verificar `TOKEN_EOF` após consumir `PIPE` |
| **Memory leak na árvore** | Se o parsing falha a meio, nós já alocados ficam pendentes | Função `free_ast()` robusta e chamada em todos os caminhos de erro |
| **argv NULL** | Comando sem WORD (ex: só redirecionamentos) | Verificar `cmd->argv == NULL` antes de executar |
| **Heredoc delimitador** | O `value` do token HEREDOC é a palavra seguinte, não `<<` | Avançar o token após `<<` e usar o WORD seguinte como delimitador |

### 4.3 Integração no Projeto Final

A `t_ast` produzida pelo Parser é o input direto do **Executor (Módulos 05/06/07)**. A qualidade da estrutura de dados define a facilidade de implementação do executor. Um bom design da `t_ast` traduz-se diretamente em código de execução mais limpo.

---

## 5. Bibliografia

### Documentação Oficial

- [POSIX.1-2017 — §2.10 Shell Grammar](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_10)
- [POSIX.1-2017 — §2.3 Token Recognition](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_03)

### Livros Clássicos

- **Alfred V. Aho, Monica S. Lam, Ravi Sethi, Jeffrey D. Ullman** — *Compilers: Principles, Techniques, and Tools*, 2nd Ed.  
  Capítulo 4: Syntax Analysis — Recursive Descent Parsing
- **Jack W. Crenshaw** — *Let's Build a Compiler* (1988-1995) — Tutorial clássico de parsing por descida recursiva

### Arquitetura de Referência

- **DimitriDaSilva/42_minishell** — [README.md — Parsing Section](https://github.com/DimitriDaSilva/42_minishell)  
  Estruturas `t_ast`, `t_cmd_table`, `t_cmd`, `t_redir` e fluxo de `parse.c`

### Recursos Web

- **GNU Bash Manual** — §3.2 Shell Commands  
  https://www.gnu.org/software/bash/manual/bash.html#Shell-Commands
- **Wikipedia** — Recursive descent parser  
  https://en.wikipedia.org/wiki/Recursive_descent_parser

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
