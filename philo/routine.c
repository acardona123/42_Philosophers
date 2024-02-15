/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 02:28:45 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 19:35:13 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	_thread_routine_loop(t_philo *philo);
static int	_routine_eat(t_philo *philo);
static int	_routine_sleep(t_philo *philo);
static void	_routine_think(t_philo *philo);

void	*thread_routine(void *philo_)
{
	t_philo	*philo;

	philo = (t_philo *)philo_;
	while (philo->t0 == 0)
	{
		pthread_mutex_lock(&philo->shared->start_m);
		philo->t0 = philo->shared->start_t;
		pthread_mutex_unlock(&philo->shared->start_m);
		usleep(USLEEP_RESOLUTION_US);
	}
	if (philo->t0 == 1)
		return (NULL);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->data_eat_t_last = philo->t0;
	pthread_mutex_unlock(&philo->data_eat_m);
	philo->loc_t_last_eat = philo->t0;
	if (philo->rules.nb_philos == 1)
		return (print_msg(philo, philo->loc_t_last_eat, MSG_FORK), NULL);
	my_usleep((philo->rules.t_eat * 1000 + 400) * philo->group);
	_thread_routine_loop(philo);
	return (NULL);
}

static void	_thread_routine_loop(t_philo *philo)
{
	pthread_mutex_lock(&philo->shared->all_alive_m);
	while (philo->shared->all_alive)
	{
		pthread_mutex_unlock(&philo->shared->all_alive_m);
		if (!_routine_eat(philo) && !_routine_sleep(philo))
			_routine_think(philo);
		pthread_mutex_lock(&philo->shared->all_alive_m);
	}
	pthread_mutex_unlock(&philo->shared->all_alive_m);
}

static int	_routine_eat(t_philo *philo)
{
	size_t	t_now;

	t_now = get_time_ms();
	if (t_now - philo->loc_t_last_eat > philo->rules.t_die)
		return (usleep(USLEEP_CHECKER_US * 3), 1);
	take_fork(philo->shared->forks, philo->fork_id_l);
	print_msg(philo, t_now, MSG_FORK);
	take_fork(philo->shared->forks, philo->fork_id_r);
	print_msg(philo, t_now, MSG_FORK);
	print_msg(philo, t_now, MSG_EAT);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->data_eat_t_last = t_now;
	pthread_mutex_unlock(&philo->data_eat_m);
	philo->loc_t_last_eat = t_now;
	while (t_now < philo->loc_t_last_eat + philo->rules.t_eat)
		(usleep(USLEEP_RESOLUTION_US), t_now = get_time_ms());
	release_fork(philo->shared->forks, philo->fork_id_l);
	release_fork(philo->shared->forks, philo->fork_id_r);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->data_eat_cpt++;
	pthread_mutex_unlock(&philo->data_eat_m);
	return (0);
}

static int	_routine_sleep(t_philo *philo)
{
	size_t	t_now;

	pthread_mutex_lock(&philo->shared->all_alive_m);
	if (philo->shared->all_alive == false)
		return (pthread_mutex_unlock(&philo->shared->all_alive_m), 1);
	pthread_mutex_unlock(&philo->shared->all_alive_m);
	t_now = get_time_ms();
	print_msg(philo, t_now, MSG_SLEEP);
	my_usleep((philo->rules.t_sleep + philo->rules.t_eat
			- (t_now - philo->loc_t_last_eat)) * 1000);
	return (0);
}

static void	_routine_think(t_philo *philo)
{
	size_t	t_now;

	pthread_mutex_lock(&philo->shared->all_alive_m);
	if (philo->shared->all_alive == false)
	{
		pthread_mutex_unlock(&philo->shared->all_alive_m);
		return ;
	}
	pthread_mutex_unlock(&philo->shared->all_alive_m);
	t_now = get_time_ms();
	print_msg(philo, t_now, MSG_THINK);
	my_usleep((philo->rules.t_cycle - (t_now - philo->loc_t_last_eat)) * 1000);
}
