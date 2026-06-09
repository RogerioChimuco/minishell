/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:02:00 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 15:02:06 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../include/minishell.h"

/*
** free_matrix — free a NULL-terminated array of strings.
** Used to destroy the envp array produced by env_to_array.
*/
void	free_matrix(char **m)
{
	int	i;

	if (!m)
		return ;
	i = 0;
	while (m[i])
		free(m[i++]);
	free(m);
}

/*
** free_token — destructor for t_token, called through node_free_all.
*/
void	free_token(void *ptr)
{
	t_token	*tok;

	if (!ptr)
		return ;
	tok = (t_token *)ptr;
	free(tok->raw);
	free(tok->val);
	free(tok);
}

/*
** free_cmd — destructor for t_cmd, called through node_free_all.
*/
void	free_cmd(void *ptr)
{
	t_cmd	*cmd;

	if (!ptr)
		return ;
	cmd = (t_cmd *)ptr;
	if (cmd->argv)
		free_matrix(cmd->argv);
	if (cmd->path)
		free(cmd->path);
	if (cmd->redirs)
		node_free_all(&cmd->redirs, free_redir);
	free(cmd);
}

/*
** free_redir — destructor for t_redir, called through node_free_all.
*/
void	free_redir(void *ptr)
{
	t_redir	*r;

	if (!ptr)
		return ;
	r = (t_redir *)ptr;
	free(r->target);
	free(r->raw_target);
	free(r);
}

/*
** build_prompt — construct the "user:path$ " prompt string.
** Replaces the home prefix with ~ just like bash does.
** Stores the result in sh->prompt (frees the previous one if set).
*/
void	build_prompt(t_shell *sh)
{
	char	*user;
	char	*cwd;
	char	*home;
	char	*display;
	char	*tmp;

	if (sh->prompt)
		free(sh->prompt);
	user = env_get(sh->env, "USER");
	if (!user)
		user = "minishell";
	cwd = getcwd(NULL, 0);
	if (!cwd)
		cwd = ft_strdup("?");
	home = env_get(sh->env, "HOME");
	display = cwd;
	if (home && !ft_strncmp(cwd, home, ft_strlen(home)))
	{
		display = ft_strjoin("~", cwd + ft_strlen(home));
		free(cwd);
		cwd = NULL;
	}
	tmp = ft_strjoin(user, ":");
	sh->prompt = ft_strjoin(tmp, display);
	free(tmp);
	tmp = sh->prompt;
	sh->prompt = ft_strjoin(tmp, "$ ");
	free(tmp);
	if (display != cwd)
		free(display);
	else if (cwd)
		free(cwd);
}
