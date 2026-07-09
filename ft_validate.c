#include "minishell.h"

static int	print_syntax_err(int status_code, char *err_msg)
{
	ft_putendl_fd(err_msg, STDERR_FILENO);
	return (status_code);
}

int	ft_validate_quotes(const char *line)
{
	int	quote_state;

	if (!line)
		return (0);
	quote_state = 0;
	while (*line)
	{
		if (*line == SINGLE_QUOTE && quote_state != 2)
		{
			if (quote_state == 1)
				quote_state = 0;
			else
				quote_state = 1;
		}
		else if (*line == DOUBLE_QUOTE && quote_state != 1)
		{
			if (quote_state == 2)
				quote_state = 0;
			else
				quote_state = 2;
		}
		line++;
	}
	if (quote_state == 1)
		return (print_syntax_err(2, "minishell: syntax error: unclosed single quote"));
	if (quote_state == 2)
		return (print_syntax_err(3, "minishell: syntax error: unclosed double quote"));
	return (0);
}
