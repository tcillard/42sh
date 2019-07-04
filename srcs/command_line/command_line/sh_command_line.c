/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmeyer <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/20 07:36:41 by tmeyer            #+#    #+#             */
/*   Updated: 2019/06/24 06:54:11 by tmeyer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "21sh.h"
#include "sh_command_edition.h"
#include "sh_command_line.h"
#include "history.h"
#include <unistd.h>

char			*sh_add_newline(char *line)
{
	char *tmp;

	tmp = ft_strdup(line);
	free(line);
	line = ft_strjoin((const char*)tmp, "\n");
	free(tmp);
	tmp = NULL;
	return (line);
}

static void		sh_ask_close_quote(char **line, int k, int code, int res)
{
	if (code == 1)
	{
		if (k == 0)
		{
			*line = sh_add_newline(*line);
			write(0, "dquote> ", 8);
		}
	}
	else if (code == 2)
	{
		if (k > 0 && res == 1)
		{
			*line = sh_add_newline(*line);
			write (0, "> ", 2);
		}
	}
}

static int		sh_check_sub_shell(char **line, int res)
{
	int			i;
	static int	k = 0;
	static char	c = 0;

	i = 0;
	while (line[0][i] != '\0')
	{
		if ((line[0][i] == '"' || line[0][i] == '\'' || line[0][i] == '`') 
				&& !sh_ice(*line, i) && (c = line[0][i]) && i++)
		{
			while (line[0][i] != 0 && !(line[0][i] == c && !sh_ice(*line, i)))
				i++;
			if (line[0][i] == 0)
				return (k != 0 ? 0 : 1);
		}
		if (line[0][i] == '(' && !sh_ice(*line, i))
			k++;
		if (line[0][i] == ')' && !sh_ice(*line, i))
			k--;
		if (k < 0)
			return (1);
		if (line[0][i] != '\0')
			i++;
	}
	sh_ask_close_quote(line, k, 2, res);
	return (k != 0 ? 0 : sh_parse_line_escape(line));
}

static int		sh_check_line(char **line)
{
	int			i;
	int			j;
	static int	k = 1;
	static char	c = '\0';

	i = 0;
	if (!*line)
		return (0);
	while (line[0][i] != '\0')
	{
		if ((line[0][i] == 34 || line[0][i] == 39 || line[0][i] == 96)
				&& !sh_ice(*line, i) && (c == 0 || c == line[0][i]))
		{
			k = (k == 1 ? 0 : 1);
			c = (k == 0 ? line[0][i] : '\0');
		}
		i++;
	}
	sh_ask_close_quote(line, k, 1, 0);
	j = sh_check_sub_shell(line, k);
	return (k == 0 ? k : j);
}

char		**sh_arguments(t_hist *hist)
{
	int		ret;
	char	*str;
	char	*tmp;
	char	*line;
	char	**command;

	line = NULL;
	command = NULL;
	str = NULL;
	ret = 0;
	while (!ret && sh_reader(&line, hist))
	{
		tmp = ft_strdup(str);
		free(str);
		ret = sh_check_line(&line);
		str = ft_strjoin((const char*)tmp, (const char*)line);
		ft_memdel((void**)&tmp);
		ft_memdel((void**)&line);
	}
	hist = put_in_history(hist, str);
	if (str && ft_strcmp(str, "") && (ret = sh_check_str(str)) == 1)
		command = sh_strsplit_m(str, ';');
	ft_memdel((void**)&str);
	sh_error_parse(ret);
	return (command);
}
