/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 23:39:03 by alexcardona       #+#    #+#             */
/*   Updated: 2023/07/07 03:36:50 by acardona         ###   ########.fr       */
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
	if (nptr[0] == '-')
		return (1);
	i = (nptr[0] == '+');
	if (ft_strlen(nptr + i) > 10)
		return (1);
	nb = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		nb = nb * 10 + nptr[i] - '0';
		i++;
	}
	if (nb > INT_MAX || nb < INT_MIN)
		return (1);
	*dst = (int) nb;
	return (0);
}

int	ft_atozu_ptr(const char *nptr, size_t *dst)
{
	int		i;
	ssize_t	nb;

	if (!nptr || !dst)
		return (1);
	if (nptr[0] == '-')
		return (1);
	i = (nptr[0] == '+');
	if (ft_strlen(nptr + i) > 10)
		return (1);
	nb = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		nb = nb * 10 + nptr[i] - '0';
		i++;
	}
	if (nb > INT_MAX || nb < INT_MIN)
		return (1);
	*dst = (size_t) nb;
	return (0);
}

size_t	ft_max_zu(size_t n1, size_t n2)
{
	if (n1 > n2)
		return (n1);
	return (n2);
}

size_t	get_time_ms(void)
{
	t_timer	now;

	gettimeofday(&now.tv, &now.tz);
	return (1000 * now.tv.tv_sec + now.tv.tv_usec / 1000);
}

// /*//no colors
void	print_msg(t_philo *philo, size_t t_now, t_msg_type msg_type)
{
	char	*msg;

	if (msg_type == MSG_DEBUG)
		msg = "release forks";
	else if (msg_type == MSG_DIED)
		msg = "died";
	else if (msg_type == MSG_EAT)
		msg = "is eating";
	else if (msg_type == MSG_SLEEP)
		msg = "is sleeping";
	else if (msg_type == MSG_THINK)
		msg = "is thinking";
	else
		msg = "has taken a fork";
	pthread_mutex_lock(&philo->shared->disp_m);
	if (philo->shared->disp_ok)
		printf("%zu %d %s\n", t_now - philo->t0, philo->id, msg);
	if (msg_type == MSG_DIED)
		philo->shared->disp_ok = false;
	pthread_mutex_unlock(&philo->shared->disp_m);
}
// */
/*
void	print_msg(t_philo *philo, size_t t_now, t_msg_type msg_type)
{
	char	*msg;

	if (msg_type == MSG_DEBUG)
		msg = "release forks";
	else if (msg_type == MSG_DIED)
		msg = "died";
	else if (msg_type == MSG_EAT)
		msg = "is eating";
	else if (msg_type == MSG_SLEEP)
		msg = "is sleeping";
	else if (msg_type == MSG_THINK)
		msg = "is thinking";
	else
		msg = "has taken a fork";
	pthread_mutex_lock(&philo->shared->disp_m);
	if (philo->shared->disp_ok)
		printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s\e[0m\n", t_now - philo->t0,
			philo->id, philo->id, msg_type, msg);
	if (msg_type == MSG_DIED)
		philo->shared->disp_ok = false;
	pthread_mutex_unlock(&philo->shared->disp_m);
}
*/

void	print_msg_fork(t_philo *philo, size_t t_now, t_msg_type msg_type, int data)
{
	char	*msg;

	if (msg_type == MSG_FORK)
		msg = "has taken the fork";
	else
		msg = "released the fork";
	pthread_mutex_lock(&philo->shared->disp_m);
	if (philo->shared->disp_ok)
		printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s (%d)\e[0m\n", t_now - philo->t0,
			philo->id, philo->id, msg_type, msg, data + 1);
	pthread_mutex_unlock(&philo->shared->disp_m);
}
