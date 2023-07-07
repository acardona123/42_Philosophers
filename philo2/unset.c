/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 00:30:53 by acardona          #+#    #+#             */
/*   Updated: 2023/07/06 22:37:46 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	unset_forks(t_shared *shared, int nb_of_forks)
{
	while (--nb_of_forks >= 0)
		pthread_mutex_destroy(&shared->fork_m[nb_of_forks]);
	pthread_mutex_destroy(&shared->fork_m[nb_of_forks]);
	free(shared->fork_m);
	free(shared->fork_ok);
}

void	stop_and_unset_philos(t_shared *shared, int nb_of_philo, bool init)
{
	int	i;

	pthread_mutex_lock(&shared->disp_m);
	shared->disp_ok = false;
	pthread_mutex_unlock(&shared->disp_m);
	i = 0;
	while (i < nb_of_philo)
	{
		pthread_mutex_lock(&(((t_philo *)(shared->philos))[i]).life_ctrl_m);
		(((t_philo *)(shared->philos))[i]).life_ctrl = LC_DEAD;
		pthread_mutex_unlock(&(((t_philo *)(shared->philos))[i]).life_ctrl_m);
		i++;
	}
	if (init)
		pthread_mutex_unlock(&shared->start_m);
	while (--i > -1)
	{
		pthread_join((((t_philo *)(shared->philos))[i]).thread, 0);
		pthread_mutex_destroy(&(((t_philo *)(shared->philos))[i]).life_ctrl_m);
	}
	free(shared->philos);
	shared->philos = NULL;
}

void	unset_shared(t_shared *shared, t_rules *rules)
{
	pthread_mutex_destroy(&shared->disp_m);
	pthread_mutex_destroy(&shared->start_m);
	unset_forks(shared, rules->nb_philos);
}
