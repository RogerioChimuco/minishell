
#include "minishell.h"

/*
** Shorten 'cwd' by replacing the home prefix with '~'.
** Returns a newly allocated string; caller must free.
*/
static char	*shorten_cwd(const char *cwd, const char *home)
{
	size_t	hlen;
	char	*short_path;

	if (!home || !cwd)
		return (msh_strdup(cwd ? cwd : "?"));
	hlen = strlen(home);
	if (strncmp(cwd, home, hlen) == 0
		&& (cwd[hlen] == '/' || cwd[hlen] == '\0'))
	{
		short_path = msh_strjoin("~", cwd + hlen);
		return (short_path ? short_path : msh_strdup(cwd));
	}
	return (msh_strdup(cwd));
}

/*
** Build a "user:path$ " prompt string.
** Caller is responsible for freeing the returned string.
*/
char	*prompt_build(t_env *env)
{
	char	*user;
	char	*home;
	char	*cwd_raw;
	char	*cwd;
	char	*tmp;
	char	*prompt;

	user = env_get(env, "USER");
	if (!user)
		user = "minishell";
	home = env_get(env, "HOME");
	cwd_raw = getcwd(NULL, 0);
	cwd = shorten_cwd(cwd_raw, home);
	free(cwd_raw);
	tmp = msh_strjoin(user, ":");
	if (!tmp)
		return (msh_free((void **)&cwd), msh_strdup("$ "));
	prompt = msh_strjoin(tmp, cwd);
	msh_free((void **)&tmp);
	msh_free((void **)&cwd);
	if (!prompt)
		return (msh_strdup("$ "));
	tmp = msh_strjoin(prompt, "$ ");
	msh_free((void **)&prompt);
	return (tmp ? tmp : msh_strdup("$ "));
}
