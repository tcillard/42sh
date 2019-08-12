/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_exec_stdin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/12 13:56:39 by thdelmas          #+#    #+#             */
/*   Updated: 2019/08/12 14:11:22 by thdelmas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"
#include "libft.h"
#include "t_token.h"
#include <fcntl.h>

int	sh_exec_stdin(void)
{
	char *buff;
	char *ret;

	ret = NULL;
	buff = ft_strnew(1024);
	ft_putendl("...");
	while (buff && read(0, buff, 1024) > 0)
	{
		if (!ret)
			ret = ft_strdup(buff);
		else
			ret = ft_strjoin_free(ret, buff, ret);
		ft_bzero(buff, 1024);
	}
	free(buff);
	if (ret && *ret && (sh()->ast = tokenize_input(ret)))//line
	{
		print_all_tokens(sh(), sh()->ast, 0);
		exec_script(sh(), sh()->ast, 0);
	}
	free(ret);
}