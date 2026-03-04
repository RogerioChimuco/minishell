# 06 — Pipes e Descritores de Ficheiro

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que são Pipes e File Descriptors?](#21-o-que-são-pipes-e-file-descriptors)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que este módulo resolve](#24-problema-que-este-módulo-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 File Descriptors — O Modelo UNIX de I/O](#31-file-descriptors--o-modelo-unix-de-io)
  - [3.2 `pipe()` — Criar um Canal de Comunicação](#32-pipe--criar-um-canal-de-comunicação)
  - [3.3 `dup2()` — Redirecionar Descritores](#33-dup2--redirecionar-descritores)
  - [3.4 Pipeline de N Comandos](#34-pipeline-de-n-comandos)
  - [3.5 Fechar os FDs Corretamente](#35-fechar-os-fds-corretamente)
  - [3.6 Limitações e Regras — Contexto 42 / Norminette](#36-limitações-e-regras--contexto-42--norminette)
  - [3.7 Funções Permitidas Relevantes](#37-funções-permitidas-relevantes)
  - [3.8 Metodologia de Aplicação (Pseudocódigo)](#38-metodologia-de-aplicação-pseudocódigo)
  - [3.9 Metodologia de Testes](#39-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que são Pipes e File Descriptors?

Um **File Descriptor (FD)** é um número inteiro não-negativo que identifica um ficheiro (ou outra fonte/destino de I/O) aberto por um processo. O kernel mantém uma tabela de FDs por processo:

| FD | Nome | Descrição |
|----|------|-----------|
| `0` | `STDIN_FILENO` | Standard Input — leitura do teclado |
| `1` | `STDOUT_FILENO` | Standard Output — escrita no terminal |
| `2` | `STDERR_FILENO` | Standard Error — mensagens de erro |
| `3+` | FDs de utilizador | Ficheiros abertos, pipes, sockets... |

Um **pipe** é um canal de comunicação unidirecional entre processos. O output de um processo vai diretamente para o input de outro, sem ficheiros intermédios.

```bash
ls -la | grep ".c" | wc -l
```

Neste exemplo, `ls` escreve no pipe 1, `grep` lê do pipe 1 e escreve no pipe 2, `wc` lê do pipe 2.

### 2.2 Contexto Histórico e Computacional

O pipe foi inventado por **Douglas McIlroy** (Bell Labs, 1964) e implementado por Ken Thompson no **UNIX V3** (1973). É uma das inovações mais influentes da arquitectura UNIX: a filosofia "Make each program do one thing well, and pipe them together" é possível graças aos pipes.

A implementação no kernel usa um **buffer circular** (tipicamente 64KB no Linux) gerido pelo kernel. `write()` bloqueia quando o buffer está cheio; `read()` bloqueia quando está vazio.

### 2.3 Relevância no Projeto Minishell da 42

Pipelines são um requisito **obrigatório** do Subject da 42. Além disso, compreender FDs é essencial para implementar redirecionamentos (`<`, `>`, `>>`). Os pipes testam simultaneamente:
- Conhecimento de `pipe`, `fork`, `dup2`, `close`
- Gestão correta de FDs (fechar todos os FDs quando necessário)
- Sincronização de processos (waitpid de múltiplos filhos)

O erro mais comum nos projetos da 42 é **não fechar** os FDs de escrita do pipe no processo pai, o que faz com que o processo filho (`wc`, `cat -n`, etc.) nunca receba EOF e fique bloqueado forever.

### 2.4 Problema que este módulo resolve

O problema é ligar o **stdout** de um processo ao **stdin** do seguinte sem ficheiros temporários. O pipe resolve isto: é um buffer gerido pelo kernel que funciona como uma fila FIFO de bytes entre dois FDs.

---

## 3. Desenvolvimento

### 3.1 File Descriptors — O Modelo UNIX de I/O

Cada processo tem uma **tabela de FDs** que são índices para uma tabela global do kernel de ficheiros abertos. Quando se faz `fork()`, o filho herda uma **cópia** desta tabela — os mesmos FDs apontam para os mesmos ficheiros/pipes subjacentes.

```
PROCESSO PAI            PROCESSO FILHO (após fork)
FD[0] → stdin           FD[0] → stdin  (mesmo stdin)
FD[1] → stdout          FD[1] → stdout (mesmo stdout)
FD[2] → stderr          FD[2] → stderr
FD[3] → pipe_read       FD[3] → pipe_read
FD[4] → pipe_write      FD[4] → pipe_write
```

As operações fundamentais sobre FDs:
```c
int  open(const char *path, int flags, mode_t mode);  /* abrir/criar */
int  close(int fd);                                    /* fechar */
int  dup(int oldfd);                                   /* duplicar para menor FD disponível */
int  dup2(int oldfd, int newfd);                       /* duplicar para FD específico */
int  pipe(int pipefd[2]);                              /* criar pipe */
```

### 3.2 `pipe()` — Criar um Canal de Comunicação

```c
#include <unistd.h>
int pipe(int pipefd[2]);
```

- `pipefd[0]` — extremidade de **leitura** (read end)
- `pipefd[1]` — extremidade de **escrita** (write end)
- Retorna `0` em caso de sucesso, `-1` em caso de erro

```c
int fd[2];
if (pipe(fd) == -1)
    return (perror("pipe"), -1);
/* fd[0] = read end, fd[1] = write end */
```

**Comportamento:**
- `read(fd[0], buf, size)` bloqueia se o pipe está vazio, retorna quando há dados ou quando **todos os write ends estão fechados** (retorna 0 = EOF)
- `write(fd[1], buf, size)` bloqueia se o pipe está cheio (64KB no Linux), retorna quando há espaço
- Se o write end for fechado e o read end tentar ler um pipe vazio → recebe EOF (0 bytes)
- Se o read end for fechado e alguém tenta escrever → recebe SIGPIPE

### 3.3 `dup2()` — Redirecionar Descritores

```c
#include <unistd.h>
int dup2(int oldfd, int newfd);
```

- Faz `newfd` apontar para o mesmo ficheiro/pipe que `oldfd`
- Se `newfd` já estava aberto, é fechado automaticamente antes
- Retorna `newfd` em caso de sucesso, `-1` em caso de erro

**Uso para redirecionar stdout para um pipe:**
```c
/* No processo filho que escreve no pipe: */
dup2(fd[1], STDOUT_FILENO);  /* stdout agora escreve no pipe */
close(fd[0]);                /* fechar o read end — não precisamos */
close(fd[1]);                /* fechar o original (só usamos o dup) */
/* A partir daqui, printf() escreve no pipe */
execve(...);
```

**Uso para redirecionar stdin a partir de um pipe:**
```c
/* No processo filho que lê do pipe: */
dup2(fd[0], STDIN_FILENO);   /* stdin agora lê do pipe */
close(fd[0]);                /* fechar o original */
close(fd[1]);                /* fechar o write end — não precisamos */
/* A partir daqui, fgets() e scanf() leem do pipe */
execve(...);
```

### 3.4 Pipeline de N Comandos

Para `cmd1 | cmd2 | cmd3`, precisamos de **N-1 pipes** para N comandos:

```
cmd1  →  pipe[0]  →  cmd2  →  pipe[1]  →  cmd3
```

Estrutura de dados:
```c
/*
 * Para uma pipeline de N comandos:
 * - N-1 pipes, cada um com fd[0] (read) e fd[1] (write)
 * - Armazenar como array: int pipes[(N-1) * 2]
 *   pipes[i*2 + 0] = read end do pipe i
 *   pipes[i*2 + 1] = write end do pipe i
 */
```

**Regras de ligação para o comando i:**
- Se `i > 0`: `dup2(pipes[(i-1)*2], STDIN_FILENO)` — ler do pipe anterior
- Se `i < N-1`: `dup2(pipes[i*2+1], STDOUT_FILENO)` — escrever no pipe seguinte
- **Fechar todos os FDs dos pipes** após os dup2 (tanto no filho como no pai)

### 3.5 Fechar os FDs Corretamente

Esta é a parte mais crítica e propensa a erros. **Regra de ouro:**

> Fechar **todos** os extremos de **todos** os pipes em **todos** os processos que não os estão a usar ativamente.

**Porquê?** O `read()` só retorna EOF quando **todos** os write ends do pipe estão fechados. Se o pai (ou outro filho) mantiver um write end aberto, o leitor nunca recebe EOF e fica bloqueado.

```
ESTADO ERRADO (pipe write end aberto no pai):
  pai: fd[1] ABERTO ←── bloqueia o cat porque cat espera EOF
  filho cmd1: fd[1] ABERTO → escreve no pipe → fecha
  filho cmd2: fd[0] ABERTO → lê do pipe → NEVER gets EOF → blocks forever

ESTADO CORRETO:
  pai: fecha fd[0] e fd[1] imediatamente após o fork
  filho cmd1: fecha fd[0], usa fd[1] como stdout, fecha fd[1] após dup2
  filho cmd2: fecha fd[1], usa fd[0] como stdin, fecha fd[0] após dup2
```

**Função auxiliar — fechar todos os pipes:**
```c
void    close_all_pipes(int *pipes, int n_pipes)
{
    int i;

    i = 0;
    while (i < n_pipes * 2)
    {
        close(pipes[i]);
        i++;
    }
}
```

### 3.6 Limitações e Regras — Contexto 42 / Norminette

**Pontos críticos testados pelos avaliadores:**

| Teste | Descrição |
|-------|-----------|
| `ls \| cat \| wc -l` | Pipeline básico de 3 comandos |
| `cat ficheiro \| cat \| cat \| cat` | Pipeline longo (verificar deadlock) |
| `echo hello \| cat -n` | Stdout de echo vai para stdin de cat |
| `ls \| grep .c > output.txt` | Pipe + redirecionamento |
| `cat /dev/urandom \| head -n 5` | Pipe com processo que termina antes do produtor |

**Norminette:**
- A função de setup de pipes pode ser longa — dividir em:
  - `init_pipes(cmd_table)` — cria todos os pipes
  - `child_setup_fds(i, n, pipes)` — faz os dup2 para o filho i
  - `close_all_pipes(pipes, n)` — fecha após o fork
- Guardar os PIDs de todos os filhos para o waitpid

### 3.7 Funções Permitidas Relevantes

```c
#include <unistd.h>
int  pipe(int pipefd[2]);
int  dup(int oldfd);
int  dup2(int oldfd, int newfd);
int  close(int fd);
int  read(int fd, void *buf, size_t count);
int  write(int fd, const void *buf, size_t count);
int  open(const char *pathname, int flags, ...);
```

### 3.8 Metodologia de Aplicação (Pseudocódigo)

```
FUNÇÃO execute_pipeline(cmd_table: t_cmd_table, msh: t_msh):
    n = número de comandos na cmd_table
    SE n == 1:
        execute_single_command(cmd_table.cmds[0], msh)
        RETORNAR
    
    // Criar N-1 pipes
    pipes = alocar array de (n-1) * 2 inteiros
    PARA i DE 0 ATÉ n-2:
        SE pipe(&pipes[i*2]) == -1:
            perror("pipe")
            LIBERTAR pipes
            RETORNAR
    
    pids = alocar array de n PIDs
    PARA i DE 0 ATÉ n-1:
        pids[i] = fork()
        SE pids[i] == -1:
            perror("fork")
            BREAK
        SE pids[i] == 0:  // FILHO i
            // Configurar stdin
            SE i > 0:
                dup2(pipes[(i-1)*2], STDIN_FILENO)   // ler do pipe anterior
            // Configurar stdout
            SE i < n-1:
                dup2(pipes[i*2+1], STDOUT_FILENO)    // escrever no próximo pipe
            // FECHAR TODOS OS PIPES NO FILHO
            PARA j DE 0 ATÉ (n-1)*2 - 1: close(pipes[j])
            // Aplicar outros redirecionamentos (<, >, >>, <<)
            apply_redirections(cmd_table.cmds[i].redirs)
            // Executar o comando
            execute_cmd_or_builtin(cmd_table.cmds[i], msh)
            exit(msh.exit_status)
    
    // PAI: FECHAR TODOS OS PIPES
    PARA j DE 0 ATÉ (n-1)*2 - 1: close(pipes[j])
    
    // Esperar por todos os filhos
    PARA i DE 0 ATÉ n-1:
        waitpid(pids[i], &wstatus, 0)
    update_exit_status(msh, último_wstatus)
    
    LIBERTAR pipes
    LIBERTAR pids
```

### 3.9 Metodologia de Testes

```bash
# Teste 1 — Pipeline simples
echo "hello" | cat             # deve mostrar "hello"

# Teste 2 — Pipeline com redirecionamento
ls /bin | grep "ls" | wc -l    # deve mostrar o número de ocorrências

# Teste 3 — Verificar que o shell não bloqueia
# (Indica que todos os pipes estão a ser fechados corretamente)
ls | cat | wc -l               # deve retornar imediatamente

# Teste 4 — Exit status do pipeline (deve ser o do último comando)
false | true
echo $?                        # deve ser 0 (true retornou 0)

# Teste 5 — Valgrind sem FD leaks
valgrind --track-fds=yes ./minishell
# Executar pipelines e verificar "FILE DESCRIPTORS: 0 open at exit"

# Teste 6 — cat com heredoc num pipe
cat << EOF | wc -l
linha 1
linha 2
linha 3
EOF
# deve mostrar 3
```

---

## 4. Conclusão

### 4.1 Síntese

Pipes e file descriptors são a mecânica que permite ao shell encadear processos de forma eficiente. A implementação correta exige disciplina na gestão de FDs: criar os N-1 pipes antes dos forks, configurar os dup2 nos filhos, e fechar rigorosamente todos os FDs desnecessários em todos os processos.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Write end aberto no pai** | O processo leitor nunca recebe EOF | Fechar TODOS os pipe FDs no pai após os forks |
| **Filho herda pipes de outros commands** | Descritores de pipes anteriores ficam abertos | Fechar todos os FDs de todos os pipes em cada filho |
| **dup2 sem close subsequente** | O FD original fica aberto além do dup | Sempre `close(oldfd)` após `dup2(oldfd, newfd)` (a menos que `oldfd == newfd`) |
| **Deadlock no pipe** | Dois processos cada um a ler de um pipe que o outro devia escrever (raro no minishell) | Garantir a ordem correta de leitura/escrita |
| **Waitpid apenas do último filho** | Os outros filhos tornam-se zombies | Fazer `waitpid` para todos os PIDs da pipeline |

---

## 5. Bibliografia

### Documentação Oficial

- `man 2 pipe`, `man 2 dup`, `man 2 dup2`, `man 2 close`, `man 2 open`
- [POSIX.1-2017 — §pipe](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pipe.html)

### Livros Clássicos

- **W. Richard Stevens** — *UNIX Network Programming, Volume 2: Interprocess Communications*  
  Capítulo 4: Pipes and FIFOs
- **W. Richard Stevens, Stephen A. Rago** — *Advanced Programming in the UNIX Environment*, 3rd Ed.  
  Capítulo 15: Interprocess Communication — Pipes

### Recursos Web

- **GNU Bash Manual** — §3.2.3 Pipelines  
  https://www.gnu.org/software/bash/manual/bash.html#Pipelines
- **Linux man-pages project** — pipe(7)  
  https://man7.org/linux/man-pages/man7/pipe.7.html

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
