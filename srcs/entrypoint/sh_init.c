/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboye <maboye@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/22 16:19:19 by thdelmas          #+#    #+#             */
/*   Updated: 2019/05/11 17:32:07 by maboye           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

t_sh		*sh_init(const char **env)
{
	t_sh	*sh;

	if (!(sh = (t_sh *)malloc(sizeof(t_sh))))
		return (NULL);
	sh->envp = sh_init_envp(env);
	sh->local = NULL;
	sh->ln_history = NULL;
	sh->ln = NULL;
	return (sh);
}