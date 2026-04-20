/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chheniqu <chheniqu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:18:18 by armarboe          #+#    #+#             */
/*   Updated: 2025/06/26 17:32:07 by chheniqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Frees all environment variables from the shell and releases their memory.
void	free_env(t_shell *data)
{
	t_env	*temp;
	t_env	*next;

	temp = data->env;
	while (temp)
	{
		next = temp->next;
		if (temp->key)
		{
			free(temp->key);
			temp->key = NULL;
		}
		if (temp->value)
		{
			free(temp->value);
			temp->value = NULL;
		}
		free(temp);
		temp = next;
	}
	data->env = NULL;
}

// Creates a new environment variable node with the given key and value.
t_env	*create_env_node(char *key, char *value)
{
	t_env	*env_var;

	if (!key || !value)
		return (NULL);
	env_var = malloc(sizeof(t_env));
	if (!env_var)
		return (NULL);
	env_var->key = ft_strdup(key);
	env_var->value = ft_strdup(value);
	if (!env_var->key || !env_var->value)
	{
		free(env_var->key);
		free(env_var->value);
		return (NULL);
	}
	env_var->next = NULL;
	return (env_var);
}

// Adds a new environment variable node to the end of the shell's environment list.
void	add_var_to_list(t_shell *data, t_env *new)
{
	t_env	*temp;

	if (!data || !new)
		return ;
	if (!data->env)
	{
		data->env = new;
		return ;
	}
	else
		temp = data->env;
	while (temp->next)
		temp = temp->next;
	temp->next = new;
}

// Extracts key and value from a string in the form KEY=VALUE and creates an env node.
static t_env	*extract_env_var(char *str)
{
	size_t	j;
	char	*key;
	char	*value;
	t_env	*env_var;

	if (!str)
		return (NULL);
	j = 0;
	while (str[j] && str[j] != '=')
			j++;
	if (str[j] != '=')
		return (NULL);
	key = ft_substr(str, 0, j);
	if (!key)
		return (NULL);
	if (str[j + 1] != '\0')
		value = ft_substr(str, j + 1, ft_strlen(str) - j - 1);
	else
		value = ft_strdup("");
	if (!value)
		return (free(key), NULL);
	env_var = create_env_node(key, value);
	if (!env_var)
		return (free(key), free(value), NULL);
	return (free(key), free(value), env_var);
}

// Initializes the shell's environment list from the given environment array.
void	get_env(t_shell *data, char **env)
{
	size_t	i;
	t_env	*env_var;

	if (!env || !*env)
		return ;
	i = 0;
	while (env[i])
	{
		env_var = extract_env_var(env[i]);
		if (env_var)
			add_var_to_list(data, env_var);
		else
		{
			free_env(data);
			return ;
		}
		i++;
	}
}
