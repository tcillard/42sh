/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_subsh_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/14 04:11:29 by thdelmas          #+#    #+#             */
/*   Updated: 2020/01/16 01:00:54 by thdelmas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_word_expansion.h"
#include "sh_tokenizer.h"
#include "sh_executer.h"
#include "sh_redirections.h"
#include "sh_tokens.h"

void	ft_subshcpy(char *buff, char **str, int i)
{
	int		j;

	j = 0;
	while (buff[j])
		(*str)[i++] = buff[j++];
	(*str)[i] = '\0';
}

void	sh_subshdup(char *buff, t_exp *exp)
{
	char	*cpy;
	int		i;

	i = 0;
	cpy = exp->value;
	if (!(exp->value))
	{
		if (!(exp->value = malloc(ft_strlen(buff) + 1)))
			destructor(-1);
	}
	else
	{
		if (!(exp->value = malloc(ft_strlen(cpy) + ft_strlen(buff) + 1)))
			destructor(-1);
		while (cpy[i])
		{
			exp->value[i] = cpy[i];
			i++;
		}
		ft_memdel((void**)&cpy);
	}
	ft_subshcpy(buff, &(exp->value), i);
}

void	sh_read_pipe(t_exp *exp, int fd)
{
	int		ret;
	char	buff[500];

	ret = 0;
	while ((ret = read(fd, &buff, 499)) > 0)
	{
		buff[ret] = '\0';
		sh_subshdup(buff, exp);
	}
}

void	sh_subsh_expansion(t_exp *exp)
{
	t_token	*tok;
	int		pipe_fd[2];

	sh_record_commande_string(exp);
	tok = create_token(SH_SUBSH, 0, NULL);
	if ((tok->sub = tokenize_input(exp->name)))
	{
		if (pipe(pipe_fd) == -1)
			destructor(-1);
		push_redirect_lst(&(sh()->redirect_lst), 1, pipe_fd[1]);
		exec_compound_subsh(sh(), tok);
		free_ast(tok);
		del_n_redirect_lst(&(sh()->redirect_lst), 1);
		sh_read_pipe(exp, pipe_fd[0]);
		close(pipe_fd[0]);
	}
	else
		sh()->abort_cmd = 1;
}
