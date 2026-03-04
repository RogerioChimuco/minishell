# 04 — Variáveis de Ambiente e Expansão ($)

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que são Variáveis de Ambiente?](#21-o-que-são-variáveis-de-ambiente)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que este módulo resolve](#24-problema-que-este-módulo-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 O Array `envp` — Estrutura e Semântica](#31-o-array-envp--estrutura-e-semântica)
  - [3.2 Gerir o Ambiente Internamente](#32-gerir-o-ambiente-internamente)
  - [3.3 Expansão de Parâmetros (`$`)](#33-expansão-de-parâmetros-)
  - [3.4 `$?` — Variável de Status de Saída](#34---variável-de-status-de-saída)
  - [3.5 Expansão dentro de Aspas Duplas](#35-expansão-dentro-de-aspas-duplas)
  - [3.6 Limitações e Regras — Contexto 42 / Norminette](#36-limitações-e-regras--contexto-42--norminette)
  - [3.7 Funções Permitidas Relevantes](#37-funções-permitidas-relevantes)
  - [3.8 Metodologia de Aplicação (Pseudocódigo)](#38-metodologia-de-aplicação-pseudocódigo)
  - [3.9 Metodologia de Testes](#39-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que são Variáveis de Ambiente?

As **variáveis de ambiente** são pares `NOME=VALOR` em formato string que o sistema operativo passa para cada processo. Elas constituem o **contexto de execução** do processo — informação sobre o utilizador (`USER`), diretório home (`HOME`), caminhos de pesquisa de executáveis (`PATH`), terminal atual (`TERM`), etc.

O shell é o principal gestor de variáveis de ambiente: pode **ler** (`echo $HOME`), **definir** (`export VAR=valor`), **remover** (`unset VAR`) e **exibir** (`env`) estas variáveis. Quando o shell lança um processo filho com `execve`, passa-lhe uma cópia do ambiente atual.

### 2.2 Contexto Histórico e Computacional

O conceito de environment foi introduzido no **UNIX Version 7** (1979), com a função `execve()` que aceita um terceiro argumento `envp` — um array de strings terminado por `NULL`. Antes disso, a comunicação entre shell e processos era feita por ficheiros e parâmetros posicionais.

A especificação **POSIX.1-2017 §8** formaliza as variáveis de ambiente e o seu comportamento. Variáveis como `PATH`, `HOME`, `OLDPWD`, `PS1`, `IFS`, `PWD` têm semântica especial definida pelo standard.

### 2.3 Relevância no Projeto Minishell da 42

Este módulo é transversal a todo o minishell:
- O **executor** precisa do `PATH` para localizar comandos (ex: `ls` → `/bin/ls`)
- O **builtin `cd`** deve atualizar `PWD` e `OLDPWD`
- A **expansão de `$`** deve consultar o ambiente para substituir `$VAR` pelo seu valor
- O **builtin `export`** adiciona/modifica variáveis
- O **builtin `unset`** remove variáveis
- O **builtin `env`** exibe o ambiente atual

Uma implementação incorreta deste módulo afeta diretamente todos os outros componentes.

### 2.4 Problema que este módulo resolve

O problema central é o **ciclo de vida e a mutabilidade** do ambiente:

O `envp` original passado ao `main()` é um array **estático** — não pode ser modificado nem expandido em memória. O Minishell precisa de uma cópia dinâmica deste array que possa ser alterada durante a sessão. Esta cópia vive num `t_list` ou array alocado no heap.

---

## 3. Desenvolvimento

### 3.1 O Array `envp` — Estrutura e Semântica

Quando o kernel lança um processo, passa o ambiente como um array de strings:
```c
int main(int argc, char **argv, char **envp)
```

Cada elemento de `envp` tem o formato `"NOME=VALOR"`:
```
envp[0] = "USER=john"
envp[1] = "HOME=/home/john"
envp[2] = "PATH=/usr/bin:/bin:/usr/local/bin"
envp[3] = "TERM=xterm-256color"
envp[4] = "PWD=/home/john/projects"
envp[5] = NULL  /* sentinela obrigatória */
```

**Variáveis com relevância especial para o Minishell:**

| Variável | Significado | Usada em |
|----------|-------------|----------|
| `PATH` | Lista de diretórios onde procurar executáveis | `execve` + pesquisa |
| `HOME` | Diretório home do utilizador | `cd` sem argumentos |
| `PWD` | Diretório de trabalho atual | `pwd`, `cd` |
| `OLDPWD` | Diretório anterior | `cd -` |
| `USER` / `LOGNAME` | Nome do utilizador | prompt |
| `SHLVL` | Nível de subshell | controlo de nesting |
| `_` | Último argumento do comando anterior | expansão |

### 3.2 Gerir o Ambiente Internamente

Como o `envp` original é imutável (os seus ponteiros apontam para região de memória do loader), o Minishell deve criar e gerir a sua própria cópia dinâmica:

```c
/* Abordagem: t_list de strings "NOME=VALOR" */
// Vantagem: usa a libft, fácil de iterar e modificar
t_list *dup_envp;  /* membro de t_msh */

/* Alternativa: array de char** (mais próximo de envp para passar ao execve) */
char **env_array;
```

**Duplicação inicial:**
```c
t_list *ft_dup_envp(char **envp)
{
    t_list *lst;
    int     i;
    char   *dup;

    lst = NULL;
    i = 0;
    while (envp[i])
    {
        dup = ft_strdup(envp[i]);
        if (!dup)
            return (ft_lstclear(&lst, free), NULL);
        ft_lstadd_back(&lst, ft_lstnew(dup));
        i++;
    }
    return (lst);
}
```

**Conversão para char** (para uso no `execve`):**
```c
char    **env_list_to_array(t_list *env)
{
    char    **arr;
    int       size;
    int       i;
    t_list   *node;

    size = ft_lstsize(env);
    arr = malloc((size + 1) * sizeof(char *));
    if (!arr)
        return (NULL);
    node = env;
    i = 0;
    while (node)
    {
        arr[i++] = ft_strdup((char *)node->content);
        node = node->next;
    }
    arr[i] = NULL;
    return (arr);
}
```

**Pesquisa de uma variável:**
```c
/* Retorna o valor da variável ou NULL se não existir */
char    *env_get(t_list *env, const char *name)
{
    size_t  len;
    t_list *node;

    len = ft_strlen(name);
    node = env;
    while (node)
    {
        if (ft_strncmp(node->content, name, len) == 0
            && ((char *)node->content)[len] == '=')
            return ((char *)node->content + len + 1); /* aponta para VALOR */
        node = node->next;
    }
    return (NULL);
}
```

### 3.3 Expansão de Parâmetros (`$`)

A **expansão de parâmetros** ocorre na fase de **pré-processamento do comando**, depois do parsing e antes da execução. Para cada token de tipo `WORD`, percorre-se o seu `value` à procura de `$`:

**Regras de expansão (POSIX §2.6.2):**

1. `$NOME` ou `${NOME}` → substituído pelo valor da variável `NOME`
2. `$?` → substituído pelo código de saída do último comando
3. `$$` → substituído pelo PID do shell atual
4. `$0` → nome do shell/script
5. `$1..$9` → parâmetros posicionais (não necessários no Minishell básico)
6. `$` sem caractere de nome válido a seguir → `$` literal

**Caracteres válidos num nome de variável:** `[A-Za-z_][A-Za-z0-9_]*`

**Algoritmo de expansão de um token:**
```
FUNÇÃO expand_token(value: string, env: t_list, exit_status: int) → string:
    resultado = buffer vazio
    i = 0
    
    ENQUANTO value[i] != '\0':
        SE value[i] != '$':
            adicionar value[i] ao resultado
            i++
        SENÃO:  // encontrámos '$'
            i++  // avançar sobre '$'
            SE value[i] == '?':
                adicionar itoa(exit_status) ao resultado
                i++
            SENÃO SE value[i] é letra ou '_':
                j = i
                ENQUANTO value[j] é alfanumérico ou '_': j++
                nome = substring(value, i, j)
                val = env_get(env, nome)
                SE val != NULL:
                    adicionar val ao resultado
                SENÃO:
                    adicionar "" (string vazia — variável inexistente = vazio)
                i = j
            SENÃO:
                adicionar '$' ao resultado ($ literal)
    
    RETORNAR resultado
```

**Exemplo:**
```bash
HOME=/home/john
echo "Hello $USER, your home is $HOME"
# Resultado: "Hello john, your home is /home/john"
```

### 3.4 `$?` — Variável de Status de Saída

O `$?` é a variável mais importante para controlo de fluxo em scripts. Armazena o **exit code do último comando** executado em foreground.

- Sucesso: `0`
- Erro genérico: `1`
- Erro de comando não encontrado: `127`
- Terminado por sinal N: `128 + N` (ex: SIGINT=2 → `130`)

No Minishell, o exit status deve ser guardado na estrutura global:
```c
typedef struct s_msh
{
    t_ast   *ast;
    t_list  *env;
    int      exit_status;  /* o valor de $? */
}   t_msh;
```

Atualizar após cada comando:
```c
/* Após waitpid: */
if (WIFEXITED(wstatus))
    g_msh.exit_status = WEXITSTATUS(wstatus);
else if (WIFSIGNALED(wstatus))
    g_msh.exit_status = 128 + WTERMSIG(wstatus);
```

### 3.5 Expansão dentro de Aspas Duplas

Dentro de aspas duplas, a expansão de `$` **permanece ativa**. Dentro de aspas simples, a expansão está **completamente desativa**.

```bash
VAR=hello
echo "$VAR"    # output: hello      (expansão ativa)
echo '$VAR'    # output: $VAR       (expansão inativa)
echo "$$"      # output: 1234       (PID do shell)
echo "$?"      # output: 0          (exit status anterior)
```

O Lexer deve registar em cada token se os caracteres foram lidos dentro de aspas simples ou duplas, para que a fase de expansão saiba se deve ou não expandir.

Uma abordagem comum: o Lexer marca segmentos do token com flags `QUOTED_SINGLE`, `QUOTED_DOUBLE`, `UNQUOTED`.

### 3.6 Limitações e Regras — Contexto 42 / Norminette

**Requisitos obrigatórios do Subject:**
- Expansão de `$VARNAME` e `$?`
- `export` sem argumentos deve exibir as variáveis exportadas (como `declare -x` no bash)
- `unset VAR` deve remover a variável do ambiente
- Variáveis definidas com `export A=B` devem ser herdadas pelos processos filhos

**Casos limite testados pelos avaliadores:**
```bash
export VAR=hello
echo $VAR           # hello
unset VAR
echo $VAR           # (linha vazia)
echo $UNDEFINED     # (linha vazia — não é erro)
echo "$?"           # exit status do echo anterior (0)
VAR=test; echo $VAR # Definição local: o Subject NÃO requer suporte a assignments
                    # sem export — testa com o teu evaluator
```

**Norminette:**
- A função de expansão será necessariamente longa — dividir em `expand_env_var()`, `expand_exit_status()`, `expand_pid()` e uma função auxiliar para extrair o nome da variável
- `exit_status` deve ser armazenado na struct global ou passado por ponteiro, não como variável global adicional (o Subject tipicamente permite apenas 1 global)

### 3.7 Funções Permitidas Relevantes

#### `getenv` (para consulta inicial / alternativa à lista interna)
```c
#include <stdlib.h>
char *getenv(const char *name);
// Procura a variável 'name' no ambiente do processo atual
// Retorna o ponteiro para o VALOR (não "NOME=VALOR")
// Retorna NULL se não existir
```
> Útil para inicialização, mas para modificações usa a tua lista interna — `getenv` não reflete mudanças feitas com `export` sem `setenv` (que não é permitida).

#### `access` (para verificar se o executável existe)
```c
#include <unistd.h>
int access(const char *pathname, int mode);
// mode: F_OK (existe?), X_OK (executável?), R_OK, W_OK
// Retorna 0 se OK, -1 se erro
```
Usado no algoritmo de pesquisa no `PATH`:
```c
// Para cada dir em PATH:
//   path = dir + "/" + comando
//   if (access(path, X_OK) == 0) → found!
```

#### `chdir` (para o builtin `cd`)
```c
#include <unistd.h>
int chdir(const char *path);
// Muda o diretório de trabalho atual
// Retorna 0 se sucesso, -1 se erro
```
Após `chdir`, atualizar `PWD` e `OLDPWD` no ambiente interno.

#### `getcwd` (para atualizar `PWD`)
```c
#include <unistd.h>
char *getcwd(char *buf, size_t size);
// Retorna o caminho absoluto do diretório atual
// Retorna NULL se erro
```

#### `stat` / `lstat` (para verificar propriedades de ficheiros)
```c
#include <sys/stat.h>
int stat(const char *pathname, struct stat *statbuf);
// statbuf.st_mode: S_ISREG() (ficheiro regular?), S_ISDIR() (diretório?)
```

### 3.8 Metodologia de Aplicação (Pseudocódigo)

```
FLUXO COMPLETO:

1. Inicialização (main):
   msh.env = ft_dup_envp(envp)
   msh.exit_status = 0

2. Após parsing — Expansão de tokens:
   PARA CADA cmd_table em msh.ast:
     PARA CADA cmd em cmd_table:
       PARA CADA arg em cmd.argv:
         arg = expand_token(arg, msh.env, msh.exit_status)
       PARA CADA redir em cmd.redirs:
         redir.file = expand_token(redir.file, msh.env, msh.exit_status)
              (EXCEPTO para heredoc delimiters em modo 'quoted')

3. Builtin export:
   FUNÇÃO builtin_export(args: string[], env: *t_list):
     SE sem argumentos:
       EXIBIR todas as vars no formato "declare -x NOME=\"VALOR\""
     PARA CADA arg em args:
       SE arg contém '=':
         nome, valor = split(arg, '=')
         env_set(env, nome, valor)
       SENÃO:
         env_mark_exported(env, arg)  // marca como exportada sem valor

4. Builtin unset:
   FUNÇÃO builtin_unset(args: string[], env: *t_list):
     PARA CADA arg em args:
       remover nó da lista onde content começa com arg + '='

5. Pesquisa no PATH:
   FUNÇÃO resolve_command(cmd: string, env: t_list) → string:
     SE cmd contém '/':
       SE access(cmd, X_OK) == 0: RETORNAR ft_strdup(cmd)
       RETORNAR NULL
     path_var = env_get(env, "PATH")
     dirs = split(path_var, ':')
     PARA CADA dir em dirs:
       full_path = dir + "/" + cmd
       SE access(full_path, X_OK) == 0:
         RETORNAR ft_strdup(full_path)
     RETORNAR NULL  // comando não encontrado → exit 127
```

### 3.9 Metodologia de Testes

```bash
# Teste 1 — Expansão básica
export GREETING=hello
echo $GREETING          # esperado: hello

# Teste 2 — Variável não existente
echo $NONEXISTENT       # esperado: (linha vazia)

# Teste 3 — $? após sucesso e erro
ls /tmp                 # sucesso
echo $?                 # esperado: 0
ls /nao_existe          # falha
echo $?                 # esperado: 2 (ou o código retornado pelo ls)

# Teste 4 — $? dentro de aspas duplas
ls /nao_existe
echo "$?"               # esperado: 2 (expansão ativa em duplas)

# Teste 5 — $? dentro de aspas simples
echo '$?'               # esperado: $? (literal, sem expansão)

# Teste 6 — export sem args
export                  # deve listar todas as variáveis exportadas

# Teste 7 — unset e verificação
export TEST=value
echo $TEST              # value
unset TEST
echo $TEST              # (vazio)

# Teste 8 — PATH e resolução de comandos
echo $PATH              # deve mostrar o PATH atual
which ls                # deve mostrar /bin/ls (ou similar)
```

---

## 4. Conclusão

### 4.1 Síntese

As variáveis de ambiente são o estado persistente do shell. A gestão correta deste módulo — duplicação do `envp`, expansão de `$`, manipulação por `export`/`unset` — é fundamental para a interoperabilidade do Minishell com os processos que lança.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Modificar o envp original** | Os ponteiros de `envp` apontam para memória gerida pelo loader | Duplicar todos os valores com `ft_strdup` |
| **`$?` não atualizado** | `$?` mantém valor stale após comandos | Atualizar `exit_status` após **cada** `waitpid` e builtin |
| **Expansão em heredoc delimiters** | Se `<< "EOF"` (quoted), o corpo do heredoc não deve expandir `$` | Registar se o delimitador foi quoted |
| **Variável com valor vazio** | `export A=` deve definir A com valor vazio `""`, não remover A | Distinguir "variável ausente" de "variável com valor vazio" |
| **PATH não encontrado** | `execve` retorna `ENOENT` se o caminho não existe | Usar `access` antes de `execve` e reportar `command not found` |

### 4.3 Integração no Projeto Final

A expansão de `$` deve ocorrer **depois** do parsing e **antes** da execução. Nunca na fase de tokenização (o Lexer deve preservar `$VAR` como está). A conversão do ambiente interno (`t_list`) para `char**` deve ser feita imediatamente antes de cada `execve`.

---

## 5. Bibliografia

### Documentação Oficial

- [POSIX.1-2017 — §2.5 Parameters and Variables](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_05)
- [POSIX.1-2017 — §2.6.2 Parameter Expansion](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_06_02)
- [POSIX.1-2017 — §8 Environment Variables](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap08.html)
- `man 3 getenv`, `man 2 chdir`, `man 3 getcwd`, `man 2 access`

### Livros Clássicos

- **W. Richard Stevens, Stephen A. Rago** — *Advanced Programming in the UNIX Environment*, 3rd Ed.  
  Capítulo 7.9: Environment List; Capítulo 8: Process Control

### Recursos Web

- **GNU Bash Manual** — §3.4 Shell Parameters & §3.5 Shell Expansions  
  https://www.gnu.org/software/bash/manual/bash.html#Shell-Parameters

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
