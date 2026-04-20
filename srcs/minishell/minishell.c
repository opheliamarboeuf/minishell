/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armarboe <armarboe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 18:17:13 by armarboe          #+#    #+#             */
/*   Updated: 2025/06/27 14:56:22 by armarboe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex.h"

/*
 * minishell project:
 * - Recreates a simplified Unix shell in C.
 * - Execution order:
 *   1. Reads user input.
 *   2. Handles signals (SIGINT, SIGTSTP and SIGQUIT) for interactive shell behavior.
 *   3. Tokenizes the command line.
 *   4. Checks syntax and quotes.
 *   5. Expands environment variables and special parameters.
 *   6. Parses tokens into commands and redirections.
 *   7. Executes builtins or external commands, handling pipes and redirections.
 *   8. Cleans up resources and updates shell status.
 * - Implements builtins (cd, echo, env, exit, export, pwd, unset) and supports external commands.
 */

extern int	g_is_sig;

void	init_minishell(t_shell *data)
{
	data->env = NULL;
	data->token = NULL;
	data->shell_cmd = NULL;
	data->pipex_data = NULL;
	data->last_status = 0;
}

void	init_pipex(t_pipex_data *data)
{
	data->path = NULL;
	data->env = NULL;
	data->i = 0;
	data->exit_status = 0;
	data->pipe_fd = NULL;
	data->pipes_count = 0;
	data->cmd_count = 0;
	data->cmd_list = NULL;
}

// Main function to process a command line: tokenizes, parses, expands, executes, and cleans up.
void	ft_minishell(t_shell *data, char *line)
{
	data->token = tokenize(line);
	if (!data->token || check_quotes(line) == FAILURE)
	{
		data->last_status = 2;
		return ;
	}
	protect_ft_minishell(data->token, data, "token");
	if (has_syntax_error(data->token) != SUCCESS)
	{
		data->last_status = 2;
		return (free_tokens(&data->token));
	}
	expand_variable(data->token, data);
	if (has_syntax_error(data->token) == SUCCESS)
	{
		data->shell_cmd = parser(data->token);
		data->pipex_data = malloc(sizeof(t_pipex_data));
		protect_ft_minishell(data->pipex_data, data, "pipex_data");
		init_pipex(data->pipex_data);
		ft_pipex(data);
		data->last_status = free_pipex(data->pipex_data, \
			data->pipex_data->exit_status);
	}
	free_minishell(data);
}

// Entry point for minishell. Initializes shell, environment, and starts prompt loop.
int	main(int ac, char **av, char **env)
{
	t_shell	shell;

	(void)ac;
	(void)av;
	if (!env)
	{
		ft_putendl_fd("error : no environment", STDERR_FILENO);
		exit(FAILURE);
	}
	init_minishell(&shell);
	get_env(&shell, env);
	read_prompt(&shell);
	free_env(&shell);
	exit(SUCCESS);
}
