/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:46:27 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 12:30:03 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REDIR_H
# define REDIR_H

# include "../minishell.h"

int		ambiguous_redirect(t_redir *redir);
int		message_error_cmd(const char *cmd, int err);
int		heredoc_fork(char *delimiter, int *pipefd, t_shell *shell);
int		heredoc_wait(pid_t pid, int *pipefd, char *delimiter, t_shell *shell);
int		create_pipes(t_shell *shell);
int		create_pids(t_shell *shell);
int		apply_redir(t_list *lst);
int		open_redir_fd(int *fd, char *value, int (*f)(char *));
int		open_redir_heredoc_fd(int *fd, char *value,
			int (*f)(char *, t_shell *), t_shell *shell);
int		redir_in(char *filename);
int		redir_out(char *filename);
int		redir_append(char *filename);
int		redir_heredoc(char *delimiter, t_shell *shell);
int		foreach_cmd(t_list *lst, t_shell *shell);
char	*valible_name(t_redir *redir);
void	message_error_redir(char *token, int is_ambiguous);
void	resolve_command(t_command *cmd, t_env *envp);
void	setup_pipes(int index, t_shell *shell);
void	close_pipes(t_shell *shell, int length);
void	execute(t_shell *shell);
void	handler_exec(int sig);
void	shell_clear(t_shell *shell);
void	setup_prompt_signal(void);
void	handler_exec(int sig);
void	setup_prompt_signal(void);
void	shell_clear(t_shell *shell);
void	restore_fds(int in, int out);
void	execute_child(t_command *cmd, int i, t_shell *shell);
void	fork_pipeline(t_shell *shell, t_env *envp);
void	wait_pipeline(t_shell *shell);
void	wait_pids(t_shell *shell, int pid_i);
void	heredoc_child(char *delimiter, int write_fd, t_shell *shell);
void	set_signal(void);
void	is_dir_or_file(const char *path);

#endif
