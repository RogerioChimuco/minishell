# 09 — Builtins do Shell

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que são Builtins?](#21-o-que-são-builtins)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Por que os Builtins existem?](#24-por-que-os-builtins-existem)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 Lista de Builtins Obrigatórios](#31-lista-de-builtins-obrigatórios)
  - [3.2 `echo` — Exibir Argumentos](#32-echo--exibir-argumentos)
  - [3.3 `cd` — Mudar Diretório](#33-cd--mudar-diretório)
  - [3.4 `pwd` — Diretório Atual](#34-pwd--diretório-atual)
  - [3.5 `export` — Exportar Variáveis](#35-export--exportar-variáveis)
  - [3.6 `unset` — Remover Variáveis](#36-unset--remover-variáveis)
  - [3.7 `env` — Exibir Ambiente](#37-env--exibir-ambiente)
  - [3.8 `exit` — Sair do Shell](#38-exit--sair-do-shell)
  - [3.9 Builtins em Pipelines](#39-builtins-em-pipelines)
  - [3.10 Limitações e Regras — Contexto 42 / Norminette](#310-limitações-e-regras--contexto-42--norminette)
  - [3.11 Funções Permitidas Relevantes](#311-funções-permitidas-relevantes)
  - [3.12 Metodologia de Testes](#312-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que são Builtins?

Os **builtins** (comandos embutidos) são comandos que o shell **implementa internamente**, sem criar um processo filho. Ao contrário de `ls` (que é `/bin/ls`), os builtins são funções C dentro do próprio executável do shell.

Quando o utilizador escreve `cd /tmp`, o shell não procura `/bin/cd` nem faz `fork + execve`. Em vez disso, chama diretamente a função `builtin_cd("/tmp")` no processo atual.

### 2.2 Contexto Histórico e Computacional

Os primeiros builtins surgiram com o Thompson Shell (1971) e o Bourne Shell (1979). Alguns comandos *exigem* ser builtins por definição: `cd` muda o diretório do processo atual — se fosse externo, mudaria o diretório do filho, e o pai (o shell) permaneceria no mesmo sítio. O mesmo se aplica a `export`, `unset`, e `exit`.

### 2.3 Relevância no Projeto Minishell da 42

O Subject da 42 lista **7 builtins obrigatórios**: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`. Estes são testados individualmente pelos avaliadores, incluindo casos de erro, exit codes, e interação com o ambiente.

### 2.4 Por que os Builtins existem?

| Razão | Builtin | Explicação |
|-------|---------|------------|
| **Acesso ao estado do shell** | `cd`, `export`, `unset` | Precisam modificar o estado interno do shell (env, cwd) |
| **Controlo do próprio processo** | `exit` | Só o próprio processo pode terminar a si mesmo |
| **Eficiência** | `echo`, `pwd` | Evitar fork desnecessário para operações simples |

---

## 3. Desenvolvimento

### 3.1 Lista de Builtins Obrigatórios

| Builtin | Efeito | Modifica estado do shell |
|---------|--------|--------------------------|
| `echo [-n] [args...]` | Imprimir argumentos | Não |
| `cd [path]` | Mudar diretório de trabalho | Sim (`PWD`, `OLDPWD`) |
| `pwd` | Imprimir diretório atual | Não |
| `export [NOME=VALOR...]` | Exportar variável | Sim (env) |
| `unset [NOME...]` | Remover variável do ambiente | Sim (env) |
| `env` | Listar todas as variáveis exportadas | Não |
| `exit [n]` | Sair do shell com código n | — (o shell termina) |

**Deteção de builtin:**
```c
int     is_builtin(const char *cmd)
{
    static const char *builtins[] = {
        "echo", "cd", "pwd", "export", "unset", "env", "exit", NULL
    };
    int i;

    i = 0;
    while (builtins[i])
    {
        if (ft_strcmp(cmd, builtins[i]) == 0)
            return (1);
        i++;
    }
    return (0);
}
```

### 3.2 `echo` — Exibir Argumentos

**Sintaxe:** `echo [-n] [string ...]`

**Comportamento:**
- Imprime os argumentos separados por espaços
- Por defeito, imprime um newline no final
- Com a flag `-n`, suprime o newline final
- **Não** interpreta escape sequences como `\n`, `\t` (ao contrário do `echo` do Bash com `-e`)

```c
int     builtin_echo(char **argv)
{
    int newline;
    int i;

    newline = 1;
    i = 1;
    while (argv[i] && ft_strcmp(argv[i], "-n") == 0)
    {
        newline = 0;
        i++;
    }
    while (argv[i])
    {
        ft_putstr_fd(argv[i], STDOUT_FILENO);
        if (argv[i + 1])
            write(STDOUT_FILENO, " ", 1);
        i++;
    }
    if (newline)
        write(STDOUT_FILENO, "\n", 1);
    return (0);
}
```

**Casos especiais testados:**
```bash
echo                    # imprime linha vazia (só o newline)
echo -n                 # imprime nada (sem newline)
echo -n -n -n hello     # imprime "hello" sem newline (múltiplos -n)
echo ""                 # imprime linha vazia
echo "hello world"      # imprime "hello world" (as aspas são removidas pelo lexer)
```

### 3.3 `cd` — Mudar Diretório

**Sintaxe:** `cd [path]`

**Comportamento:**
- Sem argumento: vai para `$HOME`
- `cd -`: vai para `$OLDPWD` e imprime o novo diretório
- Com argumento: muda para o diretório especificado

```c
int     builtin_cd(char **argv, t_msh *msh)
{
    char   *path;
    char   *old_pwd;
    char    cwd[PATH_MAX];

    old_pwd = ft_strdup(env_get(msh->env, "PWD"));
    if (!argv[1])
    {
        path = env_get(msh->env, "HOME");
        if (!path)
        {
            ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
            return (free(old_pwd), 1);
        }
    }
    else if (ft_strcmp(argv[1], "-") == 0)
    {
        path = env_get(msh->env, "OLDPWD");
        if (!path)
        {
            ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
            return (free(old_pwd), 1);
        }
        ft_putendl_fd(path, STDOUT_FILENO);  /* cd - imprime o diretório destino */
    }
    else
        path = argv[1];
    if (chdir(path) == -1)
    {
        ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
        perror(path);
        return (free(old_pwd), 1);
    }
    getcwd(cwd, PATH_MAX);
    env_set(msh->env, "OLDPWD", old_pwd);
    env_set(msh->env, "PWD", cwd);
    free(old_pwd);
    return (0);
}
```

**Casos especiais:**
```bash
cd          # vai para $HOME
cd -        # vai para $OLDPWD, imprime o caminho
cd /tmp     # muda para /tmp
cd /nao_existe  # minishell: cd: /nao_existe: No such file or directory; $?=1
cd ""       # comportamento bash: vai para $HOME (ou erro, testa com bash)
```

### 3.4 `pwd` — Diretório Atual

**Sintaxe:** `pwd`

**Comportamento:** Imprime o caminho absoluto do diretório de trabalho atual.

```c
int     builtin_pwd(void)
{
    char cwd[PATH_MAX];

    if (!getcwd(cwd, PATH_MAX))
    {
        perror("pwd");
        return (1);
    }
    ft_putendl_fd(cwd, STDOUT_FILENO);
    return (0);
}
```

> Alternativa: usar `env_get(msh->env, "PWD")` — mais rápido mas pode estar desatualizado em edge cases. O `getcwd()` é sempre preciso.

### 3.5 `export` — Exportar Variáveis

**Sintaxe:** `export [NOME[=VALOR]...]`

**Comportamento:**
- Sem argumentos: lista todas as variáveis exportadas no formato `declare -x NOME="VALOR"`
- Com argumentos: adiciona/modifica variáveis no ambiente

```c
int     builtin_export(char **argv, t_msh *msh)
{
    int i;

    if (!argv[1])
        return (print_export(msh->env));  /* listar todas */
    i = 1;
    while (argv[i])
    {
        if (!is_valid_identifier(argv[i]))
        {
            ft_putstr_fd("minishell: export: '", STDERR_FILENO);
            ft_putstr_fd(argv[i], STDERR_FILENO);
            ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
            return (1);
        }
        env_set_or_add(msh->env, argv[i]);  /* NOME=VALOR ou só NOME */
        i++;
    }
    return (0);
}
```

**Validação do identificador:**
- Deve começar com letra ou `_`
- Pode conter letras, dígitos, e `_`
- `export 1VAR=x` → erro: "not a valid identifier"
- `export VAR+=x` → comportamento do bash (não é necessário para a 42 básica)

### 3.6 `unset` — Remover Variáveis

**Sintaxe:** `unset [NOME...]`

```c
int     builtin_unset(char **argv, t_msh *msh)
{
    int i;

    i = 1;
    while (argv[i])
    {
        env_remove(msh->env, argv[i]);  /* remover da lista interna */
        i++;
    }
    return (0);
}
```

**`env_remove` — remover nó da lista:**
```c
void    env_remove(t_list **env, const char *name)
{
    t_list *prev;
    t_list *curr;
    size_t  len;

    len = ft_strlen(name);
    prev = NULL;
    curr = *env;
    while (curr)
    {
        if (ft_strncmp(curr->content, name, len) == 0
            && ((char *)curr->content)[len] == '=')
        {
            if (prev)
                prev->next = curr->next;
            else
                *env = curr->next;
            free(curr->content);
            free(curr);
            return ;
        }
        prev = curr;
        curr = curr->next;
    }
}
```

### 3.7 `env` — Exibir Ambiente

**Sintaxe:** `env`

**Comportamento:** Lista todas as variáveis de ambiente no formato `NOME=VALOR`, uma por linha.

```c
int     builtin_env(t_msh *msh)
{
    t_list *node;

    node = msh->env;
    while (node)
    {
        ft_putendl_fd((char *)node->content, STDOUT_FILENO);
        node = node->next;
    }
    return (0);
}
```

### 3.8 `exit` — Sair do Shell

**Sintaxe:** `exit [n]`

**Comportamento:**
- Sem argumento: sair com o exit status do último comando (`$?`)
- Com argumento numérico: sair com o código `n % 256`
- Com argumento não numérico: erro + sair com código 2 (comportamento bash)
- Com mais de 1 argumento: erro "too many arguments", mas **não sair**

```c
int     builtin_exit(char **argv, t_msh *msh)
{
    long    code;

    ft_putendl_fd("exit", STDERR_FILENO);
    if (!argv[1])
        clean_and_exit(msh, msh->exit_status);
    if (argv[2])
    {
        ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
        return (1);  /* não sair! */
    }
    if (!is_numeric(argv[1]))
    {
        ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
        ft_putstr_fd(argv[1], STDERR_FILENO);
        ft_putendl_fd(": numeric argument required", STDERR_FILENO);
        clean_and_exit(msh, 2);
    }
    code = ft_atol(argv[1]);
    clean_and_exit(msh, (int)(code % 256));
    return (0);  /* nunca alcançado */
}
```

### 3.9 Builtins em Pipelines

Quando um builtin aparece numa pipeline (ex: `echo test | cat`), ele deve ser executado num **processo filho** (como qualquer outro comando), porque as suas mudanças de estado não devem afetar o shell pai:

```bash
export VAR=hello | echo test   # o export NÃO persiste no pai — está num pipe
```

Porém, quando o builtin é o **único comando** (sem pipeline), executa **no processo pai**:
```bash
export VAR=hello    # sem pipe → executa no pai → VAR persiste
```

Implementação:
```c
if (n_cmds_in_pipeline == 1 && is_builtin(cmd->argv[0]))
    return (execute_builtin(cmd, msh));   /* no pai */
else
{
    pid = fork();
    if (pid == 0)
    {
        execute_builtin_or_external(cmd, msh);
        exit(msh->exit_status);
    }
    waitpid(pid, &wstatus, 0);
}
```

### 3.10 Limitações e Regras — Contexto 42 / Norminette

**Tabela de exit codes esperados:**

| Situação | Exit Code |
|----------|-----------|
| Sucesso | `0` |
| Erro genérico (cd falha, etc.) | `1` |
| Argumento inválido em exit | `2` |
| Comando não encontrado | `127` |

**Norminette:**
- Uma função por builtin
- Função dispatch: `execute_builtin(cmd, msh)` que usa array de ponteiros de função ou `if/else if` (preferível em 42 por clareza)

### 3.11 Funções Permitidas Relevantes

```c
#include <unistd.h>
int  chdir(const char *path);     /* builtin cd */
char *getcwd(char *buf, size_t size); /* builtin pwd */

#include <stdlib.h>
void exit(int status);            /* builtin exit */

#include <limits.h>
/* PATH_MAX — tamanho máximo de um caminho de ficheiro */
```

### 3.12 Metodologia de Testes

```bash
# echo
echo hello world          # hello world
echo -n hello             # hello (sem newline)
echo ""                   # (linha vazia)
echo -n                   # (nada)

# cd
cd /tmp && pwd            # /tmp
cd -                      # volta ao anterior, imprime o caminho
cd                        # vai para $HOME
cd /nao_existe            # errro; $?=1

# pwd
cd /tmp
pwd                       # /tmp

# export + env
export TEST=42
env | grep TEST           # TEST=42
export                    # deve listar: declare -x TEST="42"
export 1INVALID=x         # erro: not a valid identifier

# unset
export A=hello
echo $A                   # hello
unset A
echo $A                   # (vazio)

# env
env                       # listar todas as vars actuais

# exit
exit 0                    # sai com código 0
exit 42                   # sai com código 42
exit abc                  # numeric argument required; sai com 2
exit 1 2                  # too many arguments; NÃO sai; $?=1
```

---

## 4. Conclusão

### 4.1 Síntese

Os 7 builtins obrigatórios são relativamente simples de implementar individualmente, mas exigem atenção nos casos de borda (argumentos inválidos, `cd -`, `export` sem args, `exit` com múltiplos args). A distinção mais importante é que builtins fora de pipeline executam **no processo pai**, afetando diretamente o estado do shell (ambiente, diretório, exit status).

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **`cd` não atualiza env** | `chdir` altera o CWD do processo mas `PWD` fica stale | Atualizar `PWD` e `OLDPWD` manualmente após `chdir` |
| **`exit` em pipeline não sai** | `| exit 1 |` deve sair apenas do filho | Verificar contexto: builtin em pipeline → filho; sozinho → pai |
| **`echo -n -n` não tratado** | Múltiplos `-n` devem ser aceites | Loop sobre os `-n` iniciais |
| **`export` com lista vazia** | Se env está vazio, não imprimir nada | Verificar `!node` antes do loop |
| **`exit` com argumento > 255** | `exit 300` deve ser `300 % 256 = 44` | Usar `ft_atol` e aplicar `% 256` |

---

## 5. Bibliografia

### Documentação Oficial

- `man 1 bash` — SHELL BUILTIN COMMANDS
- [POSIX.1-2017 — §2.14 Special Built-In Utilities](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_14)
- [POSIX.1-2017 — cd utility](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cd.html)
- [POSIX.1-2017 — echo utility](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/echo.html)

### Recursos Web

- **GNU Bash Manual** — §4 Shell Builtin Commands  
  https://www.gnu.org/software/bash/manual/bash.html#Shell-Builtin-Commands

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
