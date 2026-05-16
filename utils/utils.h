/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 10:29:24 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 13:12:46 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "../minishell.h"

void	handler(int sig);
void	var_delete(t_env *env);
void	ft_historic(void *line);
void	env_delete(t_env **var);
void	var_get_key_value(char *var, char **key, char **value);
void	ft_prompt(char **prompt, t_env *env);
void	setup_prompt_signal(void);
void	handler_exec(int sig);
void	envp_update(t_shell *shell);
void	ft_free(void **s1, void **s2);
char	*ft_readline(char *prompt);
char	*home_path(t_env *env);
t_env	*env_add(char *var);
t_env	*env_get(char **env, char *home);

#endif
