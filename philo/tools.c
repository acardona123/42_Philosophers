/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexcardona <alexcardona@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 23:39:03 by alexcardona       #+#    #+#             */
/*   Updated: 2023/06/30 00:23:08 by alexcardona      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1 * *s2);
	if (!s2)
		return (*s1);
	i = 0;
	while (i < n && (s1[i] || s2[i]))
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

int	ft_atoi_ptr(const char *nptr, int *dst)
{
	int		i;
	ssize_t	nb;

	if (!nptr || !dst)
		return (1);
	i = (nptr[0] == '+' || nptr[0] == '-');
	if (ft_strlen(nptr + i) > 10)
		return (1);
	nb = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		nb = nb * 10 + nptr[i] - '0';
		i++;
	}
	if (nptr[0] == '-')
		nb *= -1;
	printf("in function : %zu\n", nb);
	if (nb > INT_MAX || nb < INT_MIN)
		return (1);
	*dst = (int) nb;
	return (0);
}
