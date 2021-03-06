/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ede-ram <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 13:17:07 by ede-ram           #+#    #+#             */
/*   Updated: 2020/01/27 13:17:09 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_word_expansion.h"

int		sh_skip_white_space(char *str, int i)
{
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		i++;
	return (i);
}

int		sh_skip_number(char *str, int i)
{
	while (str[i] >= '0' && str[i] <= '9')
		i++;
	i = sh_skip_white_space(str, i);
	return (i);
}

int		sh_skip_parenthesis(char *str, int i)
{
	while (str[i] == ')')
	{
		i++;
		i = sh_skip_white_space(str, i);
	}
	return (i);
}

int		sh_skip_number_par(char *str, int i)
{
	int		opt;

	opt = 0;
	if (str[i] < '0' || str[i] > '9')
	{
		i++;
		i = sh_skip_white_space(str, i);
		while (str[i] == '(')
		{
			i++;
			i = sh_skip_white_space(str, i);
		}
		if (str[i] == '-' || str[i] == '+')
			i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		i++;
	i = sh_skip_white_space(str, i);
	if (str[i] == ')')
		i = sh_skip_parenthesis(str, i);
	return (i);
}
