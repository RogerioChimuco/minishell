#ifndef TYPES_H
# define TYPES_H

# include "minishell.h"

/* ========================================================================== */
/* ENUMERAÇÕES                                 */
/* ========================================================================== */

typedef enum s_token_type
{
	WORD,
	PIPE,
	HEREDOC,
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND
}	t_token_type;

typedef enum s_quote_state
{
	NO_QUOTES,
	SINGLE_QUOTES,
	DOUBLE_QUOTES
}	t_quote_state;

/* ========================================================================== */
/* ESTRUTURAS BASE                                */
/* ========================================================================== */

typedef struct s_token
{
	char			*value;
	char			*original_value;
	int				exp;
	t_token_type	type;
	t_quote_state	quote_state;
}	t_token;

typedef struct s_redir
{
	int				fd;
	int				amb;
	char			*value;
	char			*original_value;
	t_token_type	type;
}	t_redir;

typedef struct s_env
{
	int				exported;
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* ========================================================================== */
/* ESTRUTURAS DE COMANDO                            */
/* ========================================================================== */

typedef struct s_command
{
	int				index;
	int				is_builtin;
	char			*path;
	char			**args;
	t_list			*redir;
}	t_command;

typedef struct s_split
{
	t_token			*token;
	t_list			*node;
	t_list			*next;
	char			**words;
}	t_split;

/* ========================================================================== */
/* ESTRUTURA PRINCIPAL                            */
/* ========================================================================== */

typedef struct s_shell
{
	int				line_count;
	int				exit_status;
	int				cmd_i;
	int				pid_i;
	int				error;
	int				length;
	int				(*pipes)[2];
	int				pipe_lineno[2];
	char			*prompt;
	char			*input;
	char			**envp;
	t_list			*tokens;
	t_list			*cmds;
	pid_t			*pids;
	t_env			*vars;
}	t_shell;

#endif
