/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_debug_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/14 20:41:34 by thdelmas          #+#    #+#             */
/*   Updated: 2019/12/23 00:50:39 by tmeyer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_env.h"
#include "sh.h"

void	sh_debug_env(t_env **ev)
{
	t_env	*tmp;

	if (!ev)
		return ;
	tmp = *ev;
	while (tmp)
	{
		sh_dprintf(sh()->dbg_fd, "KEY = :%s:\t", tmp->key);
		sh_dprintf(sh()->dbg_fd, "VALUE = :%s:\t", tmp->value);
		sh_dprintf(sh()->dbg_fd, "EXPORTED = %i\t", tmp->exported);
		sh_dprintf(sh()->dbg_fd, "READONLY = %i\n", tmp->readonly);
		tmp = tmp->next;
	}
}
