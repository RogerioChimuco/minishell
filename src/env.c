
#include "minishell.h"

/*
** Parse a raw "KEY=value" string into a new t_env node.
** Returns NULL on any allocation failure.
*/
static t_env	*env_node_new(const char *raw, int exported)
{
	t_env		*node;
	const char	*eq;
	size_t		klen;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	eq = strchr(raw, '=');
	if (eq)
	{
		klen = (size_t)(eq - raw);
		node->key = malloc(klen + 1);
		if (!node->key)
			return (free(node), NULL);
		memcpy(node->key, raw, klen);
		node->key[klen] = '\0';
		node->val = msh_strdup(eq + 1);
	}
	else
	{
		node->key = msh_strdup(raw);
		node->val = msh_strdup("");
	}
	if (!node->key || !node->val)
	{
		msh_free((void **)&node->key);
		msh_free((void **)&node->val);
		return (free(node), NULL);
	}
	node->exported = exported;
	node->next = NULL;
	return (node);
}

/*
** Build the internal env list from the envp array passed to main().
** Returns NULL if envp is NULL or on first allocation failure.
*/
t_env	*env_build(char **envp)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	int		i;

	if (!envp)
		return (NULL);
	head = NULL;
	tail = NULL;
	i = 0;
	while (envp[i])
	{
		node = env_node_new(envp[i], 1);
		if (!node)
			return (env_free(&head), NULL);
		if (!head)
			head = node;
		else
			tail->next = node;
		tail = node;
		i++;
	}
	return (head);
}

/*
** Free every node in the env list and set *lst to NULL.
*/
void	env_free(t_env **lst)
{
	t_env	*cur;
	t_env	*nxt;

	if (!lst || !*lst)
		return ;
	cur = *lst;
	while (cur)
	{
		nxt = cur->next;
		msh_free((void **)&cur->key);
		msh_free((void **)&cur->val);
		free(cur);
		cur = nxt;
	}
	*lst = NULL;
}

/*
** Return the value string for the given key, or NULL if not found.
** The returned pointer belongs to the list — do not free it.
*/
char	*env_get(t_env *env, const char *key)
{
	while (env)
	{
		if (strcmp(env->key, key) == 0)
			return (env->val);
		env = env->next;
	}
	return (NULL);
}

/*
** Set (or create) the variable 'key' to 'val'.
** exported controls whether it appears in child processes.
** Returns 0 on success, -1 on allocation failure.
*/
int	env_set(t_env **env, const char *key, const char *val, int exported)
{
	t_env	*cur;
	t_env	*node;
	char	*new_val;

	cur = *env;
	while (cur)
	{
		if (strcmp(cur->key, key) == 0)
		{
			new_val = msh_strdup(val ? val : "");
			if (!new_val)
				return (-1);
			msh_free((void **)&cur->val);
			cur->val = new_val;
			if (exported)
				cur->exported = 1;
			return (0);
		}
		cur = cur->next;
	}
	node = malloc(sizeof(t_env));
	if (!node)
		return (-1);
	node->key = msh_strdup(key);
	node->val = msh_strdup(val ? val : "");
	node->exported = exported;
	node->next = NULL;
	if (!node->key || !node->val)
	{
		msh_free((void **)&node->key);
		msh_free((void **)&node->val);
		return (free(node), -1);
	}
	if (!*env)
		*env = node;
	else
	{
		cur = *env;
		while (cur->next)
			cur = cur->next;
		cur->next = node;
	}
	return (0);
}
