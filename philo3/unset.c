/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 00:30:53 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 19:35:59 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	unset_shared(t_shared *shared, t_rules *rules)
{
	pthread_mutex_destroy(&shared->all_alive_m);
	pthread_mutex_destroy(&shared->start_m);
	unset_fork(shared, rules->nb_philos);
}

void	unset_fork(t_shared *shared, int nb_of_forks)
{
	while (--nb_of_forks >= 0)
		pthread_mutex_destroy(&shared->forks[nb_of_forks].fork_m);
	free(shared->forks);
}

void	stop_and_unset_philos(t_shared *shared, int nb_of_threads,
	int rule_nb_philo, bool init)
{
	int	i;

	pthread_mutex_lock(&shared->all_alive_m);
	shared->all_alive = false;
	pthread_mutex_unlock(&shared->all_alive_m);
	if (init)
	{
		pthread_mutex_lock(&shared->start_m);
		shared->start_t = 1;
		pthread_mutex_unlock(&shared->start_m);
	}
	i = -1;
	while (++i < nb_of_threads)
	{
		release_fork(shared->forks, i);
		pthread_join((((t_philo *)(shared->philos))[i]).thread, 0);
		pthread_mutex_destroy(&(((t_philo *)(shared->philos))[i]).data_eat_m);
	}
	while (i < rule_nb_philo)
	{
		pthread_mutex_destroy(&(((t_philo *)(shared->philos))[i]).data_eat_m);
		i++;
	}
	free(shared->philos);
	shared->philos = NULL;
}
