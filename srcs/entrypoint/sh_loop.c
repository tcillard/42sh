/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_loop.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/04 17:32:52 by thdelmas          #+#    #+#             */
/*   Updated: 2019/07/17 16:20:56 by thdelmas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"
#include "tmp.h"
#include "libft.h"
#include "sh_env.h"
#include "sh_entrypoint.h"
#include "sh_command_line.h"
#include "history.h"
#include "sh_env.h"

#include <stdio.h>

void    print_all_tokens(t_sh *p, t_token *t, int lvl)
{
	int lvcpy;
	while (t)
	{
		lvcpy = lvl;
		while (lvcpy--)
			dprintf(p->debug_fd, "%c      ", (lvcpy == 0) ? '|' : ' ');
		if (t->content)
			dprintf(p->debug_fd, "%-10s (%i)\n", t->content, t->type);
		else
			dprintf(p->debug_fd, "%-10s (%i)\n", "", t->type);
		if (t->sub)
		{
			print_all_tokens(p, t->sub, lvl + 1);
		}
		t = t->next;
	}
}

static t_hist 	*init_history(void)
{
	t_hist *hist;

	hist = malloc(sizeof(t_hist));
 	hist->path = find_path();
	hist->size_l = 200;
	hist = command_history(hist);
	return (hist);
}

int		sh_loop(void)
{
	char	**ln_tab;
	t_hist	*hist;
	t_sh	*p;
	char	*input;

	ft_putendl("Shell: sh_loop.c\n---");
	p = sh();
	ask_for_extern_terminal(p);
	hist = init_history();
	int	complete;
	while (42)
	{
		sh_prompt();
		ln_tab = NULL;
		int dbug = 0;
		complete = 0;
		input = 0;
		while (!complete)
		{
			if (!dbug)
			{
				if (!(ln_tab = sh_arguments(hist)) || !*ln_tab)
					break ;
			}
			else
			{
				ln_tab = malloc(2 * sizeof(char*));
		//	ln_tab[0] = ft_strdup("/bin/echo lala && /bin/ls -la && CTA 3");
		//	ln_tab[0] = ft_strdup("yolo () { echo lala ; }");
		//	ln_tab[0] = ft_strdup("/bin/cat tet");
		//	ln_tab[0] = ft_strdup("for lala in po la ka nu ; do /bin/echo $lala ; done");
		//	ln_tab[0] = ft_strdup("case yolo in yola ) echo ;; yali | yolo ) loul;;(po )tu ;esac");
		//	ln_tab[0] = ft_strdup("case yoz in ( lap | yoz ) /bin/echo yes ;esac");
		//	ln_tab[0] = ft_strdup("  echo ; done");
				ln_tab[0] = ft_strdup("w");
				ln_tab[1] = 0;
			}
		//	int z = 0;
		//	while (ln_tab[z])
		//		fprintf(stderr, "[%s]\n", ln_tab[z++]);
		//	if (!*ln_tab || !ft_strncmp("exit", *ln_tab, 4))
		//		break ;
		//	else
			input = ft_strjoin_free(input, sh_tab_fusion(ln_tab), input);
			ft_tab_strdel(&ln_tab);
			p->unfinished_cmd = 0;
			if (p->ast = tokenize_input(input))//line
			{
				print_all_tokens(p, p->ast, 0);
				exec_script(p, p->ast, 0);
			}
			if (!p->unfinished_cmd)
				complete = 1;
		}
		free(input);
	}
	push_history(hist);
	return (1);
}
