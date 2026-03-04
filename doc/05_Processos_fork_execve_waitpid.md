# 05 — Processos: fork(), execve() e waitpid()

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O Modelo de Processos UNIX](#21-o-modelo-de-processos-unix)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que este módulo resolve](#24-problema-que-este-módulo-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 `fork()` — Duplicação de Processos](#31-fork--duplicação-de-processos)
  - [3.2 `execve()` — Substituição da Imagem do Processo](#32-execve--substituição-da-imagem-do-processo)
  - [3.3 `waitpid()` — Esperar pelo Filho](#33-waitpid--esperar-pelo-filho)
  - [3.4 Sequência Fork-Exec-Wait](#34-sequência-fork-exec-wait)
  - [3.5 Resolução de Caminhos e PATH](#35-resolução-de-caminhos-e-path)
  - [3.6 Processos Zombie e Processos Órfãos](#36-processos-zombie-e-processos-órfãos)
  - [3.7 Limitações e Regras — Contexto 42 / Norminette](#37-limitações-e-regras--contexto-42--norminette)
  - [3.8 Funções Permitidas Relevantes](#38-funções-permitidas-relevantes)
  - [3.9 Metodologia de Aplicação (Pseudocódigo)](#39-metodologia-de-aplicação-pseudocódigo)
  - [3.10 Metodologia de Testes](#310-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O Modelo de Processos UNIX

No UNIX, um **processo** é a unidade fundamental de execução — um programa em curso com o seu próprio espaço de endereçamento, descritores de ficheiro, e estado. O kernel organiza os processos numa hierarquia (árvore de parentesco) onde o `init` (PID 1) é o processo ancestral de todos.

O mecanismo clássico para executar um comando externo no UNIX baseia-se em três syscalls:
1. **`fork()`** — cria um processo filho que é uma cópia do pai
2. **`execve()`** — substitui a imagem do processo filho pelo programa desejado
3. **`waitpid()`** — faz o pai esperar pelo término do filho e recolhe o seu status

Este padrão **fork-exec-wait** é o coração de todo o shell UNIX.

### 2.2 Contexto Histórico e Computacional

O `fork()` foi concebido por Melvin Conway em 1963 e implementado no **UNIX V1** (1971) por Ken Thompson e Dennis Ritchie. A elegância da abordagem fork-exec é notável: `fork` é a única forma de criar novos processos no UNIX (sem recursos adicionais como posix_spawn), e a separação entre criação de processo e substituição de imagem permite configurar o filho antes de executar (redirect de I/O, setup de pipes, etc.).

`execve` (e a família `exec`) foi projetada para ser chamada **imediatamente após o fork**. O `waitpid` introduzido no UNIX V6 substituiu o `wait` original, adicionando a capacidade de esperar por um PID específico com opções de comportamento (non-blocking, etc.).

### 2.3 Relevância no Projeto Minishell da 42

Este é o módulo de **execução** — sem `fork`/`execve`, o Minishell não executa nenhum programa externo. O correto uso de:
- `fork()` garante que o shell pai sobrevive após executar um comando
- `execve()` garante que o programa correto (com ambiente correto) é carregado
- `waitpid()` garante que o exit status é capturado para `$?` e que não há zombies

Erros aqui causam: crash do shell ao executar comandos, valores incorretos de `$?`, memory leaks de processos, ou o shell ficar bloqueado permanentemente.

### 2.4 Problema que este módulo resolve

O shell não pode **substituir-se a si próprio** pelo comando que executa (isso seria um `exec` sem fork e destruiria o shell). Precisa de criar uma **cópia** de si mesmo (fork), deixar a cópia transformar-se no programa desejado (exec), e aguardar o resultado (wait). O pai continua vivo, lê o próximo comando, e o ciclo recomeça.

---

## 3. Desenvolvimento

### 3.1 `fork()` — Duplicação de Processos

```c
#include <unistd.h>
pid_t fork(void);
```

**O que `fork` faz:**
- Cria um processo filho que é uma **cópia quase exata** do pai:
  - Mesmo espaço de endereçamento (cópia COW — Copy-On-Write)
  - Mesmos descritores de ficheiro abertos
  - Mesmo código, mesma stack, mesmos dados
  - MAS: PID diferente, PPID diferente, `fork()` retorna valores diferentes

**Valores de retorno:**
```c
pid_t pid = fork();

if (pid == -1)
{
    /* ERRO — fork falhou (ex: limite de processos atingido) */
    perror("fork");
    exit(1);
}
else if (pid == 0)
{
    /* PROCESSO FILHO — executa aqui */
    /* pid == 0 identifica que somos o filho */
}
else
{
    /* PROCESSO PAI — pid é o PID do filho criado */
    /* executa aqui */
}
```

**Copy-On-Write (COW):**
O kernel não duplica imediatamente toda a memória. A cópia só é feita quando um dos processos (pai ou filho) tenta escrever numa página. Isto torna o `fork` eficiente mesmo para processos com muitas páginas de memória.

**Descritores de ficheiro após fork:**
Todos os FDs abertos no pai são **herdados** pelo filho. Esta propriedade é essencial para a implementação de pipes e redirecionamentos (ver Módulos 06/07): o filho herda os FDs alterados antes do `exec`.

### 3.2 `execve()` — Substituição da Imagem do Processo

```c
#include <unistd.h>
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

**O que `execve` faz:**
- **Substitui completamente** o espaço de endereçamento do processo atual pelo programa em `pathname`
- O código, dados, stack e heap do processo anterior são descartados
- O novo programa começa a executar desde o seu `main()`
- Os FDs abertos **são mantidos** (a não ser que tenham o flag `FD_CLOEXEC`)

**Parâmetros:**
- `pathname`: caminho absoluto ou relativo para o executável (ex: `/bin/ls`)
- `argv`: array de argumentos, terminado por `NULL`. `argv[0]` é o nome do programa
- `envp`: array de variáveis de ambiente (`"NOME=VALOR"`), terminado por `NULL`

**Retorno:**
- `execve` **nunca retorna** em caso de sucesso
- Retorna `-1` em caso de erro (ex: ficheiro não encontrado, sem permissão)

```c
/* Execução de "ls -la /tmp" */
char *argv[] = {"ls", "-la", "/tmp", NULL};
execve("/bin/ls", argv, envp);
/* Se chegou aqui, execve falhou */
perror("execve");
exit(127);  /* convenção: 127 = command not found */
```

**Família exec:**
Existem variantes como `execvp` (procura no PATH), `execl`, `execlp`, `execle`. O Subject da 42 **só permite `execve`** (a de nível mais baixo). A resolução do PATH deve ser feita manualmente.

### 3.3 `waitpid()` — Esperar pelo Filho

```c
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int *wstatus, int options);
```

**Parâmetros:**
- `pid`: PID do filho a aguardar; `-1` aguarda por qualquer filho; `0` aguarda por qualquer filho do mesmo process group
- `wstatus`: ponteiro para um `int` onde o status é armazenado — interpretar com macros
- `options`: `0` (bloqueante), `WNOHANG` (non-bloqueante), `WUNTRACED` (reportar stopped)

**Macros para interpretar `wstatus`:**
```c
/* Após waitpid(pid, &wstatus, 0): */

if (WIFEXITED(wstatus))
    printf("Saiu normalmente com código: %d\n", WEXITSTATUS(wstatus));
    /* WEXITSTATUS: os bits inferiores do exit code (0-255) */

if (WIFSIGNALED(wstatus))
    printf("Terminado por sinal: %d\n", WTERMSIG(wstatus));
    /* O exit_status do shell deve ser 128 + WTERMSIG(wstatus) */

if (WIFSTOPPED(wstatus))
    printf("Processo parado por sinal: %d\n", WSTOPSIG(wstatus));
    /* Só com WUNTRACED */
```

**Cálculo do `$?` após waitpid:**
```c
void    update_exit_status(t_msh *msh, int wstatus)
{
    if (WIFEXITED(wstatus))
        msh->exit_status = WEXITSTATUS(wstatus);
    else if (WIFSIGNALED(wstatus))
        msh->exit_status = 128 + WTERMSIG(wstatus);
}
```

### 3.4 Sequência Fork-Exec-Wait

A sequência completa para executar um comando simples sem pipeline:

```c
int     execute_external_cmd(t_cmd *cmd, t_msh *msh)
{
    pid_t   pid;
    int     wstatus;
    char   *path;
    char  **envp;

    path = resolve_path(cmd->argv[0], msh->env);  /* procura no PATH */
    if (!path)
    {
        ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
        ft_putendl_fd(cmd->argv[0], STDERR_FILENO);
        return (127);
    }
    pid = fork();
    if (pid == -1)
        return (perror("fork"), 1);
    if (pid == 0)
    {
        /* FILHO */
        apply_redirections(cmd->redirs);  /* aplicar <, >, >>, << */
        envp = env_list_to_array(msh->env);
        execve(path, cmd->argv, envp);
        /* execve falhou */
        perror(cmd->argv[0]);
        exit(127);
    }
    /* PAI */
    free(path);
    waitpid(pid, &wstatus, 0);
    update_exit_status(msh, wstatus);
    return (msh->exit_status);
}
```

### 3.5 Resolução de Caminhos e PATH

Quando o utilizador escreve `ls`, o Minishell deve:
1. Verificar se o comando contém `/`: se sim, usar o caminho diretamente
2. Caso contrário, iterar pelas entradas de `PATH`, concatenando cada uma com o comando
3. Usar `access(path, X_OK)` (ou `stat` + verificação de permissão) para verificar se é executável

```c
char    *resolve_path(const char *cmd, t_list *env)
{
    char    *path_var;
    char   **dirs;
    char    *full_path;
    int      i;

    if (ft_strchr(cmd, '/'))  /* caminho absoluto ou relativo */
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    path_var = env_get(env, "PATH");
    if (!path_var)
        return (NULL);
    dirs = ft_split(path_var, ':');
    i = 0;
    while (dirs[i])
    {
        full_path = ft_strjoin3(dirs[i], "/", cmd);  /* dir + "/" + cmd */
        if (access(full_path, X_OK) == 0)
        {
            ft_free_split(dirs);
            return (full_path);
        }
        free(full_path);
        i++;
    }
    ft_free_split(dirs);
    return (NULL);
}
```

**Casos especiais:**
- `PATH` vazio: só executáveis com caminho explícito funcionam
- `PATH` não definido: o Subject não especifica claramente — testa com bash
- Diretório no argumento sem executável: `access` retorna `-1` com `EACCES` ou `ENOENT`

### 3.6 Processos Zombie e Processos Órfãos

**Processo Zombie:**
Quando um filho termina antes de o pai chamar `waitpid`, o filho torna-se um **zombie** — o seu PID e status estão retidos na tabela de processos do kernel aguardando que o pai os recolha. Os zombies consomem entradas na tabela de processos (recurso limitado).

No Minishell, é crítico chamar `waitpid` para **todos** os filhos criados:
- Em pipelines: esperar pelos PIDs de **todos** os filhos do pipe
- Em builtins que criam filhos: também aguardar
- Nunca sair do shell sem terminar todos os filhos pendentes

**Processo Órfão:**
Quando o pai termina antes do filho, o filho torna-se órfão e é **adotado pelo `init`** (PID 1), que automaticamente chama `waitpid` por ele. No contexto do Minishell, um órfão não é problema desde que o shell termine normalmente.

### 3.7 Limitações e Regras — Contexto 42 / Norminette

**Requisitos críticos:**
- O shell **nunca** deve terminar ao executar um comando externo que falha ou termina com erro
- O **processo pai** deve sempre chamar `waitpid` — nunca deixar zombies
- Os builtins **não devem usar fork** (exceto em pipeline) — devem executar no processo do shell diretamente
- `exit` no filho deve ser `exit()` da stdlib, não `return` do main

**Norminette:**
- A função de execução de um comando externo pode ser > 25 linhas — dividir em:
  - `exec_external(cmd, msh)` — coordena o fluxo
  - `child_process(cmd, msh)` — execução no filho (apenas chamada após fork)
  - `resolve_path(cmd, env)` — resolução do caminho
  - `update_exit_status(msh, wstatus)` — atualização de `$?`

### 3.8 Funções Permitidas Relevantes

```c
#include <unistd.h>
pid_t fork(void);
int   execve(const char *pathname, char *const argv[], char *const envp[]);
int   access(const char *pathname, int mode);

#include <sys/wait.h>
pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);
pid_t wait3(int *wstatus, int options, struct rusage *rusage);
pid_t wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);

#include <unistd.h>
pid_t getpid(void);   /* PID do processo atual — para $$ */
pid_t getppid(void);  /* PID do processo pai */
```

> `wait3` e `wait4` são variantes avançadas de `waitpid` com informação de uso de recursos. Não são necessários para o Minishell básico.

### 3.9 Metodologia de Aplicação (Pseudocódigo)

```
FUNÇÃO execute_command(cmd: t_cmd, msh: t_msh) → int:
    SE is_builtin(cmd.argv[0]):
        RETORNAR execute_builtin(cmd, msh)  // executa no processo atual
    
    path = resolve_path(cmd.argv[0], msh.env)
    SE path == NULL:
        ESCREVER STDERR: "minishell: " + cmd.argv[0] + ": command not found\n"
        msh.exit_status = 127
        RETORNAR 127
    
    pid = fork()
    SE pid == -1:
        perror("fork")
        free(path)
        RETORNAR 1
    
    SE pid == 0:  // FILHO
        aplicar_redirecionamentos(cmd.redirs)
        envp = env_list_to_array(msh.env)
        execve(path, cmd.argv, envp)
        // se chegou aqui, execve falhou
        ft_putstr_fd("minishell: ", STDERR_FILENO)
        perror(cmd.argv[0])
        exit(127)
    
    // PAI
    free(path)
    waitpid(pid, &wstatus, 0)
    update_exit_status(msh, wstatus)
    RETORNAR msh.exit_status


FUNÇÃO execute_pipeline(cmd_table: t_cmd_table, msh: t_msh) → int:
    // Ver Módulo 06 para a implementação completa com pipes
    n = número de cmds no cmd_table
    SE n == 1:
        RETORNAR execute_command(cmd_table.cmds[0], msh)
    
    pids = array de n PIDs
    init_pipes(cmd_table)  // criar pipe FDs
    
    PARA i DE 0 ATÉ n-1:
        pids[i] = fork()
        SE pids[i] == 0:  // FILHO i
            configurar_pipe_fds(i, n, cmd_table)
            fechar_todos_os_pipes(cmd_table)
            executar_cmd_ou_builtin(cmd_table.cmds[i], msh)
            exit(exit_status)
    
    fechar_todos_os_pipes_no_pai(cmd_table)
    
    PARA i DE 0 ATÉ n-1:
        waitpid(pids[i], &wstatus, 0)
    update_exit_status(msh, último_wstatus)
    RETORNAR msh.exit_status
```

### 3.10 Metodologia de Testes

```bash
# Teste 1 — Comando externo simples
ls -la /tmp          # deve listar /tmp, $?=0

# Teste 2 — Comando não encontrado
nonexistent_cmd      # "minishell: nonexistent_cmd: command not found", $?=127

# Teste 3 — Comando com caminho absoluto
/bin/echo hello      # deve imprimir "hello"

# Teste 4 — Exit status
/bin/false           # programa que exit(1)
echo $?              # deve mostrar 1

# Teste 5 — Exit status de sinal
sleep 10 &           # background (bonus) ou em outro terminal
kill -SIGKILL <pid>
echo $?              # deve mostrar 137 (128 + 9)

# Teste 6 — PATH
echo $PATH           # verificar PATH
PATH=/tmp            # limitar PATH a /tmp
ls                   # deve falhar com "command not found"
PATH=/bin:/usr/bin   # restaurar

# Teste 7 — Valgrind (sem zombies, sem leaks)
valgrind --leak-check=full ./minishell
# executar: ls, cat, echo, etc.
# verificar: "0 bytes in 0 blocks" no leak summary
```

---

## 4. Conclusão

### 4.1 Síntese

O modelo fork-exec-wait é a mecânica central de qualquer shell UNIX. A sua implementação correta garante que o Minishell pode executar qualquer programa externo, capturar o seu resultado, e continuar funcionando. A elegância deste modelo — em que o filho configura o seu próprio ambiente I/O antes de se substituir pelo programa — é o que torna possível pipes e redirecionamentos de forma limpa.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Shell morre após execve falhar** | Se execve falha e não há `exit()` no filho, o filho continua como shell duplicado | Sempre `exit(127)` após execve com falha no filho |
| **Zombies** | Fork sem waitpid | Para cada `fork()`, garantir que existe um `waitpid()` correspondente |
| **Builtins com fork** | Fazer `fork()` para builtins significa que as mudanças de estado (env, pwd) não afetam o pai | Builtins executam **no processo pai** (exceto em pipeline) |
| **envp não atualizado** | Passar o `envp` original em vez da lista interna modificada | Converter `t_list env` para `char**` na altura do `execve` |
| **FDs vazados no filho** | FDs extra (pipes já usados) ficam abertos após execve | Fechar todos os FDs desnecessários antes do execve |

### 4.3 Integração no Projeto Final

Este módulo integra-se diretamente com:
- **Módulo 06 (Pipes)**: O fork é feito por cada comando da pipeline; os FDs dos pipes são configurados entre o fork e o exec
- **Módulo 07 (Redirecionamentos)**: Os redirecionamentos são aplicados no filho, após o fork e antes do exec
- **Módulo 08 (Sinais)**: Os sinais são redefinidos no filho para o comportamento padrão antes do exec

---

## 5. Bibliografia

### Documentação Oficial

- `man 2 fork`, `man 2 execve`, `man 2 waitpid`, `man 2 wait`
- `man 2 access`, `man 2 getpid`
- [POSIX.1-2017 — §fork](https://pubs.opengroup.org/onlinepubs/9699919799/functions/fork.html)
- [POSIX.1-2017 — §execve](https://pubs.opengroup.org/onlinepubs/9699919799/functions/execve.html)

### Livros Clássicos

- **W. Richard Stevens, Stephen A. Rago** — *Advanced Programming in the UNIX Environment*, 3rd Ed.  
  Capítulo 8: Process Control — fork, exec, wait, exit, process relationships
- **Brian W. Kernighan, Rob Pike** — *The UNIX Programming Environment* (1984)  
  Capítulo 5: Shell Programming

### Tutoriais de Referência

- **Stephen Brennan** — *Write a Shell in C* — Secção "How shell starts processes"  
  https://brennan.io/2015/01/16/write-a-shell-in-c/

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
