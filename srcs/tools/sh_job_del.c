/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_job_del.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 22:47:02 by thdelmas          #+#    #+#             */
/*   Updated: 2019/12/23 00:56:30 by tmeyer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signal.h"
#include "sh_types.h"
#include "sh.h"
#include "libft.h"

#include <stdlib.h>

void	sh_job_del(t_job **job)
{
	t_job *j;

	if (!job || !(j = *job))
		return ;
	sh_job_del(&(j->next));
	kill(j->pid, SIGINT);
	kill(j->pid, SIGKILL);
	sh_dprintf(1, "[%i] %s : KILLED\n", j->pid, j->name);
	ft_strdel(&(j->name));
	free(*job);
	*job = NULL;
	j = NULL;
}
