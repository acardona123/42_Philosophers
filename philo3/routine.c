/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 02:28:45 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 18:30:06 by acardona         ###   ########.fr       */
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
	// printf("philo %3d started with t0 = %zu\n", philo->id, philo->loc_t_last_eat);//
	if (philo->rules.nb_philos == 1)
		return (print_msg(philo, philo->loc_t_last_eat, MSG_FORK), NULL);
	my_usleep((philo->rules.t_eat * 1000 + 400) * philo->group);
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

	t_now = get_time_ms();
	if (t_now - philo->loc_t_last_eat > philo->rules.t_die)
		return (usleep(USLEEP_CHECKER_US * 3), 1);
	pthread_mutex_lock(&philo->shared->fork_m[philo->fork_id_l]);
	print_msg(philo, t_now, MSG_FORK);
	pthread_mutex_lock(&philo->shared->fork_m[philo->fork_id_r]);
	print_msg(philo, t_now, MSG_FORK);
	print_msg(philo, t_now, MSG_EAT);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->data_eat_t_last = t_now;
	pthread_mutex_unlock(&philo->data_eat_m);
	philo->loc_t_last_eat = t_now;
	// printf("-------------t_eat : %d -> %zu\n", philo->id, philo->loc_t_last_eat - philo->t0);//
	while (t_now < philo->loc_t_last_eat + philo->rules.t_eat)
		(usleep(USLEEP_RESOLUTION_US), t_now = get_time_ms());
	pthread_mutex_unlock(&philo->shared->fork_m[philo->fork_id_r]);
	pthread_mutex_unlock(&philo->shared->fork_m[philo->fork_id_l]);
	pthread_mutex_lock(&philo->data_eat_m);
	philo->data_eat_cpt++;
	pthread_mutex_unlock(&philo->data_eat_m);
	return (0);
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
	my_usleep((philo->rules.t_sleep + philo->rules.t_eat
			- (t_now - philo->loc_t_last_eat)) * 1000);
	return (0);
}

static void	_routine_think(t_philo *philo)
{
	size_t	t_now;

	if (!_check_alive(philo))
		return ;
	t_now = get_time_ms();
	print_msg(philo, t_now, MSG_THINK);
	my_usleep((philo->rules.t_cycle - (t_now - philo->loc_t_last_eat)) * 1000);
}
