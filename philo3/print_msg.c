/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_msg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:05:39 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 16:56:21 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	_print_msg_no_color(t_philo *philo, size_t t_now,
				t_msg_type msg_type);
static void	_print_msg_color(t_philo *philo, size_t t_now, t_msg_type msg_type);

void	print_msg(t_philo *philo, size_t t_now, t_msg_type msg_type)
{
	if (COLOR)
		_print_msg_color(philo, t_now, msg_type);
	else
		_print_msg_no_color(philo, t_now, msg_type);
}

static void	_print_msg_no_color(t_philo *philo, size_t t_now,
t_msg_type msg_type)
{
	char	*msg;

	if (msg_type == MSG_DIED)
		msg = "died";
	else if (msg_type == MSG_EAT)
		msg = "is eating";
	else if (msg_type == MSG_SLEEP)
		msg = "is sleeping";
	else if (msg_type == MSG_THINK)
		msg = "is thinking";
	else if (msg_type == MSG_FORK)
		msg = "has taken a fork";
	else if (msg_type == MSG_DEBUG)
		msg = "debug";
	else if (msg_type == MSG_ERROR)
		msg = "forks thread error";
	pthread_mutex_lock(&philo->shared->all_alive_m);
	if (philo->shared->all_alive)
		printf("%zu %d %s\n", t_now - philo->t0, philo->id, msg);
	if (msg_type == MSG_DIED || msg_type == MSG_ERROR)
		philo->shared->all_alive = false;
	pthread_mutex_unlock(&philo->shared->all_alive_m);
}

static void	_print_msg_color(t_philo *philo, size_t t_now, t_msg_type msg_type)
{
	char	*msg;

	if (msg_type == MSG_DIED)
		msg = "died";
	else if (msg_type == MSG_EAT)
		msg = "is eating";
	else if (msg_type == MSG_SLEEP)
		msg = "is sleeping";
	else if (msg_type == MSG_THINK)
		msg = "is thinking";
	else if (msg_type == MSG_FORK)
		msg = "has taken a fork";
	else if (msg_type == MSG_DEBUG)
		msg = "debug";
	else if (msg_type == MSG_ERROR)
		msg = "forks thread error";
	pthread_mutex_lock(&philo->shared->all_alive_m);
	if (philo->shared->all_alive)
		printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s\e[0m\n", t_now - philo->t0,
			philo->id, philo->id, msg_type, msg);
	if (msg_type == MSG_DIED || msg_type == MSG_ERROR)
		philo->shared->all_alive = false;
	pthread_mutex_unlock(&philo->shared->all_alive_m);
}

void	printf_end_msg(t_shared *shared)
{
	pthread_mutex_lock(&shared->all_alive_m);
	shared->all_alive = false;
	printf("\n%zu philos all fed\n",
		get_time_ms() - ((t_philo *)shared->philos)[0].t0);
	pthread_mutex_unlock(&shared->all_alive_m);
}
