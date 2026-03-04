# 08 — Sinais (Signals)

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que são Sinais?](#21-o-que-são-sinais)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que este módulo resolve](#24-problema-que-este-módulo-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 Conceitos Fundamentais de Sinais POSIX](#31-conceitos-fundamentais-de-sinais-posix)
  - [3.2 Sinais Relevantes para o Minishell](#32-sinais-relevantes-para-o-minishell)
  - [3.3 `signal()` vs `sigaction()`](#33-signal-vs-sigaction)
  - [3.4 Comportamento Esperado dos Sinais no Shell](#34-comportamento-esperado-dos-sinais-no-shell)
  - [3.5 A Variável Global e o Sinal](#35-a-variável-global-e-o-sinal)
  - [3.6 Limitações e Regras — Contexto 42 / Norminette](#36-limitações-e-regras--contexto-42--norminette)
  - [3.7 Funções Permitidas Relevantes](#37-funções-permitidas-relevantes)
  - [3.8 Metodologia de Aplicação (Pseudocódigo)](#38-metodologia-de-aplicação-pseudocódigo)
  - [3.9 Metodologia de Testes](#39-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que são Sinais?

Um **sinal** é uma notificação assíncrona enviada a um processo para informá-lo de um evento. São a forma mais simples de comunicação inter-processo (IPC) no UNIX. Quando um processo recebe um sinal, pode:
1. **Ignorá-lo** (`SIG_IGN`)
2. **Usar o handler padrão** (`SIG_DFL`) — tipicamente terminar ou parar o processo
3. **Capturá-lo** com uma função handler definida pelo utilizador

Exemplos de situações que geram sinais:
- Utilizador pressiona **Ctrl-C** → `SIGINT` (interrupt)
- Utilizador pressiona **Ctrl-\\** → `SIGQUIT` (quit com core dump)
- Utilizador pressiona **Ctrl-Z** → `SIGTSTP` (suspend/stop)
- Processo acede a memória inválida → `SIGSEGV`
- Processo escreve num pipe sem leitores → `SIGPIPE`

### 2.2 Contexto Histórico e Computacional

Os sinais foram introduzidos no **UNIX V4** (1973). A API original (`signal()`) tinha problemas de portabilidade e reentrância. O POSIX.1 introduziu `sigaction()` (1988) com semântica mais robusta e controlável.

O modelo de sinais POSIX define que um sinal é entregue ao processo **de forma assíncrona**, podendo interromper qualquer syscall. Esta característica torna a programação com sinais delicada — os handlers devem ser reentrants (async-signal-safe).

### 2.3 Relevância no Projeto Minishell da 42

O Subject da 42 exige comportamento específico para Ctrl-C, Ctrl-D, e Ctrl-\\ em dois contextos distintos:

| Contexto | Ctrl-C | Ctrl-D | Ctrl-\\ |
|----------|--------|--------|---------|
| **Shell interativo** (prompt ativo) | Nova linha com novo prompt | Sair do shell | Nada/ignorar |
| **Processo filho em execução** | Terminar o processo (default) | EOF para o processo | Core dump (default) |

### 2.4 Problema que este módulo resolve

O problema é que o **shell pai** e os **processos filhos** devem ter comportamentos distintos ao receber sinais. O pai não pode ser terminado pelo Ctrl-C; o filho sim. Também é necessário que o estado do terminal (readline) seja atualizado quando um sinal interrompe a leitura.

---

## 3. Desenvolvimento

### 3.1 Conceitos Fundamentais de Sinais POSIX

**Enviar um sinal:**
```c
#include <signal.h>
int kill(pid_t pid, int sig);
/* pid > 0: enviar ao processo pid */
/* pid = 0: enviar a todos no mesmo process group */
/* pid = -1: enviar a todos os processos que o utilizador pode sinalizar */
```

**Registar um handler:**
```c
/* API legada (signal) */
void (*signal(int signum, void (*handler)(int)))(int);

/* API POSIX robusta (sigaction) */
int sigaction(int signum, const struct sigaction *act,
              struct sigaction *oldact);
```

**Tipos de handlers:**
- `SIG_DFL` — comportamento padrão do kernel
- `SIG_IGN` — ignorar o sinal
- `my_handler` — função definida pelo utilizador

### 3.2 Sinais Relevantes para o Minishell

| Sinal | Número (Linux) | Default | Causado por | Minishell |
|-------|----------------|---------|-------------|-----------|
| `SIGINT` | 2 | Terminar | Ctrl-C | Capturar no pai |
| `SIGQUIT` | 3 | Terminar + core dump | Ctrl-\\ | Ignorar no pai |
| `SIGTERM` | 15 | Terminar | `kill pid` | Pode ignorar ou default |
| `SIGPIPE` | 13 | Terminar | Escrita em pipe fechado | Ignorar (nos filhos) |
| `SIGHUP` | 1 | Terminar | Terminal fechado | Default |
| `SIGTSTP` | 20 | Parar (stop) | Ctrl-Z | Ignorar no pai (Subject) |

### 3.3 `signal()` vs `sigaction()`

**`signal()`** (simples mas problemático em alguns sistemas):
```c
signal(SIGINT, my_sigint_handler);
signal(SIGQUIT, SIG_IGN);
```
- Comportamento não especificado: em alguns sistemas, o handler é reset para `SIG_DFL` após ser chamado uma vez (POSIX permite este comportamento)
- Não permite controlar se syscalls são reiniciadas após interrupção

**`sigaction()`** (recomendado):
```c
struct sigaction sa;

sa.sa_handler = my_sigint_handler;
sigemptyset(&sa.sa_mask);          /* não bloquear outros sinais durante handler */
sa.sa_flags = SA_RESTART;          /* reiniciar syscalls interrompidas automaticamente */
sigaction(SIGINT, &sa, NULL);
```
- `SA_RESTART`: syscalls como `read()` são automaticamente reiniciadas se interrompidas pelo sinal
- `SA_SIGINFO`: handler recebe informação adicional (`siginfo_t`)
- `sa_mask`: sinais a bloquear enquanto o handler está a executar

### 3.4 Comportamento Esperado dos Sinais no Shell

#### No processo pai (shell interativo — aguardando input):

**Ctrl-C (`SIGINT`):**
```
Antes:   minishell$ ls -la_incomple|
Depois:  (nova linha)
         minishell$
```
- Cancelar a linha atual de input
- Exibir nova linha
- Reexibir o prompt
- `$?` deve ser `130` (128 + 2)

Implementação:
```c
void    sigint_handler(int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();        /* informar readline que está numa nova linha */
    rl_replace_line("", 0); /* limpar o buffer do readline */
    rl_redisplay();          /* reexibir o prompt vazio */
    g_exit_status = 130;    /* atualizar $? */
}
```

**Ctrl-\\ (`SIGQUIT`):**
```
Comportamento esperado: nada (ignorar)
```
```c
signal(SIGQUIT, SIG_IGN);
```

**Ctrl-D:**
Ctrl-D não é um sinal — é um EOF enviado ao terminal. O `readline()` retorna `NULL` quando recebe EOF. O shell deve interpretar isto como "sair":
```c
line = readline(PROMPT);
if (!line)   /* Ctrl-D */
{
    write(STDOUT_FILENO, "exit\n", 5);
    clean_exit(&msh, msh.exit_status);
}
```

#### No processo filho (comando externo a executar):

Antes de `execve`, os filhos devem **restaurar os handlers para SIG_DFL**:
```c
/* No filho, antes do execve: */
signal(SIGINT,  SIG_DFL);   /* Ctrl-C deve terminar o filho normalmente */
signal(SIGQUIT, SIG_DFL);   /* Ctrl-\ deve terminar com core dump (padrão) */
```

Desta forma, se o utilizador pressionar Ctrl-C enquanto `sleep 10` está a executar, apenas o `sleep` é terminado; o shell pai captura o `SIGINT`, exibe nova linha, e volta ao prompt.

#### Durante heredoc:

O heredoc usa `readline()` para ler input linha a linha. Ctrl-C durante o heredoc deve:
- Cancelar o heredoc
- Mostrar nova linha
- Não guardar o heredoc no histórico

### 3.5 A Variável Global e o Sinal

O Subject da 42 permite **no máximo uma variável global**. Esta variável é tipicamente usada para comunicar entre o signal handler e o loop principal.

```c
/* No topo do programa — única variável global permitida */
int g_signal;   /* ou g_exit_status, ou g_received_signal */
```

O handler define esta variável; o loop principal lê-a:
```c
void    sigint_handler(int sig)
{
    g_signal = sig;   /* SIGINT = 2 */
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
```

> **Atenção — async-signal-safety:** Dentro de um signal handler, **apenas** funções async-signal-safe podem ser chamadas. `malloc`, `printf`, `free` NÃO são async-signal-safe. `write()` é. `rl_on_new_line()`, `rl_replace_line()`, `rl_redisplay()` são chamadas de readline e podem não ser tecnicamente async-signal-safe, mas são usadas por convenção nos projetos 42.

### 3.6 Limitações e Regras — Contexto 42 / Norminette

**Regras do Subject:**
- Exatamente **1 variável global** para sinalização de sinais (se necessário)
- Ctrl-C no prompt: nova linha + novo prompt (não sair)
- Ctrl-D no prompt: sair do shell (como `exit`)
- Ctrl-\\ no prompt: nada (ignorar)
- Ctrl-C num processo filho: terminar o filho, não o shell

**Norminette:**
- Os handlers de sinais são funções simples — fáceis de manter ≤ 25 linhas
- A função de setup de sinais (`setup_signals()`) deve ser chamada no início do loop de main e antes dos exec nos filhos

### 3.7 Funções Permitidas Relevantes

```c
#include <signal.h>
int  sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
void (*signal(int signum, void (*handler)(int)))(int);
int  kill(pid_t pid, int sig);
int  sigemptyset(sigset_t *set);       /* inicializar conjunto vazio de sinais */
int  sigaddset(sigset_t *set, int sig);/* adicionar sinal ao conjunto */

/* Funções do readline para uso no handler: */
void rl_on_new_line(void);
void rl_replace_line(const char *text, int clear_undo);
void rl_redisplay(void);
```

### 3.8 Metodologia de Aplicação (Pseudocódigo)

```
FUNÇÃO setup_signals_parent():
    // Configurar sinais para o processo pai (shell interativo)
    sigaction(SIGINT, {handler=sigint_handler, flags=SA_RESTART}, NULL)
    sigaction(SIGQUIT, {handler=SIG_IGN}, NULL)


FUNÇÃO sigint_handler(sig: int):
    g_signal = sig
    write(STDOUT, "\n", 1)
    rl_on_new_line()
    rl_replace_line("", 0)
    rl_redisplay()


FUNÇÃO setup_signals_child():
    // No filho, restaurar comportamentos padrão
    signal(SIGINT, SIG_DFL)
    signal(SIGQUIT, SIG_DFL)


FUNÇÃO setup_signals_heredoc():
    // Durante a leitura do heredoc (Ctrl-C deve cancelar)
    sigaction(SIGINT, {handler=sigint_heredoc_handler, flags=0}, NULL)


LOOP PRINCIPAL:
    setup_signals_parent()
    ENQUANTO verdade:
        g_signal = 0
        line = readline(PROMPT)
        SE line == NULL:
            write("exit\n")
            SAIR
        SE g_signal == SIGINT:
            msh.exit_status = 130
            free(line)
            CONTINUAR
        SE line não é vazio:
            add_history(line)
            processar_e_executar(line, &msh)
        free(line)
```

### 3.9 Metodologia de Testes

```bash
# Teste 1 — Ctrl-C no prompt
# Esperado: nova linha + novo prompt, $? = 130
minishell$ ^C
minishell$ echo $?   # 130

# Teste 2 — Ctrl-\ no prompt
# Esperado: NADA (ignorar)
minishell$ ^\
minishell$            # prompt reaparece sem mensagem

# Teste 3 — Ctrl-D no prompt
# Esperado: "exit" impresso e shell termina
minishell$ ^D
exit
$

# Teste 4 — Ctrl-C com processo filho em execução
minishell$ sleep 10
^C                    # sleep termina, shell volta ao prompt
minishell$ echo $?    # 130

# Teste 5 — Ctrl-\ com processo filho (pode gerar "Quit (core dumped)")
minishell$ sleep 10
^\                    # Quit (core dumped) — comportamento padrão do filho

# Teste 6 — Ctrl-C durante heredoc
minishell$ cat << EOF
> linha 1
> ^C                  # deve cancelar o heredoc, nova linha, novo prompt

# Teste 7 — Múltiplos Ctrl-C sequenciais
# Cada Ctrl-C deve mostrar novo prompt sem acumular comportamentos
```

---

## 4. Conclusão

### 4.1 Síntese

A gestão de sinais no Minishell requer dois perfis distintos: o pai captura `SIGINT` para não ser terminado e atualiza o terminal; os filhos restauram os defaults para serem terminados normalmente. A única variável global permitida serve como canal de comunicação entre o handler e o loop principal.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Shell termina com Ctrl-C** | O pai não captura SIGINT | Registar handler para SIGINT no pai |
| **Ctrl-\\ mostra "Quit"** | SIGQUIT não é ignorado no pai | `signal(SIGQUIT, SIG_IGN)` no pai |
| **Filho ignora Ctrl-C** | O filho herdou o handler do pai | Restaurar `SIG_DFL` no filho antes do execve |
| **Handler chama `printf`** | `printf` não é async-signal-safe (pode deadlock) | Usar apenas `write()` no handler |
| **readline não atualiza após SIGINT** | Linha do readline fica "suja" | Chamar `rl_on_new_line()` + `rl_replace_line("", 0)` + `rl_redisplay()` |
| **$? não atualizado após Ctrl-C** | `$?` mantém valor anterior | Atualizar `g_signal` no handler e usar no loop principal |

---

## 5. Bibliografia

### Documentação Oficial

- `man 2 signal`, `man 2 sigaction`, `man 2 kill`, `man 7 signal`
- [POSIX.1-2017 — §signal.h](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/signal.h.html)
- [POSIX.1-2017 — §sigaction](https://pubs.opengroup.org/onlinepubs/9699919799/functions/sigaction.html)

### Livros Clássicos

- **W. Richard Stevens, Stephen A. Rago** — *Advanced Programming in the UNIX Environment*, 3rd Ed.  
  Capítulo 10: Signals — señalización completa, sigaction, signal masks

### Recursos Web

- **GNU Bash Manual** — §3.7.6 Signals  
  https://www.gnu.org/software/bash/manual/bash.html#Signals
- **Linux man-pages** — signal-safety(7)  
  https://man7.org/linux/man-pages/man7/signal-safety.7.html

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
