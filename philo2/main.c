/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:37:57 by acardona          #+#    #+#             */
/*   Updated: 2023/07/07 00:23:15 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	_check_philos_alive(t_rules rules, t_philo *philos);

int	main(int ac, char **av)
{
	t_rules		rules;
	t_shared	shared;

	rules = (t_rules){0};
	shared = (t_shared){0};
	if (init_and_start(ac, av, &rules, &shared))
		return (1);
	_check_philos_alive(rules, (t_philo *)(shared.philos));
	stop_and_unset_philos(&shared, rules.nb_philos, false);
	unset_shared(&shared, &rules);
	return (0);
}

static void	_check_philos_alive(t_rules rules, t_philo *philos)
{
	int				i;
	int				cpt_fed;
	bool			all_alive;
	t_life_controle	lc;

	all_alive = true;
	cpt_fed = 0;
	while (all_alive && cpt_fed != rules.nb_philos)
	{
		cpt_fed = 0;
		i = -1;
		while (all_alive && ++i < rules.nb_philos)
		{
			pthread_mutex_lock(&philos[i].life_ctrl_m);
			lc = philos[i].life_ctrl;
			pthread_mutex_unlock(&philos[i].life_ctrl_m);
			all_alive = (lc != LC_DEAD);
			cpt_fed += (lc == LC_FED);
		}
		usleep(300);
	}
}
