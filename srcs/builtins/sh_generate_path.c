/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_generate_path.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcillard <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/13 23:05:28 by tcillard          #+#    #+#             */
/*   Updated: 2020/02/16 22:57:16 by tcillard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"
#include "sh_env.h"

void	sh_del_one_dir(char **new)
{
	int		i;
	int		j;
	char	*cpy;

	cpy = *new;
	j = 0;
	i = ft_strlen(cpy) - 1;
	if (i == 0)
		return ;
	if (cpy[i] == '/')
		i--;
	while (cpy[i] != '/')
		i--;
	if (!(*new = malloc(i + 1)))
		destructor(-1);
	while ((i == 0 && j <= i) || (i > 1 && j < i))
	{
		(*new)[j] = cpy[j];
		j++;
	}
	(*new)[j] = '\0';
	ft_memdel((void**)&cpy);
}

void	sh_add_one_dir(char **new, char *path, int *i_pa)
{
	int		i;
	int		j;
	char	*cpy;

	i = *i_pa;
	j = 0;
	cpy = *new;
	while (path[i] && path[i] != '/')
		i++;
	if (!(*new = malloc(i - *i_pa + ft_strlen(cpy) + 2)))
		destructor(-1);
	while (cpy && cpy[j])
	{
		(*new)[j] = cpy[j];
		j++;
	}
	(*new)[j++] = '/';
	while (path[*i_pa] && path[*i_pa] != '/')
		(*new)[j++] = path[(*i_pa)++];
	(*new)[j] = '\0';
	ft_memdel((void**)&cpy);
}

void	sh_find_new_path(char *path, char **new)
{
	int		i;

	i = 0;
	while (path[i])
	{
		while (path[i] && path[i] == '/')
			i++;
		if (path[i] == '.' && path[i + 1] == '.'
			&& (!path[i + 2] || path[i + 2] == '/'))
		{
			i = i + 2;
			sh_del_one_dir(new);
		}
		else if (path[i] == '.' && path[i + 1] == '/')
			i++;
		else
			sh_add_one_dir(new, path, &i);
		if (path[i])
			i++;
	}
}

void	sh_generate_path(char *path, int s1)
{
	char	*cpy;
	char	*new;

	new = NULL;
	cpy = sh()->pwd;
	if (path == NULL)
		return ;
	if (path[0] == '/')
		new = NULL;
	else
		new = ft_strdup(cpy);
	sh_find_new_path(path, &new);
	if (s1)
	{
		sh()->pwd = new;
		sh_setev("PWD", new);
		sh_setev("OLDPWD", cpy);
		ft_memdel((void**)&cpy);
	}
	else
	{
		ft_memdel((void**)&(sh()->potential_pwd));
		sh()->potential_pwd = new;
	}
}
