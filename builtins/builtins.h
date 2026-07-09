/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 12:30:22 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:50:49 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef BUILTINS_H
# define BUILTINS_H

# include "../minishell.h"

int		ft_matrislen(char **argv);
int		ft_pwd(int argc, char **av);
int		var_name_validate(char *var);
int		is_builtin(char const *value);
int		ft_echo(int argc, char **argv);
int		ft_env(int argc, char **argv, t_env *env);
int		ft_export(t_env **vars, char **av);
int		ft_unset(t_env **env, char **argv);
int		ft_cd(int argc, char **argv, t_env **env);
int		built_error(char *cmd, char *arg, char *msg);
int		invoke_builtin(t_command *cmd, t_shell *shell);
int		ft_exit(int argc, char **argv, t_shell *shell);

char	*var_add(t_env *env);
char	*ft_getenv(t_env *env, char *name);

void	env_update(t_env **env, char *key, char *value);
void	matriz_delete(char **matriz);
void	env_print(t_env *env, int sort, int i);

size_t	env_size(t_env *env);

t_env	*env_find(t_env *env, char *key);
t_env	*env_new(char *key, char *value);

#endif
