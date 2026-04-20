/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chheniqu <chheniqu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:49:36 by chheniqu          #+#    #+#             */
/*   Updated: 2025/06/27 13:11:15 by chheniqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Tokenization in minishell:
 * - Splits the input line into tokens representing commands, arguments, pipes, and redirections.
 * - Steps:
 *   1. Checks for valid quotes in the input.
 *   2. Skips whitespace and iterates through the line.
 *   3. Processes each token (word, pipe, redirection) and adds it to the token list.
 *   4. Frees all tokens and returns NULL if an error occurs.
 *   5. Returns the complete token list for further parsing.
 */

// Tokenizes the input line into a linked list of tokens.
t_token	*tokenize(char *line)
{
	int				i;
	t_token			*tokens;
	t_token			*new;

	i = 0;
	tokens = NULL;
	if (!line || check_quotes(line) == FAILURE)
		return (NULL);
	while (line[i])
	{
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (!line[i])
			break ;
		new = process_tokens(line, &i);
		if (!new)
		{
			free_tokens(&tokens);
			return (NULL);
		}
		add_token_back(&tokens, new);
	}
	return (tokens);
}
