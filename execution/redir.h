/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:59:51 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:59:57 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef REDIR_H
# define REDIR_H

# include "../minishell.h"

int		check_ambiguous_redirect(t_redir *redir);
int		print_command_error(const char *cmd, int err);
int		heredoc_fork(char *delimiter, int *pipefd, t_shell *shell);
int		heredoc_wait(pid_t pid, int *pipefd, char *delimiter, t_shell *shell);
int		create_pipes(t_shell *shell);
int		create_pids(t_shell *shell);
int		apply_redirections(t_list *lst);
int		open_file_redir(int *fd, char *value, int (*f)(char *));
int		open_heredoc_redir_fd(int *fd, char *value,
			int (*f)(char *, t_shell *), t_shell *shell);
int		open_input_redir(char *filename);
int		open_output_redir(char *filename);
int		open_append_redir(char *filename);
int		open_heredoc_redir(char *delimiter, t_shell *shell);
int		process_all_heredocs(t_list *lst, t_shell *shell);
char	*get_redir_display_value(t_redir *redir);
void	print_redir_error(char *token, int is_ambiguous);
void	resolve_command(t_command *cmd, t_env *envp);
void	setup_pipes(int index, t_shell *shell);
void	close_pipes(t_shell *shell, int length);
void	execute(t_shell *shell);
void	handler_exec(int sig);
void	clear_shell(t_shell *shell);
void	setup_prompt_signal(void);
void	handler_exec(int sig);
void	setup_prompt_signal(void);
void	clear_shell(t_shell *shell);
void	restore_fds(int in, int out);
void	execute_child(t_command *cmd, int i, t_shell *shell);
void	fork_pipeline(t_shell *shell, t_env *envp);
void	wait_pipeline(t_shell *shell);
void	wait_pids(t_shell *shell, int pid_i);
void	run_heredoc_child(char *delimiter, int write_fd, t_shell *shell);
void	set_heredoc_signals(void);
void	check_exec_permissions(const char *path);

#endif
