/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_type.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chheniqu <chheniqu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:49:59 by chheniqu          #+#    #+#             */
/*   Updated: 2025/06/23 14:18:27 by chheniqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Determines the token type at position i in the input string.
t_token_type	token_type(char *str, int *i)
{
	if (str[*i] == '>' && str[*i + 1] == '<')
		return (*i += 2, (INVALID));
	if (str[*i] == '<' && str[*i + 1] == '>')
		return (*i += 2, (INVALID2));
	if (str[*i] == '|')
		return ((*i)++, PIPE);
	if (str[*i] == '<')
	{
		if (str[*i + 1] == '<')
			return (*i += 2, (REDIR_HEREDOC));
		(*i)++;
		return (REDIR_IN);
	}
	if (str[*i] == '>')
	{
		if (str[*i + 1] == '>')
			return (*i += 2, (REDIR_APPEND));
		(*i)++;
		return (REDIR_OUT);
	}
	return (WORD);
}

// Returns the string representation of a token type (e.g., |, <, >).
char	*symbol_type(t_token_type type)
{
	if (type == PIPE)
		return ("|");
	else if (type == REDIR_IN)
		return ("<");
	else if (type == REDIR_OUT)
		return (">");
	else if (type == REDIR_APPEND)
		return (">>");
	else if (type == REDIR_HEREDOC)
		return ("<<");
	return (NULL);
}
