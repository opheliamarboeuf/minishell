/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armarboe <armarboe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:47:16 by armarboe          #+#    #+#             */
/*   Updated: 2025/06/27 14:32:40 by armarboe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Pipeline execution in minishell (pipex.c):
 * - Manages the execution of multiple commands connected by pipes.
 * - Steps:
 *   1. Builds the command list and initializes pipex data.
 *   2. Sets up environment and heredoc handling.
 *   3. Handles signals for parent and child processes.
 *   4. Iterates through commands:
 *      - Checks for builtins.
 *      - Executes builtins directly if only one command.
 *      - Otherwise, checks for directories and forks child processes.
 *      - Sets up pipes and redirections for each command.
 *   5. Closes all pipes and waits for child processes to finish.
 *   6. Cleans up resources and restores signals.
 */

#include "pipex.h"

// Executes a command using execve, handling errors and freeing resources on failure.
void	execute_cmd(t_shell *shell_data, t_cmd *cmd, char **env)
{
	cmd->execve_av = ft_split_minishell(cmd->cmd_str);
	if (!cmd->execve_av)
	{
		perror("malloc failed");
		free_pipex(shell_data->pipex_data, EXIT_FAILURE);
		free_minishell(shell_data);
		free_env(shell_data);
	}
	execve(cmd->cmd_path, cmd->execve_av, env);
	perror("execve command");
	free_pipex(shell_data->pipex_data, EXIT_FAILURE);
	free_minishell(shell_data);
	free_env(shell_data);
}

// Handles the case where no command string is present, frees resources and exits.
void	handle_no_cmd_str(t_shell *shell_data)
{
	int	exit_code;

	exit_code = shell_data->pipex_data->exit_status;
	free_pipex(shell_data->pipex_data, exit_code);
	free_minishell(shell_data);
	free_env(shell_data);
	exit(exit_code);
}

// Forks and executes the child process, sets up redirections, handles builtins, and executes the command.
void	child_process(t_shell *shell_data, t_cmd *cmd, char **env)
{
	cmd->pid = fork();
	if (cmd->pid == -1)
	{
		perror("fork error");
		free_pipex(shell_data->pipex_data, EXIT_FAILURE);
		free_minishell(shell_data);
		free_env(shell_data);
		exit(EXIT_FAILURE);
	}
	if (cmd->pid == 0)
	{
		check_punctuation(shell_data, cmd);
		init_redir(shell_data, cmd);
		close_heredoc_read_fd(shell_data->pipex_data);
		if (!cmd->cmd_str || !*cmd->cmd_str || \
			cmd->is_directory == true || !*env)
			handle_no_cmd_str(shell_data);
		if (is_builtin(cmd->clean_cmd) == SUCCESS)
			exec_builtins_in_child(shell_data, cmd);
		get_cmd_path(shell_data, cmd);
		close_all_pipes(shell_data->pipex_data);
		execute_cmd(shell_data, cmd, env);
	}
	else
		close_heredoc_fd(cmd);
}

// Executes all commands in the pipeline:
// - Handles heredoc setup and pipe initialization
// - Iterates through each command node
// - Executes builtins directly if only one command
// - Otherwise, checks for directories and forks child processes for each command
// - Closes pipes and waits for all children at the end
void	ft_pipe(t_shell *shell_data, char **env)
{
	t_cmd	*cmd;

	cmd = shell_data->pipex_data->cmd_list;
	handle_heredoc(shell_data);
	init_ft_pipe(shell_data, env);
	while (cmd)
	{
		get_clean_cmd(cmd, cmd->cmd_str);
		if (shell_data->pipex_data->cmd_count == 1 && \
			is_builtin(cmd->clean_cmd) == SUCCESS)
		{
			cmd->is_builtin_parent = true;
			exec_builtins_in_parent(shell_data, cmd);
			return ;
		}
		else
		{
			check_directory(shell_data, cmd);
			child_process(shell_data, cmd, env);
		}
		cmd = cmd->next;
	}
	close_all_pipes(shell_data->pipex_data);
	wait_for_children(shell_data);
	close_heredoc_read_fd(shell_data->pipex_data);
}

// Sets up the pipex data structure, builds command list, and starts pipeline execution.
void	ft_pipex(t_shell *shell_data)
{
	t_cmd		*cmd;
	t_shell_cmd	*temp;

	temp = shell_data->shell_cmd;
	while (temp)
	{
		cmd = create_cmd_node(temp->cmd_str);
		if (!cmd)
			free_pipex(shell_data->pipex_data, EXIT_FAILURE);
		add_cmd_to_list(shell_data->pipex_data, cmd);
		cmd->redir = temp->redir;
		shell_data->pipex_data ->cmd_count++;
		temp = temp->next;
	}
	shell_data->pipex_data->pipes_count = shell_data->pipex_data->cmd_count - 1;
	shell_data->pipex_data->env = get_env_array(shell_data);
	if (!shell_data->pipex_data->env)
		return ;
	ignore_signals_parent();
	ft_pipe(shell_data, shell_data->pipex_data->env);
	free_pipex(shell_data->pipex_data, shell_data->pipex_data->exit_status);
	init_signals_parent();
}
