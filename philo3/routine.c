/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 02:28:45 by acardona          #+#    #+#             */
/*   Updated: 2023/07/19 19:03:58 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	_routine_eat(t_philo *philo);
static int	_check_alive(t_philo *philo);
static int	_routine_sleep(t_philo *philo);
static void	_routine_think(t_philo *philo);

void	*thread_routine(void *philo_)
{
	t_philo	*philo;

	philo = (t_philo *)philo_;
	pthread_mutex_lock(&philo->shared->start_m);
	philo->t0 = philo->shared->start_t;
	pthread_mutex_unlock(&philo->shared->start_m);
	if (philo->t0 < 0)
		return (NULL);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->t_last_eat = philo->t0;
	pthread_mutex_unlock(&philo->data_eat_m);
	if (philo->id % 2 == 1)
		my_usleep(10000);
	pthread_mutex_lock(&philo->shared->all_alive_m);
	while (philo->shared->all_alive)
	{
		pthread_mutex_unlock(&philo->shared->all_alive_m);
		if (!_routine_eat(philo) && !_routine_sleep(philo))
			_routine_think(philo);
		pthread_mutex_lock(&philo->shared->all_alive_m);
	}
	pthread_mutex_unlock(&philo->shared->all_alive_m);
	return (NULL);
}

static int	_routine_eat(t_philo *philo)
{
	size_t	t_now;
	size_t	t_last_eat_loc;

	pthread_mutex_lock(&philo->shared->fork_m[(philo->id - 1
			+ (philo->id == philo->rules.nb_philos)) % philo->rules.nb_philos]);
	t_now = get_time_ms();
	print_msg(philo, t_now, MSG_FORK);
	if (!_check_alive(philo))
		return (pthread_mutex_unlock(&philo->shared->fork_m[(philo->id - 1
						+ (philo->id == philo->rules.nb_philos))
					% philo->rules.nb_philos]), 1);
	pthread_mutex_lock(&philo->shared->fork_m[(philo->id
			- (philo->id == philo->rules.nb_philos)) % philo->rules.nb_philos]);
	t_now = get_time_ms();
	(print_msg(philo, t_now, MSG_FORK), print_msg(philo, t_now, MSG_EAT));
	pthread_mutex_lock(&philo->data_eat_m);
	philo->t_last_eat = t_now;
	pthread_mutex_unlock(&philo->data_eat_m);
	t_last_eat_loc = t_now;
	while (t_now < t_last_eat_loc + philo->rules.t_eat)
		(usleep(USLEEP_RESOLUTION_US), t_now = get_time_ms());
	pthread_mutex_unlock(&philo->shared->fork_m[(philo->id - 1
			+ (philo->id == philo->rules.nb_philos)) % philo->rules.nb_philos]);
	pthread_mutex_unlock(&philo->shared->fork_m[(philo->id
			- (philo->id == philo->rules.nb_philos)) % philo->rules.nb_philos]);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->cpt_eat++;
	return (pthread_mutex_unlock(&philo->data_eat_m), 0);
}

static int	_check_alive(t_philo *philo)
{
	int	rtn;

	pthread_mutex_lock(&philo->shared->all_alive_m);
	rtn = (philo->shared->all_alive != 0);
	pthread_mutex_unlock(&philo->shared->all_alive_m);
	return (rtn);
}

static int	_routine_sleep(t_philo *philo)
{
	size_t	t_now;

	if (!_check_alive(philo))
		return (1);
	t_now = get_time_ms();
	print_msg(philo, t_now, MSG_SLEEP);
	my_usleep(philo->rules.t_sleep * 1000);
	return (0);
}

static void	_routine_think(t_philo *philo)
{
	size_t	t_now;

	t_now = get_time_ms();
	print_msg(philo, t_now, MSG_THINK);
}
