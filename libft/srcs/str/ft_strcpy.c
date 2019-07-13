/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/09 21:23:44 by thdelmas          #+#    #+#             */
/*   Updated: 2019/07/13 18:09:36 by thdelmas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strcpy(char *dest, const char *src)
{
	size_t n;

	n = 0;
	while (src[n])
	{
		dest[n] = src[n];
		n++;
	}
	dest[n] = 0;
	return (dest);
}
