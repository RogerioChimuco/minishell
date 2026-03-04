# 10 — readline, Histórico e Interface do Terminal

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que é a biblioteca readline?](#21-o-que-é-a-biblioteca-readline)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que este módulo resolve](#24-problema-que-este-módulo-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 `readline()` — Leitura de Linha Interativa](#31-readline--leitura-de-linha-interativa)
  - [3.2 Histórico — `add_history` e Funções Relacionadas](#32-histórico--add_history-e-funções-relacionadas)
  - [3.3 Funções de Controlo do Terminal readline](#33-funções-de-controlo-do-terminal-readline)
  - [3.4 Terminal e `tcsetattr`/`tcgetattr`](#34-terminal-e-tcsetattrtcgetattr)
  - [3.5 Modo Canónico vs. Modo Raw](#35-modo-canónico-vs-modo-raw)
  - [3.6 `isatty`, `ttyname`, `ttyslot`](#36-isatty-ttyname-ttyslot)
  - [3.7 Termcaps — `tgetent`, `tgetstr`, `tgoto`, `tputs`](#37-termcaps--tgetent-tgetstr-tgoto-tputs)
  - [3.8 Limitações e Regras — Contexto 42 / Norminette](#38-limitações-e-regras--contexto-42--norminette)
  - [3.9 Funções Permitidas — Visão Geral](#39-funções-permitidas--visão-geral)
  - [3.10 Metodologia de Aplicação (Pseudocódigo)](#310-metodologia-de-aplicação-pseudocódigo)
  - [3.11 Metodologia de Testes](#311-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que é a biblioteca readline?

A **GNU Readline** é uma biblioteca que fornece edição de linha de comandos interativa, com suporte para:
- Navegação com as setas do teclado (← →)
- Histórico de comandos (↑ ↓)
- Completação automática (Tab, se configurado)
- Atalhos de edição (Emacs-style: Ctrl-A para início da linha, Ctrl-E para fim, etc.)
- Gerenciamento do histórico entre sessões (ficheiro `~/.history`)

É a biblioteca que o próprio Bash usa, e é a que o Minishell deve usar conforme o Subject da 42.

### 2.2 Contexto Histórico e Computacional

A GNU Readline foi criada por **Brian Fox** (1987) como parte do projeto GNU, inicialmente para o GNU Bash. É baseada no sistema de edição de linha de comandos da UNIX `editline` e do clássico Emacs.

Internamente, a readline usa **termcap/terminfo** para emitir sequências de escape compatíveis com diferentes tipos de terminal (xterm, VT100, ANSI, etc.). Esta abstração permite que a mesma biblioteca funcione em dezenas de terminais diferentes.

### 2.3 Relevância no Projeto Minishell da 42

O uso da `readline` é **obrigatório** no Subject da 42. Além disso, a correcta gestão do histórico (sem duplicar entradas, sem crashar com Ctrl-C, sem memory leaks do histórico) é testada durante a avaliação.

As funções do readline necessárias para a integração com o tratamento de sinais (`rl_on_new_line`, `rl_replace_line`, `rl_redisplay`) foram abordadas no Módulo 08 (Sinais) — este módulo aprofunda todo o ecossistema do terminal.

### 2.4 Problema que este módulo resolve

O problema é proporcionar uma **experiência de utilizador profissional** na leitura de input: navegação, histórico, edição. Sem a readline, o utilizador teria apenas um `read()` básico sem edição. A readline resolve também a complexidade de interagir com o sistema de terminal de forma portável.

---

## 3. Desenvolvimento

### 3.1 `readline()` — Leitura de Linha Interativa

```c
#include <readline/readline.h>
char *readline(const char *prompt);
```

**Comportamento:**
- Exibe o `prompt` no terminal
- Lê uma linha de input com edição interativa (setas, delete, etc.)
- Retorna um ponteiro para uma **string alocada no heap** (deve ser `free`d)
- Retorna `NULL` quando recebe EOF (Ctrl-D)
- A string retornada **não inclui o `\n` final**

```c
char *line;

line = readline("minishell$ ");
if (!line)
{
    /* Ctrl-D recebido — sair */
    write(STDOUT_FILENO, "exit\n", 5);
    exit(g_msh.exit_status);
}
if (*line)  /* só adicionar ao histórico se não for linha vazia */
    add_history(line);
/* processar a linha... */
free(line);  /* OBRIGATÓRIO — readline aloca com malloc */
```

**Compilação com readline:**
```makefile
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lreadline
# Em alguns sistemas (macOS/brew): -lreadline -L/opt/homebrew/lib -I/opt/homebrew/include
```

> **macOS (42 iMacs):** O macOS inclui uma versão do `libedit` que emula readline. Dependendo da configuração da máquina, pode ser necessário instalar a GNU readline via Homebrew e ajustar os paths.

**Prompt personalizado:**
```c
/* Usar uma string de prompt com informação útil */
#define PROMPT  "minishell$ "
/* ou dinamicamente: */
char *make_prompt(t_msh *msh)
{
    /* pode incluir o diretório atual, utilizador, etc. */
    return (ft_strdup("minishell$ "));
}
```

### 3.2 Histórico — `add_history` e Funções Relacionadas

```c
#include <readline/history.h>

void add_history(const char *string);
/* Adiciona a string ao histórico em memória */
/* O utilizador pode navegar com as setas ↑ e ↓ */

void rl_clear_history(void);
/* Limpa todo o histórico em memória e liberta a memória associada */
/* Deve ser chamado antes de sair (clean_and_exit) para evitar leaks */
```

**Gestão do histórico:**
```c
/* No loop principal: */
if (line && *line)         /* não adicionar linhas vazias */
    add_history(line);

/* Na saída do shell: */
rl_clear_history();        /* limpar antes de exit para evitar leaks */
free(line);
```

**Detectar duplicados (opcional, melhor UX):**
```c
/* Não adicionar se igual ao último comando */
/* A readline não faz isto automaticamente (por defeito) */
/* Pode comparar com history_get(history_length) */
```

> O Subject da 42 exige apenas que o histórico funcione com ↑ e ↓. A persistência em ficheiro (`write_history`, `read_history`) **não é obrigatória** mas é um bom extra.

### 3.3 Funções de Controlo do Terminal readline

Estas funções são essenciais para a integração com o handler de `SIGINT` (ver Módulo 08):

```c
#include <readline/readline.h>

void rl_on_new_line(void);
/* Informar a readline que o cursor está agora no início de uma nova linha */
/* Necessário após escrever "\n" num signal handler */

void rl_replace_line(const char *text, int clear_undo);
/* Substituir o conteúdo atual do buffer de edição da readline */
/* rl_replace_line("", 0) → limpar a linha atual */
/* clear_undo: 0 = não limpar o undo buffer */

void rl_redisplay(void);
/* Forçar a readline a redesenhar o prompt e a linha atual */
/* Necessário após alterar o buffer para que o utilizador veja as mudanças */
```

**Sequência completa para SIGINT:**
```c
/* No handler de SIGINT: */
write(STDOUT_FILENO, "\n", 1); /* 1. nova linha no terminal */
rl_on_new_line();               /* 2. avisar readline */
rl_replace_line("", 0);        /* 3. limpar o buffer de input */
rl_redisplay();                 /* 4. reexibir o prompt */
```

### 3.4 Terminal e `tcsetattr`/`tcgetattr`

O terminal (TTY) tem um conjunto de atributos que controlam o seu comportamento: processamento de input, output, sinais, timeouts, etc. Estas configurações são manipuladas com:

```c
#include <termios.h>

int tcgetattr(int fd, struct termios *termios_p);
/* Obter os atributos actuais do terminal fd */
/* fd: tipicamente STDIN_FILENO (0) */

int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
/* Definir os atributos do terminal */
/* optional_actions: TCSANOW (imediato), TCSADRAIN (após flush), TCSAFLUSH */
```

**Campos importantes da `struct termios`:**
```c
struct termios {
    tcflag_t c_iflag;  /* input modes: ICRNL, IXON, etc. */
    tcflag_t c_oflag;  /* output modes: OPOST, etc. */
    tcflag_t c_cflag;  /* control modes: baud rate, etc. */
    tcflag_t c_lflag;  /* local modes: ECHO, ICANON, ISIG, etc. */
    cc_t     c_cc[NCCS]; /* special characters: VEOF (Ctrl-D), VINTR (Ctrl-C), etc. */
};
```

**Guardar e restaurar o estado do terminal:**
```c
struct termios  original_termios;

/* Guardar no início do programa */
tcgetattr(STDIN_FILENO, &original_termios);

/* Restaurar na saída */
tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
```

> A readline já gere os atributos do terminal internamente. Para o Minishell básico da 42, geralmente não é necessário usar `tcsetattr` diretamente — a readline trata disso. No entanto, é importante compreender o conceito para saber o que acontece quando a readline chama estas funções.

### 3.5 Modo Canónico vs. Modo Raw

**Modo canónico (padrão):**
- O kernel processa o input linha a linha
- Backspace e delete apagam caracteres no buffer do kernel
- O programa só recebe o input após o utilizador pressionar Enter
- Sinais (Ctrl-C, Ctrl-Z) são gerados automaticamente

**Modo raw:**
- Cada caractere é enviado imediatamente ao programa
- O programa tem controlo total sobre o processamento
- Necessário para editores como `vim`, e para a própria readline

A readline comuta para um modo semi-raw durante a sua operação e restaura o modo canónico quando retorna.

### 3.6 `isatty`, `ttyname`, `ttyslot`

```c
#include <unistd.h>

int   isatty(int fd);
/* Retorna 1 se fd é um terminal (TTY), 0 caso contrário */
/* Útil para detectar se o input vem do teclado ou de um pipe */

char *ttyname(int fd);
/* Retorna o nome do terminal (ex: "/dev/pts/0") */
/* Retorna NULL se fd não é um TTY */

int   ttyslot(void);
/* Retorna o índice do TTY atual na tabela de terminais */
/* Raramente necessário no Minishell */
```

**Uso de `isatty` no Minishell:**
```c
/* Verificar se o Minishell está a ser executado de forma interativa */
if (!isatty(STDIN_FILENO))
{
    /* Input não-interativo (ex: redirecionado de um ficheiro) */
    /* Pode não exibir o prompt */
}
```

### 3.7 Termcaps — `tgetent`, `tgetstr`, `tgoto`, `tputs`

O sistema **termcap** (terminal capabilities) é uma base de dados de capacidades dos terminais — as sequências de escape para mover o cursor, limpar o ecran, mudar de cor, etc.

```c
#include <curses.h>  /* ou <term.h> */

int   tgetent(char *bp, const char *name);
/* Carregar a entrada para o terminal 'name' (ex: "xterm") */
/* bp: buffer de 1024 bytes; name: tipicamente getenv("TERM") */
/* Retorna 1 se encontrado, 0 se não encontrado, -1 se erro */

char *tgetstr(const char *id, char **area);
/* Obter a sequência de escape para a capacidade 'id' */
/* Ex: tgetstr("cl", &area) → clear screen */
/* Retorna NULL se a capacidade não existe */

char *tgoto(const char *cap, int col, int row);
/* Processar uma capacidade com parâmetros (ex: mover cursor para linha/col) */

int   tputs(const char *str, int affcnt, int (*putc)(int));
/* Enviar uma string de capacidade para o terminal */
/* affcnt: número de linhas afetadas (geralmente 1) */
/* putc: função para output de cada caractere */
```

> **Nota para o Minishell da 42:** As funções termcap estão listadas como permitidas, mas para o projeto **básico** raramente são necessárias. A readline já usa termcap internamente. Só são necessárias em bonus (como implementar um mini-`vi` para edição de linha, que a readline já fornece).

### 3.8 Limitações e Regras — Contexto 42 / Norminette

**Memory leaks da readline:**

Um dos erros mais comuns é não libertar a memória alocada pela readline:

```c
/* ERRADO — memory leak */
line = readline("minishell$ ");
process(line);
/* free(line) esquecido! */

/* CORRETO */
line = readline("minishell$ ");
process(line);
free(line);       /* SEMPRE libertar */
```

**Valgrind e readline:**

A própria biblioteca readline tem alguns leaks "benignos" (still reachable) na sua inicialização interna. Para distinguir dos leaks do teu código:

```bash
valgrind --leak-check=full \
         --suppressions=readline.supp \  # suprimir leaks do readline
         ./minishell
```

O evaluador da 42 geralmente usa um ficheiro de suppressions para os leaks internos do readline. Verifica com o teu evaluator.

**`rl_clear_history` na saída:**
```c
void    clean_and_exit(t_msh *msh, int exit_code)
{
    rl_clear_history();     /* limpar histórico (liberta memória) */
    free_msh(msh);          /* libertar todas as estruturas do shell */
    exit(exit_code);
}
```

### 3.9 Funções Permitidas — Visão Geral

```c
/* Readline core */
#include <readline/readline.h>
char *readline(const char *prompt);
void  rl_on_new_line(void);
void  rl_replace_line(const char *text, int clear_undo);
void  rl_redisplay(void);

/* Histórico */
#include <readline/history.h>
void  add_history(const char *string);
void  rl_clear_history(void);

/* Terminal */
#include <termios.h>
int   tcgetattr(int fd, struct termios *termios_p);
int   tcsetattr(int fd, int optional_actions, const struct termios *termios_p);

/* TTY */
#include <unistd.h>
int   isatty(int fd);
char *ttyname(int fd);
int   ttyslot(void);

/* I/O */
#include <sys/ioctl.h>
int   ioctl(int fd, unsigned long request, ...);
/* Controlo de I/O de baixo nível — não necessário em Minishell básico */

/* Termcap */
int   tgetent(char *bp, const char *name);
int   tgetflag(const char *id);
int   tgetnum(const char *id);
char *tgetstr(const char *id, char **area);
char *tgoto(const char *cap, int col, int row);
int   tputs(const char *str, int affcnt, int (*putc)(int));
```

### 3.10 Metodologia de Aplicação (Pseudocódigo)

```
INICIALIZAÇÃO:
    // Guardar estado do terminal (para restaurar na saída)
    tcgetattr(STDIN_FILENO, &original_termios)
    
    // Inicializar ambiente e estruturas
    msh.env = ft_dup_envp(envp)
    msh.exit_status = 0
    g_signal = 0
    
    // Configurar sinais para o modo interativo
    setup_signals_parent()


LOOP PRINCIPAL:
    ENQUANTO verdade:
        g_signal = 0
        
        // Ler input
        line = readline(PROMPT)
        
        // Ctrl-D → sair
        SE line == NULL:
            write(STDOUT, "exit\n", 5)
            clean_and_exit(&msh, msh.exit_status)
        
        // Ctrl-C recebido durante readline
        SE g_signal == SIGINT:
            msh.exit_status = 130
            free(line)
            CONTINUAR
        
        // Linha vazia
        SE *line == '\0':
            free(line)
            CONTINUAR
        
        // Processar linha
        add_history(line)
        tokens = tokenize(line)
        free(line)
        SE tokens == NULL: CONTINUAR
        
        ast = parse(tokens)
        free_token_list(tokens)
        SE ast == NULL: CONTINUAR
        
        expand_ast(ast, &msh)
        execute_ast(ast, &msh)
        free_ast(ast)
        
        // Restaurar sinais para modo interativo (podem ter sido alterados)
        setup_signals_parent()


SAÍDA:
    rl_clear_history()
    free_msh(&msh)
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios)
    exit(msh.exit_status)
```

### 3.11 Metodologia de Testes

```bash
# Teste 1 — Histórico com setas
# Executar vários comandos, depois usar ↑ e ↓ para navegar
echo teste1
ls /tmp
pwd
# Pressionar ↑ três vezes → deve mostrar "echo teste1"

# Teste 2 — Edição de linha
# Usar Ctrl-A (início), Ctrl-E (fim), setas ← →, Backspace
echo hello world   # usar Ctrl-A para ir ao início, editar

# Teste 3 — readline liberta memória
valgrind --leak-check=full ./minishell
# Executar alguns comandos, sair com Ctrl-D
# Verificar que não há leaks (além dos "still reachable" do readline)

# Teste 4 — Ctrl-D
# O shell deve mostrar "exit" e terminar limpa
# Verificar com echo $? no shell pai

# Teste 5 — Linha vazia (só Enter)
# Deve simplesmente re-exibir o prompt sem erro

# Teste 6 — Prompt não afetado por output de processos filhos
# Garantir que o prompt aparece sempre na linha correta
echo -n "sem newline"; echo " com continucao"

# Teste 7 — Histórico não guarda duplicados (opcional)
echo test
echo test
# Pressionar ↑ uma vez deve mostrar "echo test" (não duas vezes)

# Teste 8 — Comportamento não-interativo
echo "ls" | ./minishell    # deve executar ls sem prompt
```

---

## 4. Conclusão

### 4.1 Síntese

A biblioteca GNU Readline transforma o Minishell de um simples leitor de comandos num shell interativo profissional. A sua integração correcta — gestão do histórico, sincronização com sinais, libertação de memória — é o que dá ao projeto a polished UX que os avaliadores esperam.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **`free(line)` esquecido** | Cada `readline()` aloca; deve ser libertado | `free(line)` em **todos** os caminhos de código |
| **`rl_clear_history` não chamado** | O histórico interno da readline fica em memória | Chamar `rl_clear_history()` antes de `exit()` |
| **Prompt "sujo" após Ctrl-C** | A linha de input fica no terminal | Sequência: `\n` + `rl_on_new_line` + `rl_replace_line("", 0)` + `rl_redisplay` |
| **readline com input não-interativo** | Quando stdin é um pipe, readline comporta-se diferente | Verificar `isatty(STDIN_FILENO)` — pode usar `get_next_line` em modo não-interativo |
| **Leaks do readline em Valgrind** | A própria readline tem "still reachable" blocks | Usar ficheiro de suppressions; ou aceitar os leaks "still reachable" distintos dos "definitely lost" |
| **Histórico de linhas vazias** | `add_history("")` adiciona entrada vazia | Verificar `if (line && *line)` antes de `add_history` |

### 4.3 Síntese Final do Projeto Minishell

Esta é a última de 10 fichas de estudo para o Minishell da 42. Os 10 módulos formam um pipeline de conhecimento:

```
Input  →  [LEXER]  →  [PARSER]  →  [EXPANSÃO]  →  [EXECUTOR]
           (M02)       (M03)         (M04)       (M05+M06+M07)

Suporte transversal:
  [SINAIS] (M08) + [BUILTINS] (M09) + [READLINE] (M10)

Fundação:
  [ARQUITETURA] (M01)
```

A implementação bem-sucedida do Minishell requer que todos estes módulos funcionem em conjunto, com código limpo (Norminette), sem memory leaks (Valgrind), e com o comportamento correto em todos os edge cases que o Subject e os avaliadores irão testar.

---

## 5. Bibliografia

### Documentação Oficial

- `man 3 readline`, `man 3 add_history`, `man 3 history`
- `man 3 tgetent`, `man 3 tputs`
- `man 3 tcgetattr`, `man 3 tcsetattr`
- `man 2 isatty`, `man 3 ttyname`
- [GNU Readline Library — Official Documentation](https://tiswww.case.edu/php/chet/readline/rltop.html)
- [GNU Readline Library — Programmer's Manual](https://tiswww.case.edu/php/chet/readline/readline.html)

### Livros Clássicos

- **W. Richard Stevens, Stephen A. Rago** — *Advanced Programming in the UNIX Environment*, 3rd Ed.  
  Capítulo 18: Terminal I/O — termios, tcgetattr, tcsetattr, raw mode

### Recursos Web

- **GNU Readline Manual** — Programming with GNU Readline  
  https://www.gnu.org/software/readline/manual/readline.html
- **The Linux Documentation Project** — Serial Programming Guide — Terminal I/O  
  https://tldp.org/HOWTO/Serial-Programming-HOWTO/

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
