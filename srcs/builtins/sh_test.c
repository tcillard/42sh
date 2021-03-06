/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ede-ram <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 13:17:07 by ede-ram           #+#    #+#             */
/*   Updated: 2020/02/13 23:09:20 by jerry            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh_env.h"
#include "libft.h"
#include "sh_builtins.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define UN_OP "-b -c -d -e -f -n -p -S -s -z -L -r -w -x"
#define BI_OP "= != -eq -ne -gt -ge -lt -le"
#define NUM_SET "+-0123456789"

int		sh_test_binary(char *s1, char *s2, char *s3)
{
	if (!ft_strcmp(s1, "!"))
		return (!sh_test_unary(s2, s3));
	else if (!ft_strcmp(s1, "(") && !ft_strcmp(s3, ")"))
		return (sh_test_unary(NULL, s2));
	else if (!ft_strcmp(s2, "=") && !ft_strcmp(s1, s3))
		return (0);
	else if (!ft_strcmp(s2, "!=") && ft_strcmp(s1, s3))
		return (0);
	else if ((!ft_isxstr(NUM_SET, s1) || !ft_isxstr(NUM_SET, s3))
			&& ft_strcmp(s2, "!=") && ft_strcmp(s2, "="))
		return (2);
	else if (!ft_strcmp(s2, "-eq") && ft_atoi(s1) == ft_atoi(s3))
		return (0);
	else if (!ft_strcmp(s2, "-ne") && ft_atoi(s1) != ft_atoi(s3))
		return (0);
	else if (!ft_strcmp(s2, "-gt") && ft_atoi(s1) > ft_atoi(s3))
		return (0);
	else if (!ft_strcmp(s2, "-ge") && ft_atoi(s1) >= ft_atoi(s3))
		return (0);
	else if (!ft_strcmp(s2, "-lt") && ft_atoi(s1) < ft_atoi(s3))
		return (0);
	else if (!ft_strcmp(s2, "-le") && ft_atoi(s1) <= ft_atoi(s3))
		return (0);
	return (1);
}

int		sh_test_and_or(int ac, char **av, t_env **ev)
{
	int		j[2];

	j[0] = 0;
	j[1] = 0;
	while (++j[0] < ac)
	{
		if (!ft_strcmp("(", av[j[0]]))
			j[1]++;
		else if (!ft_strcmp(")", av[j[0]]))
			j[1]--;
		else if (!j[1] && !ft_strcmp("-a", av[j[0]]))
			return (sh_test(j[0], av, ev) || sh_test(ac - j[0], av + j[0], ev));
	}
	j[0] = 0;
	j[1] = 0;
	while (++j[0] < ac)
	{
		if (!ft_strcmp("(", av[j[0]]))
			j[1]++;
		else if (!ft_strcmp(")", av[j[0]]))
			j[1]--;
		else if (!j[1] && !ft_strcmp("-o", av[j[0]]))
			return (sh_test(j[0], av, ev) && sh_test(ac - j[0], av + j[0], ev));
	}
	return (-1);
}

int		sh_test_braces(int ac, char **av, int i, int ret)
{
	int j;

	j = i;
	while (++i < ac && ft_strcmp(av[i], ")"))
		;
	if (i >= ac)
	{
		ft_putendl_fd("Missing )", 2);
		return (1);
	}
	else if (i == ac - 1)
		ret = sh_test(i - j, av + j, NULL);
	if (i < ac - 2 && !ft_strcmp(av[i + 1], "-a"))
		return (ret == 0 && sh_test(ac - 1, av + 1, NULL) == 0 ? 0 : 1);
	else if (i < ac - 2 && !ft_strcmp(av[i + 1], "-o"))
		return (ret == 0 || sh_test(ac - 1, av + 1, NULL) == 0 ? 0 : 1);
	else
		return (ret);
}

int		sh_test_sub(int ac, char **av, int i, int ret)
{
	if (i < ac - 3 && !ft_strcmp(av[i], "!")
			&& ft_strstr(BI_OP, av[i + 2]))
		ret = (sh_test_binary(av[i + 1], av[i + 2], av[i + 3]) ? 1 : 0);
	else if (i < ac - 3 && !ft_strcmp(av[i], "(")
			&& !ft_strcmp(av[i + 3], ")") && ft_strstr(BI_OP, av[i + 1]))
		ret = sh_test_unary(av[i + 1], av[i + 2]);
	else if (i < ac - 4 && !ft_strcmp(av[i], "(")
			&& !ft_strcmp(av[i + 4], ")") && ft_strstr(BI_OP, av[i + 2]))
		ret = (sh_test_binary(av[i + 1], av[i + 2], av[i + 3]));
	else if (i < ac - 1 && !ft_strcmp(av[i], "("))
		ret = sh_test_braces(ac, av, i, ret);
	else
		ret = 2;
	return (ret);
}

int		sh_test(int ac, char **av, t_env **ev)
{
	int		ret;
	int		i;

	(void)ev;
	i = 1;
	ret = 1;
	if ((ret = sh_test_and_or(ac, av, ev)) != -1)
		return (ret);
	else if (ac >= 2 && !ft_strcmp(av[i], "!"))
		ret = !(sh_test(ac - 1, av + 1, ev));
	else if (ac == 2)
		ret = (sh_test_unary(NULL, av[i]));
	else if (i < ac - 1 && ft_strstr(UN_OP, av[i]))
		ret = (sh_test_unary(av[i], av[i + 1]));
	else if (i < ac - 2 && ft_strstr(BI_OP, av[i + 1]))
		ret = (sh_test_binary(av[i], av[i + 1], av[i + 2]));
	else
		ret = sh_test_sub(ac, av, i, ret);
	if (ret == 2)
		ft_putendl_fd("42sh: test: bad usage", 2);
	return (ret);
}
