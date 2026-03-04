# 07 — Redirecionamentos de I/O

---

## Índice

- [2. Introdução](#2-introdução)
  - [2.1 O que são Redirecionamentos?](#21-o-que-são-redirecionamentos)
  - [2.2 Contexto Histórico e Computacional](#22-contexto-histórico-e-computacional)
  - [2.3 Relevância no Projeto Minishell da 42](#23-relevância-no-projeto-minishell-da-42)
  - [2.4 Problema que este módulo resolve](#24-problema-que-este-módulo-resolve)
- [3. Desenvolvimento](#3-desenvolvimento)
  - [3.1 Tipos de Redirecionamentos](#31-tipos-de-redirecionamentos)
  - [3.2 `<` — Redirecionamento de Input](#32----redirecionamento-de-input)
  - [3.3 `>` e `>>` — Redirecionamento de Output](#33--e----redirecionamento-de-output)
  - [3.4 `<<` — Here-Document (Heredoc)](#34-----here-document-heredoc)
  - [3.5 Múltiplos Redirecionamentos](#35-múltiplos-redirecionamentos)
  - [3.6 Redirecionamentos em Pipelines](#36-redirecionamentos-em-pipelines)
  - [3.7 Limitações e Regras — Contexto 42 / Norminette](#37-limitações-e-regras--contexto-42--norminette)
  - [3.8 Funções Permitidas Relevantes](#38-funções-permitidas-relevantes)
  - [3.9 Metodologia de Aplicação (Pseudocódigo)](#39-metodologia-de-aplicação-pseudocódigo)
  - [3.10 Metodologia de Testes](#310-metodologia-de-testes)
- [4. Conclusão](#4-conclusão)
- [5. Bibliografia](#5-bibliografia)

---

## 2. Introdução

### 2.1 O que são Redirecionamentos?

Os **redirecionamentos de I/O** permitem alterar para onde ão enviados (ou donde vêm) os streams padrão de um processo. Em vez de ler do teclado ou escrever no terminal, o processo pode ler de um ficheiro ou escrever para outro.

```bash
cat < input.txt         # stdin é o ficheiro em vez do teclado
ls > output.txt         # stdout vai para o ficheiro em vez do terminal
echo "a" >> log.txt     # stdout é ACRESCENTADO ao ficheiro
cat << END              # stdin é o "here-document" até encontrar END
  hello world
END
```

### 2.2 Contexto Histórico e Computacional

Os redirecionamentos foram introduzidos por Ken Thompson no UNIX V1 (1971). A notação `>` para output e `<` para input tornou-se parte da filosofia UNIX. O `>>` (append) e o `<<` (heredoc) foram adicionados nas versões subsequentes.

O heredoc (`<<`) foi concebido para passar texto multi-linha como input a comandos, sem necessitar de ficheiros temporários — embora algumas implementações (incluindo Bash) usem ficheiros temporários internamente.

### 2.3 Relevância no Projeto Minishell da 42

Os quatro tipos de redirecionamento (`<`, `>`, `>>`, `<<`) são **todos obrigatórios** no Subject da 42. Além disso, o heredoc tem comportamento específico relativamente à expansão de variáveis: se o delimitador estiver entre aspas, a expansão de `$` no corpo do heredoc deve ser inibida.

### 2.4 Problema que este módulo resolve

O problema é alterar os FDs padrão (0, 1, 2) do processo filho antes de executar o programa. Como os FDs são herdados pelo `execve`, se o filho fizer `dup2(file_fd, STDOUT_FILENO)` antes do `execve`, o programa executado vai naturalmente escrever para o ficheiro em vez do terminal.

---

## 3. Desenvolvimento

### 3.1 Tipos de Redirecionamentos

| Operador | Token | Efeito |
|----------|-------|--------|
| `< ficheiro` | `REDIR_IN` | Substituir stdin (FD 0) pelo ficheiro aberto para leitura |
| `> ficheiro` | `REDIR_OUT` | Substituir stdout (FD 1) pelo ficheiro criado/truncado para escrita |
| `>> ficheiro` | `REDIR_APPEND` | Substituir stdout (FD 1) pelo ficheiro aberto em append |
| `<< DELIM` | `HEREDOC` | Substituir stdin (FD 0) por um pipe com o conteúdo do heredoc |

### 3.2 `<` — Redirecionamento de Input

```c
/* Aplicar redirecionamento de input para o filho: */
int fd = open(filename, O_RDONLY);
if (fd == -1)
{
    perror(filename);
    exit(1);
}
dup2(fd, STDIN_FILENO);
close(fd);
/* A partir daqui, o processo filho lê do ficheiro em vez do teclado */
```

Flags do `open`:
- `O_RDONLY` — abrir apenas para leitura

**Erros comuns:**
- Ficheiro não existe → `No such file or directory`
- Sem permissão de leitura → `Permission denied`

### 3.3 `>` e `>>` — Redirecionamento de Output

```c
/* Redirecionamento de output (truncar): */
int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

/* Redirecionamento de output (append): */
int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

if (fd == -1)
{
    perror(filename);
    exit(1);
}
dup2(fd, STDOUT_FILENO);
close(fd);
```

**Flags do `open`:**
- `O_WRONLY` — abrir apenas para escrita
- `O_CREAT` — criar o ficheiro se não existir (requer o parâmetro `mode`)
- `O_TRUNC` — truncar o ficheiro se existir (apagar conteúdo)
- `O_APPEND` — posicionar o ponteiro no final antes de cada `write`
- `mode: 0644` — permissões: owner rw, group r, others r (standard)

### 3.4 `<<` — Here-Document (Heredoc)

O heredoc é o redirecionamento mais complexo. O comportamento esperado:

```bash
cat << EOF
Hello $USER
This is line 2
EOF
```

1. O shell lê linhas do utilizador até encontrar o delimitador (`EOF`) numa linha isolada
2. O conteúdo lido (com expansões de `$` se o delimitador não for quoted) é fornecido como stdin ao comando

**Implementação para o Minishell:**

Existem duas abordagens comuns:

**Abordagem A — Pipe com processo de leitura:**
```
1. Criar um pipe
2. Ler linhas (readline ou read) até encontrar DELIM
3. Escrever cada linha no write end do pipe
4. Fechar o write end
5. O read end passa a ser o stdin do comando
```

**Abordagem B — Ficheiro temporário:**
```
1. Criar ficheiro em /tmp (com mkstemp ou nome único)
2. Escrever o conteúdo para o ficheiro
3. Abrir o ficheiro para leitura
4. Aplicar como stdin e apagar o ficheiro
```

A **Abordagem A (pipe)** é mais limpa e não deixa ficheiros temporários. Exemplo:

```c
int     do_heredoc(const char *delimiter, int expand, t_msh *msh)
{
    int     fd[2];
    char   *line;
    char   *expanded;

    if (pipe(fd) == -1)
        return (perror("pipe"), -1);
    while (1)
    {
        line = readline("> ");  /* prompt secundário */
        if (!line || ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break ;
        }
        if (expand)
            expanded = expand_token(line, msh->env, msh->exit_status);
        else
            expanded = ft_strdup(line);
        free(line);
        write(fd[1], expanded, ft_strlen(expanded));
        write(fd[1], "\n", 1);
        free(expanded);
    }
    close(fd[1]);   /* fechar write end — sinaliza EOF ao leitor */
    return (fd[0]); /* retornar read end para usar como stdin */
}
```

**Heredoc e expansão de variáveis:**
```bash
cat << EOF          # delimitador não quoted → $VAR é expandido
Hello $USER
EOF

cat << 'EOF'        # delimitador quoted → $VAR é literal
Hello $USER
EOF
```

O Lexer deve registar se o delimitador do heredoc estava entre aspas. Esta informação deve ser propagada até à fase de execução do heredoc.

### 3.5 Múltiplos Redirecionamentos

O POSIX permite múltiplos redirecionamentos num mesmo comando. O último tem precedência:

```bash
cat < a < b             # 'b' prevalece como stdin (o 'a' é aberto e imediatamente fechado)
echo test > out > out2  # 'out2' prevalece como stdout
```

```bash
cat < input > output    # stdin=input, stdout=output
```

A ordem de aplicação deve seguir a order em que aparecem na linha de comando, da esquerda para a direita. Na prática, a implementação mais simples é iterar pela lista de redirecionamentos e aplicar cada um (o último dup2 para o mesmo FD "ganha").

### 3.6 Redirecionamentos em Pipelines

Num pipeline, os redirecionamentos afetam apenas o comando a que pertencem:

```bash
cmd1 > out | cmd2
```

- `cmd1` tem stdout redirecionado para `out` — **não** para o pipe
- `cmd2` lê do stdin normal (ou do pipe, dependendo da ordem de aplicação)

> **Ordem de aplicação no filho:** Primeiro configurar os pipes (dup2 para pipe FDs), depois aplicar os redirecionamentos do comando. Os redirecionamentos explícitos **sobrepõem-se** aos implícitos do pipe.

### 3.7 Limitações e Regras — Contexto 42 / Norminette

**Casos testados pelos avaliadores:**

| Teste | Esperado |
|-------|----------|
| `cat < ficheiro_nao_existe` | Erro: `No such file or directory`, `$?≠0` |
| `echo test > /sem/permissao` | Erro: `Permission denied`, `$?≠0` |
| `echo a >> novo.txt` | Cria `novo.txt` se não existir |
| `cat << EOF` (ctrl-D antes de EOF) | Comportar como Bash (terminar heredoc) |
| `echo $HOME << 'DELIM'` ... | `$HOME` no corpo NÃO expandido (delimitador quoted) |
| Heredoc no meio de pipeline | `cat << EOF \| wc -l` deve funcionar |

**Norminette:**
- A função `apply_redirections` deve iterar pela lista e chamar funções auxiliares por tipo
- O `do_heredoc` deve ser uma função separada (é a mais longa)

### 3.8 Funções Permitidas Relevantes

```c
#include <fcntl.h>
int open(const char *pathname, int flags, ...);
/* flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC, O_APPEND */

#include <unistd.h>
int close(int fd);
int dup2(int oldfd, int newfd);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int unlink(const char *pathname);  /* apagar ficheiro temporário, se necessário */

#include <sys/stat.h>
int stat(const char *pathname, struct stat *statbuf);
int access(const char *pathname, int mode);  /* verificar existência/permissões */
```

### 3.9 Metodologia de Aplicação (Pseudocódigo)

```
FUNÇÃO apply_redirections(redirs: lista de t_redir) → int:
    PARA CADA redir EM redirs:
        SE redir.type == REDIR_IN:
            fd = open(redir.file, O_RDONLY)
            SE fd == -1: reportar_erro(redir.file); RETORNAR -1
            dup2(fd, STDIN_FILENO)
            close(fd)
        
        SENÃO SE redir.type == REDIR_OUT:
            fd = open(redir.file, O_WRONLY | O_CREAT | O_TRUNC, 0644)
            SE fd == -1: reportar_erro(redir.file); RETORNAR -1
            dup2(fd, STDOUT_FILENO)
            close(fd)
        
        SENÃO SE redir.type == REDIR_APPEND:
            fd = open(redir.file, O_WRONLY | O_CREAT | O_APPEND, 0644)
            SE fd == -1: reportar_erro(redir.file); RETORNAR -1
            dup2(fd, STDOUT_FILENO)
            close(fd)
        
        SENÃO SE redir.type == HEREDOC:
            fd = do_heredoc(redir.file, redir.expand, msh)
            SE fd == -1: RETORNAR -1
            dup2(fd, STDIN_FILENO)
            close(fd)
    
    RETORNAR 0


/* Nota: apply_redirections é chamada no FILHO, após fork e antes de execve.
   Para heredoc, o do_heredoc deve ser chamado NO PAI antes dos forks,
   para ler o input do utilizador, e o fd do pipe resultante passado ao filho. */
```

**Importante — Heredoc em pipelines:**
O heredoc deve ser **processado antes** dos forks para garantir que o input do utilizador é lido corretamente (o pai é quem corre o `readline`):

```
SEQUÊNCIA PARA PIPELINE COM HEREDOC:
1. Percorrer todos os cmds e seus redirs
2. Para cada HEREDOC encontrado: chamar do_heredoc() → obter fd
3. Guardar o fd no redir para uso posterior
4. Fazer os forks e executar os comandos
5. No filho com heredoc: usar o fd guardado como stdin
```

### 3.10 Metodologia de Testes

```bash
# Teste 1 — Input redirection
echo "conteudo do teste" > input.txt
cat < input.txt                      # deve mostrar "conteudo do teste"

# Teste 2 — Output redirection (truncar)
echo "linha 1" > output.txt
echo "linha 2" > output.txt          # apaga "linha 1"
cat output.txt                       # deve mostrar apenas "linha 2"

# Teste 3 — Output redirection (append)
echo "linha 1" > output.txt
echo "linha 2" >> output.txt
cat output.txt                       # deve mostrar ambas as linhas

# Teste 4 — Heredoc básico
cat << FIM
Hello World
FIM
# deve mostrar "Hello World"

# Teste 5 — Heredoc com expansão
export TEST=42School
cat << FIM
Test: $TEST
FIM
# deve mostrar "Test: 42School"

# Teste 6 — Heredoc sem expansão (quoted delimiter)
cat << 'FIM'
Test: $TEST
FIM
# deve mostrar "Test: $TEST" (literal)

# Teste 7 — Redirecionamento com ficheiro inexistente
cat < nao_existe.txt     # minishell: nao_existe.txt: No such file or directory; $?=1

# Teste 8 — Múltiplos redirecionamentos
cat < input.txt > output2.txt
cat output2.txt          # deve ter o mesmo conteúdo que input.txt
```

---

## 4. Conclusão

### 4.1 Síntese

Os redirecionamentos são implementados através de `open` + `dup2` no processo filho, antes do `execve`. O heredoc é o caso mais complexo, porque requer leitura de input multi-linha no processo pai antes dos forks. A gestão correcta de FDs (fechar após dup2, tratar erros) é essencial para evitar leaks e comportamentos indefinidos.

### 4.2 Armadilhas Comuns

| Pitfall | Descrição | Solução |
|---------|-----------|---------|
| **Não fechar fd após dup2** | O FD original fica aberto | Sempre `close(fd)` após `dup2(fd, STD*_FILENO)` |
| **Heredoc no filho** | Se o heredoc é processado no filho, o pai já passou à frente | Processar heredocs no pai antes dos forks |
| **Expansão em heredoc com delimiter quoted** | `$VAR` expande quando não devia | Registar o flag `expand` quando o delimitador é quoted |
| **Permissões do ficheiro criado** | `open` com modo 0000 cria ficheiro ilegível | Usar modo `0644` para ficheiros de output |
| **Múltiplos `>` no mesmo comando** | Apenas o stdout deve ser afetado | Aplicar todos (cada dup2 substitui o anterior) — ambos os ficheiros são criados |

---

## 5. Bibliografia

### Documentação Oficial

- `man 2 open`, `man 2 dup2`, `man 2 close`, `man 2 read`, `man 2 write`
- [POSIX.1-2017 — §2.7 Redirection](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_07)
- [POSIX.1-2017 — §2.7.4 Here-Document](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_07_04)

### Recursos Web

- **How-To Geek** — *How to Use Here Documents in Bash on Linux*  
  https://www.howtogeek.com/719058/how-to-use-here-documents-in-bash-on-linux/
- **GNU Bash Manual** — §3.6 Redirections  
  https://www.gnu.org/software/bash/manual/bash.html#Redirections

---

*Documento gerado no contexto do projeto Minishell — Escola 42 Luanda*  
*Data: Março 2026*
