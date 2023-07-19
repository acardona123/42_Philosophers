/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_msg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:05:39 by acardona          #+#    #+#             */
/*   Updated: 2023/07/19 19:06:02 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*//no colors
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
	pthread_mutex_lock(&philo->shared->all_alive_m);
	if (philo->shared->all_alive)
		printf("%zu %d %s\n", t_now - philo->t0, philo->id, msg);
	if (msg_type == MSG_DIED)
		philo->shared->all_alive = false;
	pthread_mutex_unlock(&philo->shared->all_alive_m);
}
// // */
// /*
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
	pthread_mutex_lock(&philo->shared->all_alive_m);
	if (philo->shared->all_alive)
		printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s\e[0m\n", t_now - philo->t0,
			philo->id, philo->id, msg_type, msg);
	if (msg_type == MSG_DIED)
		philo->shared->all_alive = false;
	pthread_mutex_unlock(&philo->shared->all_alive_m);
}
// */