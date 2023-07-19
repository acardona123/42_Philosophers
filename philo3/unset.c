/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 00:30:53 by acardona          #+#    #+#             */
/*   Updated: 2023/07/19 18:25:21 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	unset_forks(t_shared *shared, int nb_of_forks)
{
	while (--nb_of_forks >= 0)
		pthread_mutex_destroy(&shared->fork_m[nb_of_forks]);
	free(shared->fork_m);
}

void	stop_and_unset_philos(t_shared *shared, int nb_of_philo, bool init)
{
	int	i;

	pthread_mutex_lock(&shared->all_alive_m);
	shared->all_alive = false;
	pthread_mutex_unlock(&shared->all_alive_m);
	if (init)
		pthread_mutex_unlock(&shared->start_m);
	i = nb_of_philo;
	while (--i > -1)
	{
		pthread_join((((t_philo *)(shared->philos))[i]).thread, 0);
		pthread_mutex_destroy(&(((t_philo *)(shared->philos))[i]).data_eat_m);
	}
	free(shared->philos);
	shared->philos = NULL;
}

void	unset_shared(t_shared *shared, t_rules *rules)
{
	pthread_mutex_destroy(&shared->all_alive_m);
	pthread_mutex_destroy(&shared->start_m);
	unset_forks(shared, rules->nb_philos);
}
