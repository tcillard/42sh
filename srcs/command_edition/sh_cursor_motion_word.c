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

#include "libft.h"
#include "sh_command_edition.h"
#include "sh.h"

static int	sh_movecursor_down(void)
{
	tputs(tgetstr("do", NULL), 0, sh_outc);
	tputs(tgetstr("cr", NULL), 0, sh_outc);
	return (1);
}

static int	sh_forward_word(char **command, int i, t_pos term, t_pos cursor)
{
	while (i == -1 || (command[0][i + 1] > 32 && command[0][i + 1] != '\0'))
	{
		if (cursor.col == term.col)
			cursor.col = sh_movecursor_down();
		else
		{
			ft_putstr_fd("\033[1C", 0);
			cursor.col++;
		}
		i++;
	}
	while (i == -1 || (command[0][i + 1] != '\0' && command[0][i + 1] <= 32))
	{
		if (cursor.col == term.col)
			cursor.col = sh_movecursor_down();
		else
		{
			ft_putstr_fd("\033[1C", 0);
			cursor.col++;
		}
		i++;
	}
	return (i);
}

static void	sh_movecursor_up(t_pos *cursor, t_pos term)
{
	int j;

	j = -1;
	tputs(tgetstr("up", NULL), 0, sh_outc);
	while (++j < term.col)
		ft_putstr_fd("\033[1C", 0);
	cursor->rows--;
	cursor->col = term.col;
	j = -1;
}

static int	sh_backward_word(char **command, int i, t_pos term, t_pos cursor)
{
	while (i > -1 && command[0][i] <= 32)
	{
		if (cursor.col == 1)
			sh_movecursor_up(&cursor, term);
		else
		{
			ft_putstr_fd("\033[1D", 0);
			cursor.col--;
		}
		i--;
	}
	while (i > -1 && command[0][i] > 32)
	{
		if (cursor.col == 1)
			sh_movecursor_up(&cursor, term);
		else
		{
			ft_putstr_fd("\033[1D", 0);
			cursor.col--;
		}
		i--;
	}
	return (i);
}

int			sh_cursor_motion_word(char **command, char *buf,
			int i, t_hist *hist)
{
	t_pos	cursor;
	t_pos	term;

	sh_cursor_position(&cursor);
	term.rows = tgetnum("li");
	term.col = tgetnum("co");
	if (sh()->buselect)
		reset_selection(command, i, hist);
	if (buf[5] == 'C')
		i = sh_forward_word(command, i, term, cursor);
	else if (buf[5] == 'D')
		i = sh_backward_word(command, i, term, cursor);
	return (i);
}
