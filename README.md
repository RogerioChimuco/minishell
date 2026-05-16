*Este projeto foi criado como parte do currículo da 42 por ahuanga, ckulembe.*

# 🐚 Minishell

> Uma reimplementação minimalista do Bash, desenvolvida em C como projeto académico para explorar conceitos fundamentais de sistemas operativos.

---

## 📋 Descrição

O **Minishell** é um interpretador de comandos desenvolvido inteiramente em C, inspirado no comportamento do **Bash (Bourne Again Shell)**. O projeto foi concebido com o objetivo de compreender em profundidade como um shell funciona internamente — desde a leitura de uma linha de comando até à sua execução por processos filhos.

O shell implementa um pipeline de processamento composto por quatro etapas principais:

1. **Leitura do input** — Captura a linha de comando do utilizador via `readline`, com suporte a histórico de sessão.
2. **Lexer** — Tokeniza a linha de entrada, classifica cada token (palavra, pipe, redirecionamento, heredoc) e verifica a sintaxe antes de prosseguir.
3. **Expansão e Parse** — Expande variáveis de ambiente (`$VAR`, `$?`) e aspas, depois constrói uma lista de comandos estruturados.
4. **Executor** — Executa os comandos, gerindo pipes entre processos, redirecionamentos de I/O e builtins.

```
Input → Lexer (tokenize → classify → syntax check) → Expansion → Parser → Executor
```

![Pipeline de processamento de comandos](pipeline_diagram.png)

O projeto foi desenvolvido a dois — **ahuanga** e **ckulembe** — e segue normas estritas de qualidade de código (flags `-Wall -Wextra -Werror`), sem uso de qualquer biblioteca externa além da `readline` e da `libft` própria.

---

## ⚙️ Instruções

### Dependências

Antes de compilar, certifica-te de que tens instalado:

- **GCC** ou compatível (`cc`)
- **GNU Readline** (`libreadline-dev` no Debian/Ubuntu)
- **Make**

```bash
# Ubuntu / Debian
sudo apt install build-essential libreadline-dev

# macOS (via Homebrew)
brew install readline
```

### Compilação

```bash
# Clonar o repositório
git clone git@vogsphere.42luanda.com:vogsphere/intra-uuid-ad8e154e-5ae5-4502-aff3-f11599379600-7084258-ahuanga minishell


# Compilar o projeto 
cd minishell
make
```

Targets disponíveis no Makefile:

| Comando     | Descrição                                      |
|-------------|------------------------------------------------|
| `make`      | Compila o projeto e gera o executável `minishell` |
| `make clean`| Remove os ficheiros objeto (`.o`)              |
| `make fclean` | Remove objetos e o executável final          |
| `make re`   | Recompila tudo do zero (`fclean` + `all`)      |

### Execução

```bash
# Iniciar o minishell
./minishell

# Modo não-interativo (leitura de ficheiro)
./minishell < script.sh
```

O prompt é exibido automaticamente em modo interativo. Em modo não-interativo (stdin redirecionado), o shell lê linha a linha com `get_next_line`.

---

## 🚀 Funcionalidades

### Builtins implementados

| Builtin   | Comportamento                                                  |
|-----------|----------------------------------------------------------------|
| `echo`    | Imprime argumentos no stdout; suporta flag `-n`               |
| `cd`      | Muda o diretório atual; atualiza `PWD` e `OLDPWD`            |
| `pwd`     | Imprime o diretório de trabalho atual                         |
| `export`  | Declara e exporta variáveis de ambiente                       |
| `unset`   | Remove variáveis de ambiente                                  |
| `env`     | Lista as variáveis de ambiente exportadas                     |
| `exit`    | Termina o shell com código de saída opcional                  |

### Pipes e redirecionamentos

```bash
ls -la | grep ".c" | wc -l        # pipeline múltiplo
echo "hello" > output.txt         # redirecionamento de output
cat < input.txt                   # redirecionamento de input
echo "line" >> file.txt           # append
cat << EOF                        # heredoc
> conteúdo
> EOF
```

### Expansão de variáveis

```bash
echo $HOME          # variável de ambiente
echo $?             # código de saída do último comando
echo "$USER world"  # expansão dentro de aspas duplas
echo '$USER'        # sem expansão em aspas simples
```

### Sinais

| Sinal      | Comportamento                                      |
|------------|----------------------------------------------------|
| `Ctrl+C`   | Interrompe o comando atual; exibe novo prompt      |
| `Ctrl+D`   | Encerra o shell (EOF)                              |
| `Ctrl+\`   | Ignorado (como no Bash)                            |

### Histórico

O minishell mantém histórico de comandos na sessão atual, navegável com as teclas ↑ e ↓.

---

## 🧱 Arquitetura

O projeto está organizado em módulos independentes:

```
minishell/
├── main.c                  # Loop principal do shell
├── minishell.h             # Header global e inclusões
├── types.h                 # Definição de tipos (t_shell, t_token, t_command, t_env, ...)
├── ft_validate_quotes.c    # Validação de aspas antes da tokenização
├── tokens/                 # Lexer: tokenização e classificação
├── parse/                  # Parser: verificação de sintaxe e construção de comandos
├── expansion/              # Expansão de variáveis e aspas
├── executor/               # Execução: pipes, redirecionamentos, fork, execve
├── builtin/                # Implementação dos comandos internos
├── utils/                  # Prompt, sinais, histórico, variáveis de ambiente
├── get_next_line/          # Leitura de ficheiros em modo não-interativo
└── libft/                  # Biblioteca utilitária própria
```

---

## 💡 Exemplos de uso

```bash
# Variáveis e expansão
export NAME="World"
echo "Hello $NAME"       # Hello World
echo "Exit: $?"          # Exit: 0

# Pipeline com builtins e externos
cat /etc/passwd | grep root | awk -F: '{print $1}'

# Redirecionamentos combinados
sort < lista.txt > lista_ordenada.txt

# Heredoc
cat << FIM
Linha 1
Linha 2
FIM

# Aspas mistas
echo "Valor de HOME: $HOME"    # expande
echo 'Valor de HOME: $HOME'    # não expande
```

---

## 📚 Referências

### Documentação e especificações

- [**POSIX Shell Command Language**](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html) — Especificação oficial do comportamento de shells POSIX.
- [**GNU Bash Manual**](https://www.gnu.org/software/bash/manual/bash.html) — Referência completa do Bash, usada como modelo de comportamento.
- [**GNU Readline Library**](https://tiswww.case.edu/php/chet/readline/rltop.html) — Documentação da biblioteca usada para leitura interativa.
- [**Linux man-pages: fork(2), execve(2), waitpid(2), pipe(2)**](https://man7.org/linux/man-pages/index.html) — Chamadas de sistema fundamentais para a execução de processos.
- [**Linux man-pages: signal(7)**](https://man7.org/linux/man-pages/man7/signal.7.html) — Referência para o tratamento de sinais POSIX.

---

## 🤖 Utilização de IA

Durante o desenvolvimento do projeto, ferramentas de IA (nomeadamente o **Claude** da Anthropic e o **ChatGPT**) foram utilizadas como apoio em tarefas específicas:

| Tarefa | Ferramenta | Partes do projeto |
|--------|------------|-------------------|
| Compreensão de comportamentos do Bash (ex: expansão de `$?`, aspas aninhadas) | Claude / ChatGPT | `expansion/`, `ft_validate_quotes.c` |
| Depuração de lógica de pipes e file descriptors | Claude | `executor/pipes.c`, `executor/child.c` |
| Esclarecimento de comportamento de sinais (`SIGINT`, `SIGQUIT`) em processos filhos | Claude | `utils/handler_signal.c` |
| Revisão de casos limite em redirecionamentos | ChatGPT | `executor/redirects.c`, `executor/redir_utils.c` |
| Apoio na redação deste README | Claude | — |

A IA **não foi utilizada** para gerar código de forma direta. Todo o código foi escrito e revisto manualmente pelos autores. As ferramentas serviram como recurso de consulta, equivalente a uma documentação interativa.

---

## 👥 Autores

| Utilizador   | Contribuições principais                          |
|--------------|---------------------------------------------------|
| **ahuanga**  | Builtins, expansão, sinais, prompt, revisão geral |
| **ckulembe** | Lexer, parser, executor, estrutura do projeto     |
