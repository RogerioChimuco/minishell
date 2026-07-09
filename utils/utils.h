/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:55:29 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:55:36 by luqalmei         ###   ########.fr       */
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
char	*prompt_user(t_env *env);
char	*prompt_path(t_env *env);
char	*concat_and_free(char *s1, char *s2);
void	apply_home_shorthand(char **path, char *home);
int		push_computed_var(char *value, t_env **head, t_env **tail);
int		push_home_path(t_env **head, t_env **tail);
int		push_path_home(t_env **head, t_env **tail);

#endif
