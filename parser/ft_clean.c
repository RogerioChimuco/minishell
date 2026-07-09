
#include "parse.h"

void	ft_redirclear(void *arg)
{
	t_redir	*redir;

	if (!arg)
		return ;
	redir = (t_redir *)arg;
	if (redir->value)
		free(redir->value);
	if (redir->original_value)
		free(redir->original_value);
	redir->value = NULL;
	redir->original_value = NULL;
	free(redir);
}

void	command_clear(void *arg)
{
	int			i;
	char		*first_arg;
	t_command	*cmd;

	if (!arg)
		return ;
	cmd = (t_command *)arg;
	first_arg = cmd->args[0];
	i = -1;
	while (cmd->args[++i])
		free(cmd->args[i]);
	free(cmd->args);
	if (cmd->path && cmd->path != first_arg)
	{
		free(cmd->path);
		cmd->path = NULL;
	}
	ft_lstclear(&cmd->redir, ft_redirclear);
	ft_free((void *)&cmd, 0);
}
