/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:37:57 by acardona          #+#    #+#             */
/*   Updated: 2023/07/03 06:14:48 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int ac, char **av)
{
	t_rules		rules;
	t_shared	shared;
	bool		all_alive;
	int			cpt_fed;
	int			i;

	rules = (t_rules){0};
	shared = (t_shared){0};
	if (init_and_start(ac, av, &rules, &shared))
		return (1);
	printf("initialisation done\n\n");
	all_alive = true;
	cpt_fed = 0;
	while (all_alive && cpt_fed != rules.nb_philos)
	{
		cpt_fed = 0;
		i = -1;
		while (all_alive && ++i < rules.nb_philos)
		{
			pthread_mutex_lock(&((t_philo *)(shared.philos))[i].life_ctrl_m);
			all_alive = (((t_philo *)(shared.philos))[i].life_ctrl != LC_DEAD);
			cpt_fed += (((t_philo *)(shared.philos))[i].life_ctrl == LC_FED);
			pthread_mutex_unlock(&((t_philo *)(shared.philos))[i].life_ctrl_m);
		}
	}
	stop_and_unset_philos(&shared, rules.nb_philos, false);
	unset_shared(&shared, &rules);
	return (0);
}
