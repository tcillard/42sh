/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destructor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerry <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 15:01:31 by jerry             #+#    #+#             */
/*   Updated: 2019/11/13 00:17:08 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"
#include "libft.h"

void	destructor(void)
{
	t_sh	*s;

	if ((s = sh()))
	{
		push_history(sh()->hist);
		if (sh()->is_interactive && sh()->pid_main_process == getpid())
			tcsetattr(0, TCSADRAIN, &sh()->extern_termios);
		sh_shdel(&s);
		printf("[%i]SUICIDE\n", getpid());
	}
}