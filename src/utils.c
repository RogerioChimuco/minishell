
#include "minishell.h"

/*
** Safe strdup: returns NULL on NULL input or alloc failure.
*/
char	*msh_strdup(const char *s)
{
	size_t	len;
	char	*out;

	if (!s)
		return (NULL);
	len = strlen(s);
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	memcpy(out, s, len + 1);
	return (out);
}

/*
** Concatenate two strings into a new allocation.
** Either argument may be NULL (treated as empty string).
*/
char	*msh_strjoin(const char *a, const char *b)
{
	size_t	la;
	size_t	lb;
	char	*out;

	la = a ? strlen(a) : 0;
	lb = b ? strlen(b) : 0;
	out = malloc(la + lb + 1);
	if (!out)
		return (NULL);
	if (a)
		memcpy(out, a, la);
	if (b)
		memcpy(out + la, b, lb);
	out[la + lb] = '\0';
	return (out);
}

/*
** Free *p and set it to NULL in one call.
*/
void	msh_free(void **p)
{
	if (p && *p)
	{
		free(*p);
		*p = NULL;
	}
}

/*
** Return non-zero if c is a whitespace character recognised by the shell.
*/
int	msh_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\v' || c == '\f');
}
