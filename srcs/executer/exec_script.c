/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_script.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ede-ram <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/09 07:24:48 by ede-ram           #+#    #+#             */
/*   Updated: 2019/12/19 05:42:32 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_job_control.h"
#include "sh_tokenizer.h"
#include "sh_executer.h"
#include "sh_redirections.h"
#include "sh.h"
#include "errno.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "sh_exitpoint.h"

int		exec_compound_command(t_sh *p, t_token *token_compound, int type)
{
	if (type == SH_WHILE || type == SH_UNTIL)
		return (exec_compound_while(p, token_compound, type));
	else if (type == SH_IF)
		return (exec_compound_if(p, token_compound));
	else if (type == SH_CASE)
		return (exec_compound_case(p, token_compound));
	else if (type == SH_FOR)
		return (exec_compound_for(p, token_compound));
	else if (type == SH_SUBSH)
		return (exec_compound_subsh(p, token_compound));
	return (exec_script(p, token_compound->sub));
}

t_token	*find_cmd_name(t_token *tok)
{
	while (tok)
	{
		if (tok->content && !(ft_strchr(tok->content, '=') > tok->content))
			return (tok);
		tok = tok->next;
	}
	return (tok);
}

int		exec_command(t_sh *p, t_token *token_begin, t_token *token_end)
{
	int		nb_redirections;
	t_token	*tok;
	int		nb_assign;

	tok = find_cmd_name(token_begin);
	if (tok && is_compound(tok->type))
	{
		if (p->nb_nested_compounds >= SH_NESTED_COMPOUND_LIMIT)
			sh_dprintf(2, "NEST_COMPOUND_LIMIT REACHED\nAbort %i\n", p->abort_cmd = 1);
		if (p->nb_nested_compounds >= SH_NESTED_COMPOUND_LIMIT)
			return (-121);
		p->nb_nested_compounds++;
		nb_assign = 0;
		nb_redirections = stock_redirections_assignements_compound(p,
				token_begin, token_end, &nb_assign);
		tok = (t_token*)(uint64_t)((!p->abort_cmd) ? exec_compound_command(p,
					tok, tok->type) : -125);
		del_n_redirect_lst(&p->redirect_lst, nb_redirections);
		del_n_assign_lst(p, nb_assign);
		p->nb_nested_compounds--;
	}
	else
		tok = (t_token*)(uint64_t)exec_simple_command(p, token_begin,
				token_end);
	return ((int)tok);
}

int		exec_command_in_background_closing_pipe(t_token *token_begin,
		t_token *token_end, int pipe1, int pipe2)
{
	int			child_pid;
	t_pipe_lst	*pipe_lst;
	t_sh		*p;

	p = sh();
	child_pid = fork_process(p, 0);
	if (child_pid < 0)
		exit(1);
	if (child_pid)
		return (child_pid);
	pipe_lst = sh()->pipe_lst;
	(void)pipe1;
	(void)pipe2;
	/*WAS HERE, DID It BREAK SMTHNG?
	while (pipe_lst)
	{
		if (pipe_lst->pipe[0] != pipe1 && pipe_lst->pipe[0] != pipe2)
			if (pipe_lst->pipe[0] > 2)
				close(pipe_lst->pipe[0]);
		if (pipe_lst->pipe[1] != pipe1 && pipe_lst->pipe[1] != pipe2)
			if (pipe_lst->pipe[1] > 2)
				close(pipe_lst->pipe[1]);
		pipe_lst = pipe_lst->next;
	}*/
	exec_command(p, token_begin, token_end);
	exit(1);
}

void	toggle_redirect_pipe(int toggle_on, int fd_in, int fd_out)
{
	if (toggle_on)
	{
		if (fd_in != -1)
			push_redirect_lst(&sh()->redirect_lst, 0, fd_in);
		if (fd_out != -1)
			push_redirect_lst(&sh()->redirect_lst, 1, fd_out);
	}
	else
		del_n_redirect_lst(&sh()->redirect_lst, (fd_in != -1) + (fd_out != -1));
}

int		exec_pipeline_recursively(t_sh *p, t_token *token_begin,
		t_token *token_end, int prev_pipe)
{
	t_token	*next_separator;
	int		next_pipe[2];

	next_separator = find_token_by_key_until(token_begin, token_end, &p->type,
			&p->pipeline_separators);
	if (!next_separator || next_separator->type != SH_OR)
	{
		toggle_redirect_pipe(1, prev_pipe, p->extern_pipe);
		p->force_setpgrp_setattr = 1;
		p->pgid_current_pipeline = exec_command_in_background_closing_pipe(
				token_begin, token_end, prev_pipe, p->extern_pipe);
		toggle_redirect_pipe(0, prev_pipe, p->extern_pipe);
		return (1 + (p->force_setpgrp_setattr = 0));
	}
	if (pipe(next_pipe) < 0)
		return (-1);
	push_pipe_lst(&p->pipe_lst, next_pipe);
	if (exec_pipeline_recursively(p, next_separator->next, token_end,
				next_pipe[0]) < 0)
		return (-1);
	toggle_redirect_pipe(1, prev_pipe, next_pipe[1]);
	exec_command_in_background_closing_pipe(token_begin, next_separator,
			prev_pipe, next_pipe[1]);
	toggle_redirect_pipe(0, prev_pipe, next_pipe[1]);
	return (1);
}

void	setup_pipeline_handle_bang(t_sh *p, t_token **p_token_begin,
		t_token *token_end, int *bang)
{
	p->pgid_current_pipeline = 0;
	p->index_pipeline_begin = (*p_token_begin)->index;
	p->index_pipeline_end = (token_end) ? token_end->index : -1;
	*bang = 0;
	if ((*p_token_begin)->type == SH_BANG)
	{
		*bang = 1;
		*p_token_begin = (*p_token_begin)->next;
	}
}

int		find_next_pipe(t_sh *p)
{
	t_redirect_lst *rlst;

	rlst = p->redirect_lst;
	while (rlst)
	{
		//printf("[%i]plst %i %i\n", getpid(), rlst->in, rlst->out);
		if (rlst->in == 1)
			return (rlst->out);
		rlst = rlst->next;
	}
	return (-1);
}

int		find_previous_pipe(t_sh *p)
{
	t_redirect_lst *rlst;

	rlst = p->redirect_lst;
	while (rlst)
	{
		//printf("[%i]plst %i %i\n", getpid(), rlst->in, rlst->out);
		if (rlst->in == 0)
			return (rlst->out);
		rlst = rlst->next;
	}
	return (-1);
}

int		exec_pipeline_core(t_token *token_begin, t_token *token_end)
{
	int		tmp;
	int		tmp2;
	t_sh	*p;

	p = sh();
	tmp = p->extern_pipe;
	p->extern_pipe = find_next_pipe(p);
	if (exec_pipeline_recursively(p, token_begin, token_end,
				find_previous_pipe(p)) < 0)
	{
		delete_close_all_pipe_lst(p->pipe_lst);
		p->pipe_lst = 0;
		return (0);
	}
	delete_close_all_pipe_lst(p->pipe_lst);
	p->pipe_lst = 0;
	tmp2 = p->pgid_current_pipeline;
	p->pgid_current_pipeline = 0;
	p->last_cmd_result = block_wait(p, tmp2, 0);
	if (!p->process_is_stopped)
	{
		kill(-1 * tmp2, SIGKILL);
	}
	p->extern_pipe = tmp;
	return (1);
}

void	exec_pipeline(t_sh *p, t_token *token_begin, t_token *token_end)
{
	int		bang;
	t_token *next_sep;

	if (token_begin == token_end)
		return ;
	setup_pipeline_handle_bang(p, &token_begin, token_end, &bang);
	if ((next_sep = find_token_by_key_until(token_begin, token_end, &p->type,
					&p->pipeline_separators)) && next_sep->type == SH_OR)
	{
		if (!exec_pipeline_core(token_begin, token_end))
			return ;
	}
	else
		p->last_cmd_result = exec_command(p, token_begin, token_end);
	if (bang)
		p->last_cmd_result = !p->last_cmd_result;
}

void	exec_and_or(t_sh *p, t_token *token_begin, t_token *token_end)
{
	t_token		*next_separator;
	t_toktype	prev_separator;
	t_toktype	tmp;

	prev_separator = 0;
	while (token_begin && token_begin != token_end && !p->abort_cmd)
	{
		next_separator = find_token_by_key_until(token_begin, token_end,
				&p->type, &p->and_or_separators);
		tmp = p->type;
		if (!prev_separator || (prev_separator == SH_AND_IF &&
					!p->last_cmd_result)
				|| (prev_separator == SH_OR_IF && p->last_cmd_result))
			exec_pipeline(p, token_begin, next_separator);
		prev_separator = tmp;
		token_begin = (next_separator) ? next_separator->next : 0;
		while (token_begin && token_begin->type == SH_NEWLINE)
			token_begin = token_begin->next;
	}
}

void	close_cpy_std_fds(t_sh *p)
{
	close(p->cpy_std_fds[0]);
	close(p->cpy_std_fds[1]);
	close(p->cpy_std_fds[2]);
	p->cpy_std_fds[0] = -1;
	p->cpy_std_fds[1] = -1;
	p->cpy_std_fds[2] = -1;
}

void	create_process_group_give_terminal_access(t_sh *p, pid_t pid,
		int foreground)
{
	if (p->pgid_current_pipeline)
		setpgid(pid, p->pgid_current_pipeline);
	else
		setpgid(pid, pid);
	if (foreground || (p->force_setpgrp_setattr))//pipeline in background?
	{
		signal(SIGTTOU, SIG_IGN);
		tcsetpgrp(0, pid);
		tcsetattr(0, TCSADRAIN, &p->extern_termios);
		signal(SIGTTOU, SIG_DFL);
	}
}

int		fork_process(t_sh *p, int foreground)
{
	int		child_pid;
	int		create_pgrp;
	pid_t	pid;

	create_pgrp = 0;
	if (p->pid_main_process == getpid() && p->is_interactive)
		create_pgrp = 1;
	child_pid = fork();
	if (child_pid < 0)
	{
		printf("[%i]fork error: ressource temporarily unavailable\n", getpid());
		p->abort_cmd = 1;
		return (-1);
	}
	pid = (child_pid) ? child_pid : getpid();
	if (create_pgrp)
		create_process_group_give_terminal_access(p, pid, foreground);
	if (!child_pid)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);
	}
	if (!child_pid)
	{
		delete_all_jobs(p->jobs);
		p->jobs = 0;
		close_cpy_std_fds(p);
	}
	return (child_pid);
}

int		exec_and_or_in_background(t_sh *p, t_token *token_begin,
		t_token *token_end)
{
	int child_pid;

	child_pid = fork_process(p, 0);
	if (child_pid < 0)
		return (-1);
	if (child_pid == 0)
	{
		close_cpy_std_fds(p);
		exec_and_or(p, token_begin, token_end);
		exit(1);
	}
	else
	{
		add_job(child_pid, p->cmd, token_begin->index,
				(token_end) ? token_end->index : -1, "background");
	}
	return (0);
}

t_token	*find_next_script_separator(t_token *token_begin, t_token *token_end)
{
	int	skip_newline;

	skip_newline = 1;
	while (token_begin && token_begin != token_end)
	{
		if (token_begin->type == SH_SEMI || token_begin->type == SH_AND
				|| (token_begin->type == SH_NEWLINE && !skip_newline))
			return (token_begin);
		if (token_begin->type == SH_WORD || token_begin->type == SH_ASSIGN
				|| token_begin->type == SH_LESS || token_begin->type == SH_GREAT
				|| token_begin->type == SH_DLESS
				|| token_begin->type == SH_DGREAT
				|| token_begin->type == SH_LESSAND
				|| token_begin->type == SH_GREATAND
				|| token_begin->type == SH_LESSGREAT
				|| token_begin->type == SH_DLESSDASH
				|| token_begin->type == SH_CLOBBER
				|| token_begin->type == SH_IF || token_begin->type == SH_CASE
				|| token_begin->type == SH_WHILE
				|| token_begin->type == SH_UNTIL || token_begin->type == SH_FOR
				|| token_begin->type == SH_BRACES
				|| token_begin->type == SH_BANG || token_begin->type == SH_FUNC)
			skip_newline = 0;
		else if (token_begin->type == SH_AND || token_begin->type == SH_OR
				|| token_begin->type == SH_AND_IF
				|| token_begin->type == SH_OR_IF
				|| token_begin->type == SH_DSEMI)
			skip_newline = 1;
		token_begin = token_begin->next;
	}
	return (token_begin);
}

int		exec_script(t_sh *p, t_token *token_begin)
{
	t_token	*next_separator;

	while (token_begin && !p->abort_cmd)
	{
		while (token_begin && token_begin->type == SH_NEWLINE)
			token_begin = token_begin->next;
		next_separator = find_next_script_separator(token_begin, 0);
		if (next_separator && next_separator->type == SH_AND)
			exec_and_or_in_background(p, token_begin, next_separator);
		else
			exec_and_or(p, token_begin, next_separator);
		token_begin = (next_separator) ? next_separator->next : 0;
	}
	return (p->last_cmd_result);
}
