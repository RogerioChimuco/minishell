# 01 — Arquitetura Geral & Ciclo de Vida do Shell

---

## Índice

- [01 — Arquitetura Geral \& Ciclo de Vida do Shell](#01--arquitetura-geral--ciclo-de-vida-do-shell)
  - [Índice](#índice)
  - [2. Introdução](#2-introdução)
    - [2.1 O que é um Shell?](#21-o-que-é-um-shell)
    - [2.2 Contexto Histórico](#22-contexto-histórico)
    - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
    - [2.4 Problema que o Shell resolve na Ciência da Computação](#24-problema-que-o-shell-resolve-na-ciência-da-computação)
  - [3. Desenvolvimento](#3-desenvolvimento)
    - [3.1 Teoria Profunda — O Ciclo de Vida](#31-teoria-profunda--o-ciclo-de-vida)
      - [Fase 1 — READ (Leitura)](#fase-1--read-leitura)
      - [Fase 2 — LEX (Análise Léxica / Tokenização)](#fase-2--lex-análise-léxica--tokenização)
      - [Fase 3 — PARSE (Análise Sintática)](#fase-3--parse-análise-sintática)
      - [Fase 4 — EXECUTE (Execução)](#fase-4--execute-execução)
    - [3.2 A Pipeline de Execução Detalhada](#32-a-pipeline-de-execução-detalhada)
    - [3.3 Limitações e Regras — Contexto 42 / Norminette](#33-limitações-e-regras--contexto-42--norminette)
    - [3.4 Funções Permitidas Relevantes](#34-funções-permitidas-relevantes)
      - [`readline`](#readline)
      - [`add_history`](#add_history)
      - [`rl_clear_history`](#rl_clear_history)
      - [`free`](#free)
      - [`exit`](#exit)
    - [3.5 Metodologia de Aplicação (Pseudocódigo)](#35-metodologia-de-aplicação-pseudocódigo)
    - [3.6 Metodologia de Testes](#36-metodologia-de-testes)
  - [4. Conclusão](#4-conclusão)
    - [4.1 Síntese](#41-síntese)
    - [4.2 Armadilhas Comuns (Pitfalls)](#42-armadilhas-comuns-pitfalls)
    - [4.3 Integração no Projeto Final](#43-integração-no-projeto-final)
  - [5. Bibliografia](#5-bibliografia)
    - [Documentação Oficial e Man Pages](#documentação-oficial-e-man-pages)
    - [Tutoriais e Artigos Técnicos](#tutoriais-e-artigos-técnicos)
    - [Livros Clássicos](#livros-clássicos)
    - [Repositórios de Referência](#repositórios-de-referência)
    - [Padrões](#padrões)

---

## 2. Introdução

### 2.1 O que é um Shell?

Um **shell** é um interpretador de linguagem de comandos — um programa que lê input do utilizador (ou de um ficheiro/script), interpreta esse input como comandos e instrui o sistema operativo a executá-los. O nome "shell" (concha) é metafórico: ele envolve o núcleo (kernel) do sistema operativo, apresentando-o ao utilizador através de uma interface de texto.

Do ponto de vista formal da especificação POSIX:

> *"The shell is a command language interpreter."*  
> — POSIX.1-2017, Section 2.1

O shell não é apenas um executor de programas. Ele é simultaneamente:
- Um **analisador léxico**: decompõe texto em tokens
- Um **analisador sintático**: constrói estrutura de comandos a partir de tokens
- Um **gestor de processos**: controla a criação e o ciclo de vida de processos
- Um **gestor de I/O**: manipula descritores de ficheiro, pipes e redirecionamentos

### 2.2 Contexto Histórico

| Ano | Marco |
|-----|-------|
| 1971 | Ken Thompson escreve o primeiro shell Unix (`/bin/sh`), chamado "Thompson shell" |
| 1979 | Stephen Bourne cria o **Bourne Shell** (`sh`) para Unix Version 7 — base do padrão POSIX |
| 1983 | David Korn cria o **KornShell** (`ksh`) |
| 1989 | Brian Fox cria o **Bash** (Bourne Again SHell) para o projeto GNU |
| 1992 | IEEE publica o **POSIX.1** que formaliza o comportamento do shell |
| 2019 | Bash 5.0 é lançado; macOS muda o default shell para `zsh` |

O **Bash** (Bourne Again SHell) é o alvo de emulação do projeto Minishell da 42.

### 2.3 Relevância no Projeto Minishell da 42

O Minishell é o projeto que introduz o estudante da 42 ao mundo dos processos Unix, I/O e gestão de sinais. Compreender a **arquitetura geral** antes de escrever uma linha de código é crítico porque:

1. Decide a **estrutura de dados** que usarás para representar comandos
2. Define a **ordem correta de implementação** (não podes implementar pipes sem ter `fork()` a funcionar)
3. Evita refactoring tardio e doloroso quando a base está errada

### 2.4 Problema que o Shell resolve na Ciência da Computação

O shell resolve o problema fundamental de **abstração da interface com o kernel**. Sem um shell:
- O utilizador teria de invocar diretamente syscalls (impossível sem programar em C/ASM)
- Não haveria composição de comandos (pipes, redirecionamentos)
- Não haveria automação repetível (scripts)

O shell implementa o padrão **REPL** (Read-Evaluate-Print Loop), um dos padrões mais fundamentais em Ciência da Computação, presente em interpretadores Python, Node.js REPL, e todos os shells interativos.

---

## 3. Desenvolvimento

### 3.1 Teoria Profunda — O Ciclo de Vida

O POSIX descreve o funcionamento de um shell como uma sequência de 7 passos ordenados:

```
1. Leitura do input (linha de texto)
2. Quebra do input em tokens (palavras e operadores)
3. Parsing — construção de comandos simples e compostos
4. Expansões (tilde, parâmetros, substituição de comandos, aritmétricas)
5. Redirecionamentos — aplicados e removidos da lista de parâmetros
6. Execução — função, builtin, ou ficheiro executável
7. Espera por terminação e recolha do exit status
```

Para o **Minishell** especificamente, podemos simplificar para 4 fases principais:

```
┌─────────────────────────────────────────────────────────────────┐
│                    CICLO DE VIDA DO MINISHELL                   │
│                                                                 │
│  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐  │
│  │  READ    │───▶│   LEX    │───▶│  PARSE   │───▶│ EXECUTE  │  │
│  │          │    │          │    │          │    │          │  │
│  │readline()│    │Tokenizer │    │AST Build │    │fork/exec │  │
│  └──────────┘    └──────────┘    └──────────┘    └──────────┘  │
│        ▲                                               │        │
│        └───────────────────────────────────────────────┘        │
│                        (loop infinito)                          │
└─────────────────────────────────────────────────────────────────┘
```

#### Fase 1 — READ (Leitura)

O shell apresenta um **prompt** ao utilizador e lê uma linha completa de input. No Minishell, esta fase usa a biblioteca `readline`, que fornece edição de linha interativa e histórico de comandos.

#### Fase 2 — LEX (Análise Léxica / Tokenização)

O input raw (`"echo hello | cat -e"`) é decomposto numa sequência de **tokens** tipados. Um token pode ser:
- Uma **WORD** (palavra): `echo`, `hello`, `cat`, `-e`
- Um **operador**: `|`, `>`, `<`, `>>`, `<<`
- Um **delimitador**: `;`, `&&`, `||`

As regras de quoteamento (`'`, `"`) são aplicadas aqui para determinar os limites dos tokens.

#### Fase 3 — PARSE (Análise Sintática)

Os tokens são organizados numa estrutura hierárquica — a **Abstract Syntax Tree (AST)**. Para o Minishell, esta estrutura segue o padrão:

```
AST
└── cmd_table (separada por ';', '&&', '||')
    └── cmd (separada por '|')
        ├── tokens (lista de palavras: nome + argumentos)
        └── redirs (lista de redirecionamentos)
```

#### Fase 4 — EXECUTE (Execução)

A AST é percorrida e cada `cmd_table` executada. Para cada `cmd`:
- Se for um **builtin** → executado diretamente no processo pai
- Se for um **executável externo** → `fork()` + `execve()` + `waitpid()`

### 3.2 A Pipeline de Execução Detalhada

```
Input: "ls -la | grep main > output.txt"

FASE 1 - READ:
  raw_input = "ls -la | grep main > output.txt"

FASE 2 - LEX:
  tokens = [WORD:"ls", WORD:"-la", PIPE:"|", WORD:"grep",
            WORD:"main", REDIR_OUT:">", WORD:"output.txt"]

FASE 3 - PARSE:
  AST:
    cmd_table:
      cmd[0]: tokens=[ls, -la],   redirs=[]
      cmd[1]: tokens=[grep, main], redirs=[{type:>, file:"output.txt"}]

FASE 4 - EXECUTE:
  - Criar pipe fd[2]
  - fork() → filho 0 executa "ls -la" com stdout→pipe[write]
  - fork() → filho 1 executa "grep main" com stdin←pipe[read], stdout→output.txt
  - waitpid() aguarda ambos
  - exit_status = status do último processo
```

### 3.3 Limitações e Regras — Contexto 42 / Norminette

**Regras da Norminette relevantes para a arquitetura:**

| Regra | Impacto Arquitetural |
|-------|----------------------|
| Máx. 25 linhas por função | Obriga a decomposição funcional rigorosa — cada fase deve ter funções pequenas e bem nomeadas |
| Máx. 5 parâmetros por função | A struct global do shell (`t_msh`) ou structs de estado passadas por ponteiro tornam-se essenciais |
| Proibição de `for` loops | Usa `while` para todos os loops de iteração sobre listas |
| Sem variáveis globais (exceto 1) | O Subject da 42 permite **uma única variável global** — tipicamente para o exit status ou para a struct principal |
| Proibição de funções não listadas | Qualquer chamada de sistema fora da lista do Subject é motivo de falha na avaliação |

**Edge Cases críticos desta fase:**
- Input vazio (utilizador pressiona Enter sem digitar nada) → não deve crashar, deve re-apresentar o prompt
- Input com apenas whitespace → idem
- `Ctrl-D` na leitura → deve sair limpo (sem memory leak, com mensagem `exit`)
- `Ctrl-C` durante o prompt → deve apresentar novo prompt na linha abaixo

**Gestão de memória:**
- A string retornada por `readline()` é alocada no heap → deve ser libertada com `free()` após uso
- A AST construída em PARSE deve ser libertada completamente após EXECUTE
- Valgrind/ASan devem reportar 0 leaks em cada iteração do loop

### 3.4 Funções Permitidas Relevantes

#### `readline`
```c
#include <readline/readline.h>
char *readline(const char *prompt);
```
- Apresenta `prompt` no terminal
- Lê uma linha completa do utilizador com edição interativa
- Retorna ponteiro para string alocada no heap (deve ser `free()`d)
- Retorna `NULL` em caso de EOF (`Ctrl-D`)

#### `add_history`
```c
#include <readline/history.h>
void add_history(const char *string);
```
- Adiciona `string` ao histórico de comandos da readline
- Permite navegar com as setas ↑ ↓

#### `rl_clear_history`
```c
void rl_clear_history(void);
```
- Limpa toda a lista de histórico — deve ser chamada no cleanup final para evitar leaks da readline

#### `free`
```c
#include <stdlib.h>
void free(void *ptr);
```
- Liberta memória alocada por `malloc`/`calloc`/`readline`
- Chamar `free(NULL)` é seguro (no-op)

#### `exit`
```c
#include <stdlib.h>
void exit(int status);
```
- Termina o processo com o código de status dado
- `EXIT_SUCCESS` (0) ou `EXIT_FAILURE` (1) são as macros padrão

### 3.5 Metodologia de Aplicação (Pseudocódigo)

> ⚠️ **Regra de Ouro**: O pseudocódigo abaixo descreve a lógica estrutural — **não** é código a copiar. O objetivo é compreender a sequência de decisões.

```
// Estrutura principal do minishell

FUNÇÃO principal(argc, argv, envp):
    inicializar_ambiente(envp)         // duplicar envp, configurar sinais
    inicializar_terminal()             // readline, tcsetattr
    
    ENQUANTO verdadeiro:
        linha = ler_linha_do_utilizador(prompt)
        
        SE linha É NULA:               // Ctrl-D → EOF
            imprimir "exit"
            terminar_limpo()
            SAIR
        
        SE linha ESTÁ VAZIA:           // Enter sem input
            libertar(linha)
            CONTINUAR
        
        adicionar_ao_historico(linha)
        
        SE validar_sintaxe(linha) É INVÁLIDO:
            imprimir_erro_sintaxe()
            libertar(linha)
            CONTINUAR
        
        ast = construir_ast(linha)     // FASE PARSE
        executar_ast(ast)              // FASE EXECUTE
        destruir_ast(ast)              // Libertar memória
        libertar(linha)
    
    // Nunca chega aqui normalmente
    limpar_tudo()
    RETORNAR exit_status
```

**Estrutura de dados central** (a definir):

```
// Hierarquia de structs
t_shell {
    t_list    *env          // lista de variáveis de ambiente
    t_ast     *ast          // árvore sintática atual
    int        exit_status  // $?
}

t_ast {
    t_list    *cmd_tables   // lista de cmd_tables
}

t_cmd_table {
    t_list    *cmds         // lista de cmds (separados por |)
    char      *delimiter    // ";" "&&" "||" ou NULL
    int      **pipes        // array de pipes criados
}

t_cmd {
    t_list    *tokens       // lista de palavras (cmd + args)
    t_list    *redirs       // lista de redirecionamentos
}
```

### 3.6 Metodologia de Testes

**Teste 1 — Loop básico (sem parsing):**
```bash
# Verificar que o shell apresenta o prompt e aceita input
./minishell
minishell$ 
minishell$    (Enter vazio — deve re-apresentar prompt)
minishell$ ^D (Ctrl-D — deve sair)
$? = 0
```

**Teste 2 — Gestão de memória (Valgrind):**
```bash
valgrind --leak-check=full --track-origins=yes ./minishell
# Executar vários comandos, sair com Ctrl-D
# Esperado: 0 bytes definitivamente perdidos
# Nota: readline tem leaks "still reachable" internos — são aceitáveis
```

**Teste 3 — Robustez do loop:**
```bash
# Executar 100 iterações do loop com input variado
for i in $(seq 1 100); do echo "echo test $i"; done | ./minishell
# Esperado: 100 linhas de output "test 1" a "test 100"
# Sem crashes, sem leaks
```

**Teste 4 — Sinal Ctrl-C no prompt:**
```bash
./minishell
minishell$ ^C    # deve apresentar novo prompt, não sair
minishell$
```

---

## 4. Conclusão

### 4.1 Síntese

A arquitetura do Minishell obedece ao padrão **REPL** e decompõe-se em 4 fases interdependentes: Read, Lex, Parse, Execute. A compreensão desta pipeline, antes de qualquer implementação, é o que separa um projeto bem estruturado de um projeto que cresce em complexidade incontrolável.

A escolha das estruturas de dados hierárquicas (`t_shell → t_ast → t_cmd_table → t_cmd`) é diretamente inspirada pela gramática POSIX formal do shell e permite que cada fase opere de forma isolada e testável.

### 4.2 Armadilhas Comuns (Pitfalls)

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Não libertar a string de `readline`** | Memory leak em cada iteração do loop | `free(line)` no final de cada iteração, mesmo em caso de erro |
| **Loop `while(1)` sem condição de saída** | O shell nunca termina em `Ctrl-D` | Verificar se `readline()` retorna `NULL` |
| **Estrutura de dados plana** | Tentar gerir tudo com arrays de strings | Definir a hierarquia de structs antes de começar a fase Parse |
| **Mistura de fases** | Fazer parsing e execução ao mesmo tempo | Separar claramente as 4 fases — modularidade é obrigatória |
| **Gestão da variável global** | Usar mais de uma variável global | O Subject permite apenas 1 variável global — planeia o seu uso com cuidado |
| **Não reinicializar o exit status** | `$?` fica com valor lixo entre comandos | Atualizar `exit_status` após cada execução de comando |

### 4.3 Integração no Projeto Final

Este módulo define o **esqueleto** onde todos os outros módulos se encaixam:
- O **Lexer (Módulo 02)** opera sobre a string retornada por `readline()`
- O **Parser (Módulo 03)** constrói a `t_ast` a partir dos tokens do Lexer
- As **Variáveis de Ambiente (Módulo 04)** populam o campo `env` da `t_shell`
- **fork/execve (Módulo 05)** é invocado pela fase Execute
- **Pipes (Módulo 06)** e **Redirecionamentos (Módulo 07)** são configurados dentro da fase Execute
- **Sinais (Módulo 08)** são registados durante a inicialização
- **Builtins (Módulo 09)** são despachados dentro da fase Execute
- **readline (Módulo 10)** é a implementação da fase Read

---

## 5. Bibliografia

### Documentação Oficial e Man Pages

- [POSIX.1-2017 — Shell Command Language, Section 2.1](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)
- `man 3 readline` — Documentação da GNU readline
- `man 3 free` — `man 3 malloc`
- `man 2 fork` — `man 2 execve` — `man 2 waitpid`
- `man 2 exit` — `man 3 exit`

### Tutoriais e Artigos Técnicos

- **Stephen Brennan** — *"Tutorial — Write a Shell in C"* (2015)  
  https://brennan.io/2015/01/16/write-a-shell-in-c/
  
- **Achraf Belarif** — *"42Cursus-Minishell"*, Medium (2021)  
  https://achrafbelarif.medium.com/42cursus-minishell-29cd25f972e6

- **GNU Bash Reference Manual**  
  https://www.gnu.org/software/bash/manual/bash.html#Shell-Operation

### Livros Clássicos

- **W. Richard Stevens & Stephen A. Rago** — *Advanced Programming in the UNIX Environment*, 3rd Ed. (2013), Addison-Wesley  
  ISBN: 978-0321637734 — Capítulos 7 (Process Environment), 8 (Process Control), 15 (Interprocess Communication)

- **Michael Kerrisk** — *The Linux Programming Interface* (2010), No Starch Press  
  ISBN: 978-1593272203 — Capítulos 24-26 (Process Creation, Execution, Termination)

### Repositórios de Referência

- **DimitriDaSilva/42_minishell** (arquitetura real de estudante da 42)  
  https://github.com/DimitriDaSilva/42_minishell

- **Swoorup/mysh** (implementação de referência alternativa)  
  https://github.com/Swoorup/mysh

- **brenns10/lsh** (implementação educacional minimalista)  
  https://github.com/brenns10/lsh

### Padrões

- **IEEE Std 1003.1-2017 (POSIX)** — The Open Group  
  https://pubs.opengroup.org/onlinepubs/9699919799/

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
