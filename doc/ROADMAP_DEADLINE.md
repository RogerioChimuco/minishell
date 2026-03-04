# Roadmap & Deadline — Projeto Minishell
### Grupo: **rochimuc** & **luqalmei** | Escola 42 Luanda

---

## Visão Geral

| Parâmetro | Valor |
|-----------|-------|
| **Projeto** | Minishell |
| **Equipa** | rochimuc + luqalmei |
| **Total de horas** | 210 h |
| **Ritmo diário** | 10 h/dia |
| **Duração total** | **21 dias** |
| **Data de início** | 04 de Março de 2026 |
| **Data limite (deadline)** | **24 de Março de 2026** |

---

## Estrutura de Trabalho em Pares

O projeto divide-se em **dois eixos paralelos** que convergem na fase de integração:

```
rochimuc  → Foco: Frontend (Lexer, Parser, Expansão, Builtins)
luqalmei  → Foco: Backend  (Executor, Processos, Pipes, Redirecionamentos)
```

Ambos partilham responsabilidade em: **Sinais, Readline, Integração, Testes**.

---

## Calendário Detalhado — 21 Dias

### FASE 1 — Fundação (Dias 1–2) `04–05 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **1** | 10h | Leitura do Subject + Normas da 42. Ler docs M01, M02, M03. Setup do repositório Git (Makefile, estrutura de pastas, libft). | Leitura do Subject + Normas da 42. Ler docs M04, M05, M06. Setup do ambiente (compilers, Valgrind, readline lib). |
| **2** | 10h | Definir e implementar as structs principais: `t_msh`, `t_token`, `t_ast`, `t_cmd_table`, `t_cmd`, `t_redir` em `structs.h`. | Implementar funções utilitárias de ambiente: `ft_dup_envp`, `env_get`, `env_set`, `env_remove`, `env_list_to_array`. |

**Entregável Fase 1:**
- [ ] Repositório Git funcional com Makefile e libft integrada
- [ ] `structs.h` com todas as structs definidas e aprovadas por ambos
- [ ] Funções de gestão de ambiente completas e testadas

---

### FASE 2 — Lexer (Dias 3–5) `06–08 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **3** | 10h | Implementar `ft_isspace`, `ft_isoperator`. Início do tokenizer: reconhecer operadores `\|`, `<`, `>`, `>>`, `<<`, `;`. | Implementar o loop principal `readline` + `add_history`. Função `clean_and_exit` com `rl_clear_history`. |
| **4** | 10h | Implementar estado de quoting: single-quote `'`, double-quote `"`. Extração de tokens WORD com aspas. | Implementar a verificação `is_unclosed_quote(input)` + mensagem de erro. Inicio do `ft_split` por PATH se não existir na libft. |
| **5** | 10h | Integrar lexer completo: `tokenize(input)` → `t_list` de tokens. Testes unitários do lexer com Valgrind. | Testes do lexer em paralelo: testar casos de aspas, operadores compostos (`>>`, `<<`), pipes consecutivos. |

**Entregável Fase 2:**
- [ ] `tokenize()` converte qualquer string de input válida para lista de tokens
- [ ] Aspas simples e duplas correctamente tratadas
- [ ] Detecção de aspas não fechadas com mensagem de erro
- [ ] 0 leaks no módulo do lexer (Valgrind)

---

### FASE 3 — Parser & AST (Dias 6–8) `09–11 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **6** | 10h | Implementar `parse_simple_command`: recolher `argv[]` + redirecionamentos. | Implementar `parse_redirection`: consumir operador + filename, criar `t_redir`. |
| **7** | 10h | Implementar `parse_pipeline`: iterar comandos separados por PIPE, construir `t_cmd_table`. | Implementar `parse_list`: iterar pipelines separadas por `;`, construir `t_ast`. |
| **8** | 10h | Testar o parser completo. Implementar `free_ast(t_ast*)` para libertar toda a estrutura. Função `debug_print_ast()`. | Testar erros sintáticos: `\|`, `>` sem filename, `\| \|`. Garantir mensagens de erro correctas e `$?` actualizado. |

**Entregável Fase 3:**
- [ ] `parse(token_list)` → `t_ast` correcta para qualquer input válido
- [ ] Erros sintáticos detectados e reportados correctamente
- [ ] `free_ast()` sem leaks
- [ ] `debug_print_ast()` funcional para facilitar debug nas fases seguintes

---

### FASE 4 — Expansão & Builtins Simples (Dias 9–11) `12–14 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **9** | 10h | Implementar `expand_token()`: expansão de `$VAR`, `$?`, variável não existente = `""`. Quote removal. | Implementar `resolve_path()`: pesquisa no `PATH` com `access(path, X_OK)`. Casos: caminho absoluto, relativo, não encontrado. |
| **10** | 10h | Implementar builtins: `echo [-n]`, `pwd`. Testes + Valgrind. | Implementar builtins: `env`, `export` (com e sem args), `unset`. Testes + Valgrind. |
| **11** | 10h | Implementar builtins: `cd` (com `HOME`, `OLDPWD`, `-`), `exit` (com validação numérica e `too many args`). | Integrar `expand_ast()`: percorrer toda a `t_ast` e expandir tokens. Testar `echo $HOME`, `echo $?`, `echo '$HOME'`. |

**Entregável Fase 4:**
- [ ] Expansão `$VAR` e `$?` correcta dentro e fora de aspas duplas
- [ ] Todos os 7 builtins implementados e testados individualmente
- [ ] `cd` actualiza `PWD` e `OLDPWD` correctamente
- [ ] `exit` trata todos os casos de erro

---

### FASE 5 — Executor: Processos (Dias 12–13) `15–16 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **12** | 10h | Implementar `execute_external_cmd()`: fork + execve + waitpid. Testar com `/bin/ls`, `/bin/echo`. | Implementar `update_exit_status()` com `WIFEXITED`/`WIFSIGNALED`. Testar `$?` após vários comandos. |
| **13** | 10h | Integrar lógica: builtins sem pipeline executam no pai; externos fazem fork. Testar: `ls`, `cat`, `grep`, `wc`. | Implementar tratamento de erros: `command not found` (127), `Permission denied`, executável que não existe. |

**Entregável Fase 5:**
- [ ] Comandos externos executam correctamente com `argv` e `envp`
- [ ] `$?` correctamente atualizado após cada comando
- [ ] Nenhum processo zombie (verificar com `ps` durante execução)

---

### FASE 6 — Pipes & File Descriptors (Dias 14–15) `17–18 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **14** | 10h | Implementar `init_pipes()`: criar N-1 pipes para pipeline de N comandos. | Implementar `child_setup_fds()`: `dup2` correcto para cada filho da pipeline (stdin do pipe anterior, stdout para o seguinte). |
| **15** | 10h | Implementar `execute_pipeline()`: loop de forks, `close_all_pipes()` no pai e nos filhos, `waitpid` de todos os PIDs. | Testar pipelines: `ls \| cat`, `ls \| grep .c \| wc -l`, `echo hello \| cat -n`. Verificar deadlocks e FD leaks com Valgrind `--track-fds=yes`. |

**Entregável Fase 6:**
- [ ] Pipelines de 2, 3 e N comandos funcionais
- [ ] Nenhum deadlock (write ends fechados no pai e em todos os filhos)
- [ ] 0 FD leaks reportados pelo Valgrind

---

### FASE 7 — Redirecionamentos & Heredoc (Dias 16–17) `19–20 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **16** | 10h | Implementar `apply_redirections()`: `REDIR_IN` (`open` + `dup2`), `REDIR_OUT` (`O_TRUNC`), `REDIR_APPEND` (`O_APPEND`). | Implementar `do_heredoc()`: criar pipe, ler linhas com `readline("> ")`, escrever no write end, retornar read end. |
| **17** | 10h | Integrar redirecionamentos: chamados no filho após fork, antes do execve. Testar `<`, `>`, `>>`. | Integrar heredoc: detectar se o delimitador é quoted → não expandir `$`. Testar `<< EOF` com e sem expansão. |

**Entregável Fase 7:**
- [ ] `<`, `>`, `>>` funcionais com tratamento de erros (`No such file or directory`, etc.)
- [ ] `<<` heredoc com e sem expansão de variáveis
- [ ] Redirecionamentos combinados com pipelines: `ls \| cat > out.txt`

---

### FASE 8 — Sinais (Dia 18) `21 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **18** | 10h | Implementar `setup_signals_parent()` com `sigaction`: SIGINT capturado (nova linha + redisplay), SIGQUIT ignorado. Handler acede à 1 variável global permitida. | Implementar `setup_signals_child()`: restaurar SIG_DFL antes do execve. Testar Ctrl-C durante `sleep 10`, Ctrl-\\ no prompt. |

**Entregável Fase 8:**
- [ ] Ctrl-C no prompt: nova linha + novo prompt + `$?=130`
- [ ] Ctrl-\\ no prompt: nada (ignorado)
- [ ] Ctrl-D no prompt: `exit` + saída limpa
- [ ] Ctrl-C durante processo filho: termina o filho, não o shell

---

### FASE 9 — Integração Total (Dia 19) `22 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **19** | 10h | Juntar todos os módulos no loop principal. Garantir fluxo correcto: `readline → tokenize → parse → expand → execute`. Testar sessão completa interactiva: pipelines + builtins + redirecionamentos + sinais juntos. | Rever `clean_and_exit()`: libertar toda a memória (`free_ast`, `free_token_list`, `env_list_clear`, `rl_clear_history`). Executar sessão longa com Valgrind — 0 leaks. |

**Entregável Fase 9:**
- [ ] Shell completamente funcional em modo interativo
- [ ] Sessão de 20 comandos variados sem crash, leak, ou zombie
- [ ] `ctrl-C`, `ctrl-D`, `ctrl-\\ ` a funcionar em todos os contextos

---

### FASE 10 — Norminette, Testes & Polimento (Dias 20–21) `23–24 Mar`

| Dia | Horas | rochimuc | luqalmei |
|-----|-------|----------|----------|
| **20** | 10h | Passar **Norminette** em todos os ficheiros `.c` e `.h`. Corrigir: funções > 25 linhas, > 4 parâmetros, vars declaradas no meio da função, `for` loops. | Executar **bateria de testes de evaluação** (ver secção abaixo). Registar todos os bugs encontrados. Corrigir os prioritários. |
| **21** | 10h | Revisão final de código conjunta. Verificar: Makefile (`all`, `clean`, `fclean`, `re`), norminette 0 erros, Valgrind 0 leaks. | Testes de stress: heredoc longo, pipeline de 10 comandos, Ctrl-C rápido repetido. Preparar para a avaliação. |

**Entregável Fase 10:**
- [ ] Norminette: **0 erros** em todos os ficheiros
- [ ] Valgrind: **0 leaks** (definitely lost = 0, indirectly lost = 0)
- [ ] Makefile: todas as regras funcionais
- [ ] Projeto pronto para submissão e avaliação

---

## Distribuição de Responsabilidades por Módulo

| Módulo | Responsável Principal | Responsável de Revisão |
|--------|-----------------------|------------------------|
| Estruturas (`structs.h`) | Ambos (decisão conjunta) | Ambos |
| Lexer / Tokenizador | **rochimuc** | luqalmei |
| Parser / AST | **rochimuc** | luqalmei |
| Expansão de variáveis | **rochimuc** | luqalmei |
| Gestão do Ambiente (`env_*`) | **luqalmei** | rochimuc |
| Executor (fork/exec/wait) | **luqalmei** | rochimuc |
| Pipes & File Descriptors | **luqalmei** | rochimuc |
| Redirecionamentos (`<`, `>`, `>>`) | **luqalmei** | rochimuc |
| Heredoc (`<<`) | **luqalmei** | rochimuc |
| Sinais | Ambos (par programming) | Ambos |
| Builtins (`echo`, `pwd`) | **rochimuc** | luqalmei |
| Builtins (`cd`, `exit`) | **rochimuc** | luqalmei |
| Builtins (`export`, `unset`, `env`) | **luqalmei** | rochimuc |
| `readline` / histórico | **rochimuc** | luqalmei |
| Integração & `clean_and_exit` | Ambos | Ambos |
| Norminette | Cada um no seu código | O outro revisa |
| Testes finais | Ambos | — |

---

## Milestones de Controlo

| Milestone | Dia | Data | Critério de Sucesso |
|-----------|-----|------|---------------------|
| **M1 — Fundação** | Fim dia 2 | 05 Mar | structs.h aprovada; env functions completas |
| **M2 — Lexer OK** | Fim dia 5 | 08 Mar | `echo "hello \| world"` tokeniza correctamente; 0 leaks |
| **M3 — Parser OK** | Fim dia 8 | 11 Mar | `ls -la \| grep .c > out` gera AST correcta; erros sintáticos tratados |
| **M4 — Builtins OK** | Fim dia 11 | 14 Mar | Todos os 7 builtins passam os testes da secção §3.12 do doc 09 |
| **M5 — Executor OK** | Fim dia 13 | 16 Mar | `ls`, `cat ficheiro`, `grep pattern ficheiro` executam com `$?` correcto |
| **M6 — Pipes OK** | Fim dia 15 | 18 Mar | `ls \| grep test \| wc -l` funciona; 0 FD leaks; nenhum deadlock |
| **M7 — Redirs OK** | Fim dia 17 | 20 Mar | `<`, `>`, `>>`, `<<` todos funcionais; heredoc com/sem expansão |
| **M8 — Sinais OK** | Fim dia 18 | 21 Mar | Ctrl-C/D/\\ com comportamento idêntico ao bash |
| **M9 — Integração** | Fim dia 19 | 22 Mar | Sessão interactiva completa sem crash, leak ou zombie |
| **M10 — Entrega** | Fim dia 21 | 24 Mar | Norminette 0 erros; Valgrind 0 leaks; avaliação aprovada |

---

## Bateria de Testes para Avaliação (Checklist)

### Builtins
```bash
echo "hello world"
echo -n "sem newline"
echo $HOME
echo '$HOME'
cd /tmp && pwd
cd - && pwd
export TEST=42 && echo $TEST
unset TEST && echo $TEST
env | grep PATH
exit 0
```

### Pipes
```bash
ls | cat
ls /bin | grep ls | wc -l
echo hello | cat | cat | cat
cat /dev/urandom | head -c 20 | xxd
```

### Redirecionamentos
```bash
echo "conteudo" > /tmp/test.txt
cat < /tmp/test.txt
echo "mais" >> /tmp/test.txt
cat /tmp/test.txt
cat << EOF
linha 1
linha 2
EOF
cat << 'EOF'
$HOME nao expande
EOF
```

### Expansão
```bash
echo $HOME
echo $USER
echo $?
ls /nao_existe; echo $?
echo "$HOME is home"
echo '$HOME is literal'
```

### Sinais
```bash
# Ctrl-C no prompt         → novo prompt, $?=130
# Ctrl-\ no prompt         → nada
# Ctrl-D no prompt         → sair
# Ctrl-C com sleep 10      → termina sleep, shell continua
```

### Edge Cases
```bash
echo ""
echo ''
cat < ficheiro_nao_existe
echo > /sem/permissao
|
>
echo test |
```

---

## Metodologia de Trabalho Diário

Cada sessão de **10 horas** segue obrigatoriamente este ciclo de 5 etapas:

```
╔══════════════════════════════════════════════════════════════════╗
║              CICLO DIÁRIO — 10 HORAS POR PESSOA                  ║
╠══════╦═══════════════╦══════════════════════════════════════════╣
║ Etapa║ Tempo         ║ Actividade                               ║
╠══════╬═══════════════╬══════════════════════════════════════════╣
║  1   ║  ~1h30        ║  ESTUDAR                                 ║
║      ║               ║  Ler o(s) doc(s) da fase actual (pasta   ║
║      ║               ║  doc/). Consultar man pages relevantes.  ║
║      ║               ║  Compreender teoria antes de codificar.  ║
╠══════╬═══════════════╬══════════════════════════════════════════╣
║  2   ║  ~5h          ║  IMPLEMENTAR & TESTAR                    ║
║      ║               ║  Escrever o código do módulo do dia.     ║
║      ║               ║  Compilar após cada função nova.         ║
║      ║               ║  Testar manualmente os casos básicos.    ║
║      ║               ║  Correr Valgrind regularmente.           ║
╠══════╬═══════════════╬══════════════════════════════════════════╣
║  3   ║  ~1h30        ║  VALIDAR                                 ║
║      ║               ║  Correr Norminette → 0 erros.            ║
║      ║               ║  Correr Valgrind → 0 leaks.              ║
║      ║               ║  Testar edge cases da checklist do doc.  ║
║      ║               ║  Só avançar se esta etapa passar.        ║
╠══════╬═══════════════╬══════════════════════════════════════════╣
║  4   ║  ~1h          ║  COMPARTILHAR & APLICAR                  ║
║      ║               ║  Explicar ao parceiro o que foi feito.   ║
║      ║               ║  Parceiro testa o código no seu ambiente.║
║      ║               ║  Discutir melhorias e decisões de design.║
║      ║               ║  Rever o plano do dia seguinte juntos.   ║
╠══════╬═══════════════╬══════════════════════════════════════════╣
║  5   ║  ~1h          ║  GIT: ADD → COMMIT → PUSH                ║
║      ║               ║  Cada um faz push na SUA branch pessoal. ║
║      ║               ║  (ver convenção abaixo)                  ║
╚══════╩═══════════════╩══════════════════════════════════════════╝
```

### Fluxo Git do Final de Cada Dia

**rochimuc** — executa sempre na sua branch:
```bash
# 1. Verificar o estado
git status

# 2. Adicionar APENAS os ficheiros do seu módulo
git add src/lexer.c src/parser.c includes/structs.h  # (exemplo)

# 3. Commit com formato padronizado
git commit -m "[módulo] descrição breve do que foi feito"
# Exemplos:
#   git commit -m "[lexer] add single quote and double quote handling"
#   git commit -m "[builtins] implement echo with -n flag"
#   git commit -m "[norm] fix functions over 25 lines in lexer.c"

# 4. Push na branch pessoal
git push origin rochimuc
```

**luqalmei** — executa sempre na sua branch:
```bash
git status
git add src/executor.c src/pipes.c src/env.c  # (exemplo)
git commit -m "[módulo] descrição breve do que foi feito"
git push origin luqalmei
```

### Convenção de Branches

```
main              → código estável; merge apenas após Milestone aprovado
dev               → branch de integração; merge das branches individuais
rochimuc          → branch pessoal do rochimuc  ← push diário aqui
luqalmei          → branch pessoal do luqalmei  ← push diário aqui
```

### Quando Fundir (Merge) para `dev`

```
Condição para merge rochimuc → dev:
  ✓ Norminette 0 erros nos ficheiros alterados
  ✓ Valgrind 0 leaks no módulo
  ✓ Parceiro testou e aprovou
  ✓ Milestone do módulo concluído

Condição para merge dev → main:
  ✓ Todos os módulos da fase integrados
  ✓ Testes de integração passam
  ✓ Ambos aprovam
```

### Formato de Commit (Obrigatório)

```
[módulo] verbo + descrição (máximo 50 caracteres)

Módulos válidos:
  lexer     parser    expand    builtins
  executor  pipes     redir     signals
  readline  structs   env       norm
  makefile  fix       test

Exemplos corretos:
  [lexer] add heredoc delimiter detection
  [pipes] fix write end not closed in parent process
  [builtins] implement cd with OLDPWD support
  [norm] split parse_cmd into helper functions
  [fix] free token list on parse error
  [test] add edge cases for unclosed quotes

Exemplos errados:
  "update"              ← sem módulo, sem descrição
  "fix bug"             ← vago
  "[lexer] added the implementation of the lexer for handling..."
                        ← demasiado longo
```

---

## Regras de Colaboração Git

```
Setup inicial do repositório (fazer uma vez):

  git clone <url-do-repo>
  git checkout -b rochimuc    # rochimuc cria a sua branch
  git push -u origin rochimuc

  git checkout -b luqalmei    # luqalmei cria a sua branch
  git push -u origin luqalmei

Dia-a-dia:
  Cada pessoa trabalha SEMPRE na sua branch.
  Nunca fazer commit directo em main ou dev.

Sincronização com o trabalho do parceiro:
  git fetch origin            # ver o que o parceiro fez
  git log origin/luqalmei     # ver commits do luqalmei (exemplo)
  git diff origin/luqalmei    # ver diferenças

Merge para dev (nos dias de Milestone):
  git checkout dev
  git merge rochimuc          # após aprovação do parceiro
  git merge luqalmei
  git push origin dev
```

---

## Distribuição de Horas por Fase

```
Fase 1 — Fundação          :  20h  ( 2 dias) ████
Fase 2 — Lexer             :  30h  ( 3 dias) ██████
Fase 3 — Parser & AST      :  30h  ( 3 dias) ██████
Fase 4 — Expansão+Builtins :  30h  ( 3 dias) ██████
Fase 5 — Executor          :  20h  ( 2 dias) ████
Fase 6 — Pipes & FDs       :  20h  ( 2 dias) ████
Fase 7 — Redirecionamentos :  20h  ( 2 dias) ████
Fase 8 — Sinais            :  10h  ( 1 dia ) ██
Fase 9 — Integração        :  10h  ( 1 dia ) ██
Fase 10— Norminette+Testes :  20h  ( 2 dias) ████
─────────────────────────────────────────────────
TOTAL                      : 210h  (21 dias)
```

---

## Notas Finais

> **Prioridade máxima:** Nunca deixar Valgrind com `definitely lost` ou `indirectly lost`. Corrigir leaks imediatamente quando encontrados, na mesma sessão.

> **Regra de sincronia diária:** No início de cada sessão de 10h, 15 minutos de stand-up: *o que fiz ontem? o que faço hoje? há algum bloqueio?* — cada um faz `git pull origin <branch_do_parceiro>` para ver o progresso do outro antes de começar.

> **Buffer de segurança:** O dia 21 (24 Mar) é exclusivamente para polimento. Se tudo estiver pronto no dia 20, usem o dia 21 para testes adicionais e bonus (se tiverem tempo).

> **Bonus (se o tempo permitir):**  `&&`, `||`, subshells `(...)`. Só iniciar o bonus DEPOIS de todos os critérios obrigatórios passarem com Norminette + Valgrind limpos.

---

*Roadmap criado em 04 de Março de 2026 — Escola 42 Luanda*  
*Equipa: rochimuc & luqalmei | Minishell Project*
