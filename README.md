# minishell — As beautiful as a shell

> *This project has been created as part of the 42 curriculum by rochimuc, luqalmei.*

---

## Descrição

O **minishell** é um interpretador de comandos (shell) escrito em C, desenvolvido como parte do currículo da 42 School. O objectivo é recriar, do zero, o comportamento essencial do **Bash** — o terminal que os utilizadores de Linux e macOS usam todos os dias.

> "Com o Minishell, viajamos no tempo e vivemos os desafios que os programadores enfrentaram antes de existir qualquer interface gráfica."

O projecto ensina como um sistema operativo lança processos, como ficheiros são geridos ao nível dos file descriptors, e como a comunicação entre processos funciona através de pipes — conceitos fundamentais em qualquer sistema Unix/Linux.

---

## O que estamos a construir

Um **mini interpretador de comandos interactivo** que:

1. Aceita comandos digitados pelo utilizador
2. Interpreta e analisa (parse) esses comandos
3. Lança executáveis reais do sistema (`/bin/ls`, `/usr/bin/cat`, etc.)
4. Gere processos filhos com `fork` + `execve`
5. Suporta comunicação entre comandos via `pipe`
6. Redireciona entradas e saídas para/de ficheiros
7. Expande variáveis de ambiente (`$HOME`, `$PATH`, `$?`)
8. Implementa comandos internos (builtins) sem lançar processos externos
9. Lida com sinais do teclado (`Ctrl-C`, `Ctrl-D`, `Ctrl-\`)
10. Mantém um histórico de comandos navegável

---

## Funcionalidades do Projecto

### Prompt Interactivo
```
minishell$ _
```
O shell exibe um prompt e aguarda input do utilizador. Enquanto não há input, o programa fica bloqueado em `readline()`.

---

### Lexer — Tokenização da linha de comando

A linha de input é quebrada em tokens:

| Token | Exemplo | Significado |
|---|---|---|
| `WORD` | `ls`, `echo`, `hello` | Palavra simples |
| `PIPE` | `\|` | Separador de pipeline |
| `REDIR_IN` | `<` | Redireccionamento de entrada |
| `REDIR_OUT` | `>` | Redireccionamento de saída |
| `REDIR_APPEND` | `>>` | Saída em modo append |
| `HEREDOC` | `<<` | Leitura até delimitador |
| `DQUOTE` | `"texto $VAR"` | Aspas duplas (expandem `$`) |
| `SQUOTE` | `'texto literal'` | Aspas simples (tudo literal) |

**Regras de citação (POSIX):**
- `'aspas simples'` → NADA é interpretado dentro delas
- `"aspas duplas"` → `$VARIAVEL` e `$?` são expandidos, o resto é literal
- Aspas não fechadas → **erro de sintaxe**

---

### Parser — Análise Sintáctica e Construção da AST

O parser converte os tokens numa estrutura em árvore (**AST — Abstract Syntax Tree**) que representa a lógica do comando:

```
echo hello | cat -n > out.txt

          PIPE
         /    \
      CMD       CMD
    [echo]     [cat -n]
    [hello]    REDIR_OUT: out.txt
```

O parser detecta erros de sintaxe como:
- `| cmd` — pipe sem comando à esquerda
- `cmd |` — pipe sem comando à direita
- `cmd >` — redireccionamento sem ficheiro
- `cmd << ` — heredoc sem delimitador

---

### Expansão de Variáveis

Antes de executar, todas as palavras são varridas e as variáveis são expandidas:

```bash
echo $HOME          → /home/rochimuc
echo $USER          → rochimuc
echo $?             → 0  (exit status do último comando)
echo "$PATH"        → /usr/local/bin:/usr/bin:/bin
echo '$PATH'        → $PATH  (literal — aspas simples)
```

Casos especiais:
- `$` sem nome válido a seguir → literalmente `$`
- `${VAR}` → **não requerido** pelo subject (apenas `$VAR`)
- Variável não definida → expande para string vazia `""`

---

### Redireccionamentos

```bash
# Entrada de ficheiro
cmd < ficheiro.txt          # stdin lê de ficheiro.txt

# Saída para ficheiro (sobrescreve)
cmd > saida.txt             # stdout escreve em saida.txt

# Saída em append (acrescenta ao ficheiro)
cmd >> saida.txt            # stdout acrescenta em saida.txt

# Heredoc — lê até linha igual ao delimitador
cmd << EOF
linha 1
linha 2
EOF
```

**Regras de implementação:**
- Redireccionamentos são aplicados **antes** do execve
- São usados `open()` + `dup2()` para redirigir os file descriptors 0 (stdin) e 1 (stdout)
- Múltiplos redireccionamentos na mesma linha são válidos; aplica-se o **último** de cada tipo
- Heredoc: lê linha a linha de stdin até encontrar o delimitador; **não** entra no histórico

---

### Pipes

```bash
cmd1 | cmd2 | cmd3
```

O output de `cmd1` é ligado ao input de `cmd2` via um pipe. O output de `cmd2` é ligado ao input de `cmd3`.

**Implementação:**
```
pipe(fd[2])  →  fd[0] = read end,  fd[1] = write end

cmd1: stdout → fd[1] (write end)
cmd2: stdin  → fd[0] (read end)   stdout → next pipe
cmd3: stdin  → previous fd[0]
```

Regras críticas:
- Todos os file descriptors não usados **devem ser fechados** imediatamente após `fork`
- O processo pai não escreve nem lê nos pipes — apenas os filhos
- O pai faz `waitpid` para **todos** os filhos antes de mostrar o próximo prompt

---

### Execução de Comandos

**Comando externo (path absoluto ou relativo):**
```bash
/bin/ls -la
./meu_programa
```

**Comando externo (pesquisa no PATH):**
```bash
ls        →  procura em cada directório de $PATH
cat       →  /bin/cat encontrado → execve("/bin/cat", ...)
git       →  /usr/bin/git encontrado → execve(...)
xyz       →  não encontrado → "minishell: xyz: command not found"
```

**Fluxo de execução:**
```
1. fork() → cria processo filho
2. filho:  aplicar redireccionamentos (dup2)
           fechar file descriptors não necessários
           execve(caminho, args, envp)
3. pai:    waitpid(filho) → obtém exit status
           guarda em $?
```

---

### Builtins — Comandos Internos

Os builtins são executados **no próprio processo** (sem fork) porque precisam modificar o estado da shell:

| Builtin | Comportamento |
|---|---|
| `echo [-n] [args...]` | Imprime args; `-n` suprime newline final |
| `cd [path]` | Muda directório; sem path → `$HOME`; actualiza `$PWD` e `$OLDPWD` |
| `pwd` | Imprime directório actual (usa `getcwd`) |
| `export [VAR=valor]` | Define/exporta variável de ambiente |
| `unset VAR` | Remove variável de ambiente |
| `env` | Lista todas as variáveis de ambiente exportadas |
| `exit [n]` | Termina a shell; exit status = n (0–255) |

**Nota:** Builtins num pipeline **são** executados num fork (porque precisam de partilhar o pipe), mas builtins sozinhos na linha de comando são executados directamente.

---

### Sinais

| Sinal | Contexto | Comportamento |
|---|---|---|
| `Ctrl-C` (SIGINT) | Prompt vazio | Nova linha + novo prompt |
| `Ctrl-C` (SIGINT) | Processo filho a correr | Termina o processo filho |
| `Ctrl-D` (EOF) | Prompt vazio | Sai da shell (equivalente a `exit`) |
| `Ctrl-\` (SIGQUIT) | Prompt vazio | **Nada acontece** |
| `Ctrl-\` (SIGQUIT) | Processo filho | Termina filho + imprime "Quit: 3" |

**Restrição importante:** Apenas **1 variável global** é permitida, e deve conter apenas o número do sinal. Não pode ser uma struct nem ter acesso a estruturas de dados principais.

---

## Entradas e Saídas

### Entradas

| Fonte | Exemplo | Descrição |
|---|---|---|
| Teclado (stdin) | `ls -la \| grep doc` | Linha de comando digitada pelo utilizador |
| Ficheiro (`<`) | `cmd < input.txt` | stdin redirigido de ficheiro |
| Heredoc (`<<`) | `cat << EOF` | stdin lido até delimitador |
| Variáveis de ambiente | `$PATH`, `$HOME` | Expandidas antes da execução |
| `$?` | `echo $?` | Exit status do último comando |

### Saídas

| Destino | Exemplo | Descrição |
|---|---|---|
| Terminal (stdout) | `echo hello` | Saída normal para o ecrã |
| Ficheiro (`>`) | `cmd > out.txt` | stdout redirigido para ficheiro |
| Ficheiro append (`>>`) | `cmd >> log.txt` | stdout acrescenta ao ficheiro |
| Pipe (`\|`) | `ls \| grep .c` | stdout de cmd1 → stdin de cmd2 |
| Terminal (stderr) | `ls naoexiste` | Erros são escritos em stderr (fd 2) |
| `$?` | — | Exit status (0=sucesso, 1-255=erro) |

---

## O que o projecto NÃO deve fazer

De acordo com o subject, os seguintes comportamentos **não são requeridos** (não implementar):

- `;` — separador de comandos em linha (bash suporta, minishell não)
- `\` — backslash de continuação ou escape
- `&&` e `||` — operadores lógicos (são bonus)
- Subshells `(...)` — são bonus
- Wildcards `*` — são bonus
- `>>` com `<` na mesma posição (combinação `<>`) — não requerido
- `${VAR}`, `$(cmd)`, `$((expr))` — não requeridos
- Histórico persistente entre sessões — não requerido

---

## Bonus (após obrigatório completo)

Só implementar **depois** de todos os critérios obrigatórios passarem com Norminette + Valgrind limpos:

```bash
# Operadores lógicos
cmd1 && cmd2     # cmd2 só executa se cmd1 suceder ($? == 0)
cmd1 || cmd2     # cmd2 só executa se cmd1 falhar ($? != 0)

# Subshells
(cmd1 | cmd2) && cmd3

# Wildcards
ls *.c           # expande para todos os .c no directório actual
```

---

## Restrições Técnicas (Norminette)

| Regra | Limite |
|---|---|
| Linhas por função | ≤ 25 |
| Parâmetros por função | ≤ 4 |
| Funções por ficheiro `.c` | ≤ 5 |
| Variáveis globais | ≤ 1 (só o número do sinal) |
| `for` loops | **Proibido** — usar `while` |
| `do..while` | **Proibido** |
| `switch` | **Proibido** |
| `goto` | **Proibido** |
| Declarações mistas | **Proibido** (declarar e atribuir na mesma linha) |
| Memory leaks | **0 tolerados** (excepto os internos do readline) |

---

## Funções Externas Autorizadas

```
readline, rl_clear_history, rl_on_new_line, rl_replace_line,
rl_redisplay, add_history, printf, malloc, free, write, access,
open, read, close, fork, wait, waitpid, wait3, wait4,
signal, sigaction, sigemptyset, sigaddset, kill, exit,
getcwd, chdir, stat, lstat, fstat, unlink, execve,
dup, dup2, pipe, opendir, readdir, closedir,
strerror, perror, isatty, ttyname, ttyslot, ioctl,
getenv, tcsetattr, tcgetattr, tgetent, tgetflag, tgetnum,
tgetstr, tgoto, tputs
```

---

## Instruções de Compilação e Execução

```bash
# Compilar
make

# Executar
./minishell

# Limpar objectos
make clean

# Limpar tudo (incluindo binário)
make fclean

# Recompilar tudo
make re
```

### Makefile obrigatório (regras mínimas)
```makefile
NAME   = minishell
FLAGS  = -Wall -Wextra -Werror

$(NAME): # compila todos os .c

all:     # compila $(NAME)
clean:   # remove .o
fclean:  # clean + remove $(NAME)
re:      # fclean + all
bonus:   # compila com ficheiros _bonus.c
```

---

## Arquitectura do Projecto

```
minishell/
├── Makefile
├── README.md
├── includes/
│   └── minishell.h          ← structs, defines, protótipos
├── src/
│   ├── main.c               ← loop principal: readline → parse → exec
│   ├── lexer/
│   │   └── lexer.c          ← tokenização da linha de input
│   ├── parser/
│   │   └── parser.c         ← construção da AST
│   ├── expand/
│   │   └── expand.c         ← expansão de variáveis $VAR, $?
│   ├── executor/
│   │   └── executor.c       ← fork, execve, waitpid
│   ├── pipes/
│   │   └── pipes.c          ← criação e gestão de pipes
│   ├── redirections/
│   │   └── redir.c          ← open, dup2, heredoc
│   ├── builtins/
│   │   └── builtins.c       ← echo, cd, pwd, export, unset, env, exit
│   ├── signals/
│   │   └── signals.c        ← sigaction, SIGINT, SIGQUIT
│   └── env/
│       └── env.c            ← cópia e gestão do envp
├── libft/                   ← libft própria
└── doc/                     ← documentação de estudo
    ├── 01_Arquitetura_Geral_e_Ciclo_de_Vida.md
    ├── 02_Lexer_e_Tokenizacao.md
    ├── 03_Parser_e_AST.md
    ├── 04_Variaveis_de_Ambiente_e_Expansao.md
    ├── 05_Processos_fork_execve_waitpid.md
    ├── 06_Pipes_e_File_Descriptors.md
    ├── 07_Redirecionamentos.md
    ├── 08_Sinais.md
    ├── 09_Builtins.md
    ├── 10_Readline_Historico_e_Terminal.md
    └── ROADMAP_DEADLINE.md
```

---

## Equipa e Divisão de Trabalho

| Membro | Área | Módulos |
|---|---|---|
| **rochimuc** | Frontend (Interface) | Lexer, Parser, Expansão, Builtins, Readline |
| **luqalmei** | Backend (Execução) | Executor, Pipes, Redireccionamentos, Sinais, Env |

Período de desenvolvimento: **04 a 24 de Março de 2026** — 210h totais (10h/dia).

---

## Recursos e Referências

### Documentação Oficial
- [POSIX Shell Command Language (IEEE Std 1003.1)](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)
- [GNU Bash Manual](https://www.gnu.org/software/bash/manual/bash.html)
- [readline library documentation](https://tiswww.case.edu/php/chet/readline/rltop.html)
- `man bash`, `man fork`, `man execve`, `man pipe`, `man dup2`, `man sigaction`

### Livros e Artigos
- *The Linux Programming Interface* — Michael Kerrisk (capítulos sobre processos, pipes e sinais)
- *Advanced Programming in the UNIX Environment* — W. Richard Stevens
- Chapter 5: Writing Your Own Shell (ficheiro incluído no repositório: `Chapter5-WritingYourOwnShell.pdf`)

### Uso de IA no Projecto
A inteligência artificial (GitHub Copilot / Claude) foi utilizada nas seguintes tarefas:
- **Pesquisa e síntese** da especificação POSIX do shell
- **Geração de documentação** de estudo (pasta `doc/`) com explicações dos conceitos de cada módulo
- **Criação do roadmap** de desenvolvimento com distribuição de tarefas e datas
- **Sugestões de arquitectura** de structs e fluxo de dados
- **Revisão de edge cases** e cenários de teste por módulo

> Todo o código foi escrito, compreendido e validado pelos autores. A IA não gerou código de implementação directamente.

---

*Roadmap detalhado: [doc/ROADMAP_DEADLINE.md](doc/ROADMAP_DEADLINE.md)*
