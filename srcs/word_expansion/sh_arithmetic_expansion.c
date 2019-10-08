/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_arithmetic_expansion.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcillard <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/22 09:31:05 by tcillard          #+#    #+#             */
/*   Updated: 2019/09/25 08:29:23 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_word_expansion.h"
#include <stdio.h>

int		sh_arithmetic_string_size(t_exp *exp)
{
	int		size;
	short	quote;

	size = exp->i;
	quote = 0;
	exp->i++;
	while ((exp->content[exp->i] != ')' || quote) && exp->content[exp->i])
	{
		sh_subsh_quote(exp, &quote);
		if (!quote && exp->content[exp->i] == '(')
			sh_arithmetic_string_size(exp);
		exp->i++;
	}
	return (exp->i - size);
}

void	sh_record_arithmetic_string(t_exp *exp)
{
	int		size;

	size = sh_arithmetic_string_size(exp);
	exp->i = exp->i - size + 1;
	if (!(exp->name = (char*)malloc(size + 1)))
		exit (-1);
	sh_str_start_end(&(exp->name), exp->content, exp->i, exp->i + size - 1);
}
/*
t_arith	*sh_creat_arithmetic_ast(char *str)
{
	int		i;

	
}*/

void	sh_arithmetic_expansion(t_exp *exp)
{
	t_arith	*arith;

	arith = NULL;
	exp->i++;
	sh_record_arithmetic_string(exp);
//	arith = sh_creat_arithmetic_ast(exp->name);
}