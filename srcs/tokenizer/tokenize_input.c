/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ede-ram <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 13:17:07 by ede-ram           #+#    #+#             */
/*   Updated: 2020/02/14 22:51:13 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_tokenizer.h"
#include "sh_tokens.h"
#include "libft.h"
#include "sh.h"

t_toktype	tokenize_reserved_word(t_toktool *t, t_token **p_actual,
		t_toktype type, int word_begin)
{
	if (sh()->alias_end)
		sh()->alias_end--;
	if (is_compound(type))
	{
		if (!((*p_actual)->next = tokenize_compound(t, type, word_begin)))
			return (SH_SYNTAX_ERROR);
	}
	else
		(*p_actual)->next = create_token(type, word_begin, 0);
	return (0);
}

void		treat_input(t_toktool *t, t_toktype actual_compound,
		t_toktype *terminator, t_token **p_actual)
{
	forward_blanks(t);
	while (t->input[t->i] == '\n')
	{
		if (sh()->here)
		{
			if (sh_record_here_doc(t, sh()->here) == SH_SYNTAX_ERROR)
			{
				*terminator = SH_SYNTAX_ERROR;
				return ;
			}
		}
		(*p_actual)->next = create_token(SH_NEWLINE, t->i, 0);
		*p_actual = (*p_actual)->next;
		if (t->input[t->i])
			t->i++;
		t->word_nb = 1;
	}
	if (!(*terminator = treat_operator(t, p_actual, actual_compound)))
		*terminator = treat_word(t, p_actual, actual_compound);
}

int			protect_tokenizer(t_toktype *terminator, t_token *origin,
		t_toktype actual_compound)
{
	if (sh()->here)
	{
		while (sh()->here)
			sh_del_here_stack(&(sh()->here));
		sh()->unfinished_cmd = 1;
	}
	if (*terminator == SH_SYNTAX_ERROR)
	{
		free_ast(origin);
		return (0);
	}
	if (!*terminator && actual_compound != SH_NULL)
	{
		sh()->unfinished_cmd = 1;
		free_ast(origin);
		return (0);
	}
	return (1);
}

t_token		*recursive_tokenizer(t_toktool *t, t_toktype actual_compound,
		t_toktype *terminator)
{
	t_token	*origin;
	t_token	*actual;
	t_token *tmp;

	origin = create_token(0, 0, 0);
	tmp = sh()->tmp_ast;
	sh()->tmp_ast = origin;
	actual = origin;
	*terminator = 0;
	while (!*terminator && t->input[t->i])
		treat_input(t, actual_compound, terminator, &actual);
	if (!protect_tokenizer(terminator, origin, actual_compound))
		return (0);
	actual = origin->next;
	delete_token(origin);
	ft_free_tabstr(sh()->alias_stack);
	sh()->alias_stack = 0;
	sh()->tmp_ast = tmp;
	return (actual);
}

t_token		*tokenize_input(const char *input)
{
	t_token			*ast;
	t_toktype		terminator;
	t_toktool		tok_tool;

	tok_tool.input = ft_strdup(input);
	tok_tool.i = 0;
	tok_tool.word_nb = 1;
	ast = recursive_tokenizer(&tok_tool, SH_NULL, &terminator);
	ft_memdel((void**)&(sh()->cmd));
	sh()->cmd = ft_strdup(tok_tool.input);
	ft_memdel((void**)&(tok_tool.input));
	return (ast);
}
