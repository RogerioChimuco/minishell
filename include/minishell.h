
#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>

/*
** Token types recognized by the lexer
*/
typedef enum e_tok
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_REDIR_APPEND,
	TOK_HEREDOC,
	TOK_EOF
}	t_tok;

/*
** A single token produced by the lexer
*/
typedef struct s_token
{
	t_tok			kind;
	char			*raw;
	struct s_token	*next;
}	t_token;

/*
** Linked list of redirections attached to a command node
*/
typedef struct s_redir
{
	t_tok			kind;
	char			*target;
	struct s_redir	*next;
}	t_redir;

/*
** A command node in the pipeline
*/
typedef struct s_cmd
{
	char			**argv;
	int				argc;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

/*
** Environment variable entry (linked list)
*/
typedef struct s_env
{
	char			*key;
	char			*val;
	int				exported;
	struct s_env	*next;
}	t_env;

/*
** Top-level shell context
*/
typedef struct s_msh
{
	char	*line;
	char	*prompt;
	t_token	*tokens;
	t_cmd	*pipeline;
	t_env	*env;
	int		status;
}	t_msh;

extern volatile sig_atomic_t	g_sig;

/* env.c */
t_env	*env_build(char **envp);
void	env_free(t_env **lst);
char	*env_get(t_env *env, const char *key);
int		env_set(t_env **env, const char *key, const char *val, int exported);

/* lexer.c */
t_token	*lexer_run(const char *line);
void	tokens_free(t_token **lst);

/* parser.c */
t_cmd	*parser_run(t_token *toks);
void	pipeline_free(t_cmd **head);

/* prompt.c */
char	*prompt_build(t_env *env);

/* signal.c */
void	signals_interactive(void);
void	signals_exec(void);

/* syntax.c */
int		syntax_quotes_ok(const char *line);
int		syntax_tokens_ok(t_token *toks);

/* utils.c */
char	*msh_strdup(const char *s);
char	*msh_strjoin(const char *a, const char *b);
void	msh_free(void **p);
int		msh_isspace(char c);

#endif
