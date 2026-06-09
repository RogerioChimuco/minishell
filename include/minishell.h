/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:59:12 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 14:59:18 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"

/*
** Token types produced by the lexer
*/
typedef enum e_tok
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_APPEND,
	TOK_HEREDOC,
	TOK_EOF
}	t_tok_type;

/*
** Quote context tracked while scanning
*/
typedef enum e_qstate
{
	Q_NONE,
	Q_SINGLE,
	Q_DOUBLE
}	t_qstate;

/*
** A single lexical token
*/
typedef struct s_token
{
	t_tok_type		type;
	t_qstate		quote;
	char			*raw;
	char			*val;
}	t_token;

/*
** A linked list node (generic)
*/
typedef struct s_node
{
	void			*data;
	struct s_node	*next;
}	t_node;

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
** Redirect descriptor attached to a command
*/
typedef struct s_redir
{
	t_tok_type	kind;
	char		*target;
	char		*raw_target;
	int			ambiguous;
}	t_redir;

/*
** A parsed command ready for execution
*/
typedef struct s_cmd
{
	char		**argv;
	int			argc;
	t_node		*redirs;
	char		*path;
	int			builtin;
	int			idx;
}	t_cmd;

/*
** Central shell state passed everywhere
*/
typedef struct s_shell
{
	char		*prompt;
	char		*line;
	t_node		*tokens;
	t_node		*cmds;
	t_env		*env;
	char		**envp;
	int			exit_code;
	int			ncmds;
	int			(*pipes)[2];
	pid_t		*pids;
	int			npids;
}	t_shell;

extern volatile sig_atomic_t	g_sig;

/* shell lifecycle */
void	ms_init(t_shell *sh, char **envp);
void	ms_reset(t_shell *sh);
void	ms_destroy(t_shell *sh);

/* signal handling */
void	sig_interactive(void);
void	sig_exec(void);
void	sig_handler(int s);

/* input */
char	*ms_readline(const char *prompt);
int		ms_validate(const char *line);

/* env helpers */
t_env	*env_build(char **envp);
char	*env_get(t_env *env, const char *key);
void	env_set(t_env **env, const char *key, char *val);
void	env_free(t_env **env);
char	**env_to_array(t_env *env);

/* list helpers */
t_node	*node_new(void *data);
void	node_push_back(t_node **head, t_node *n);
int		node_len(t_node *head);
void	node_free_all(t_node **head, void (*del)(void *));

/* prompt builder */
void	build_prompt(t_shell *sh);

#endif
