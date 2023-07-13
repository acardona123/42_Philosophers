/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 23:06:15 by acardona          #+#    #+#             */
/*   Updated: 2023/07/12 23:14:25 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	eat(t_philo *philo, size_t t_now, size_t *t_last_eat,
	char *have_forks)
{
	int	i;

	i = -1;
	usleep (100);
	while (++i < 5)
	{
		usleep(200);
		if (!have_forks[1] && take_a_fork(philo->shared, philo->id - 1))
			(print_msg(philo, t_now, MSG_FORK), have_forks[1] = 1);
		if (!have_forks[0] && take_a_fork(philo->shared,
				philo->id % philo->rules.nb_philos))
			(print_msg(philo, t_now, MSG_FORK), have_forks[0] = 1);
		if (have_forks[0] && have_forks[1])
		{
			philo->status = S_EAT;
			print_msg(philo, t_now, MSG_EAT);
			*t_last_eat = t_now;
			return (true);
		}
	}
	return (false);
}

bool	take_a_fork(t_shared *shared, int id_fork)
{
	pthread_mutex_lock(shared->fork_m + id_fork);
	if (shared->fork_ok[id_fork])
	{
		shared->fork_ok[id_fork] = false;
		pthread_mutex_unlock(shared->fork_m + id_fork);
		return (true);
	}
	pthread_mutex_unlock(shared->fork_m + id_fork);
	return (false);
}

bool	give_a_fork(t_shared *shared, int id_fork)
{
	pthread_mutex_lock(shared->fork_m + id_fork);
	shared->fork_ok[id_fork] = true;
	pthread_mutex_unlock(shared->fork_m + id_fork);
	return (false);
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