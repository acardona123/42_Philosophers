/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:46:48 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 19:34:41 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_fork(t_fork *forks, int id_fork)
{
	while (1)
	{
		pthread_mutex_lock(&forks[id_fork].fork_m);
		if (forks[id_fork].fork_dispo == true)
			break ;
		pthread_mutex_unlock(&forks[id_fork].fork_m);
		usleep(USLEEP_RESOLUTION_US);
	}
	forks[id_fork].fork_dispo = false;
	pthread_mutex_unlock(&forks[id_fork].fork_m);
}

void	release_fork(t_fork *forks, int id_fork)
{
	pthread_mutex_lock(&forks[id_fork].fork_m);
	forks[id_fork].fork_dispo = true;
	pthread_mutex_unlock(&forks[id_fork].fork_m);
}
