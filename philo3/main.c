/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:37:57 by acardona          #+#    #+#             */
/*   Updated: 2023/07/19 18:53:24 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	_check_philos(t_rules rules, t_philo *philos);

int	main(int ac, char **av)
{
	t_rules		rules;
	t_shared	shared;

	rules = (t_rules){0};
	shared = (t_shared){0};
	if (init_and_start(ac, av, &rules, &shared))
		return (1);
	my_usleep(19000);
	_check_philos(rules, (t_philo *)(shared.philos));
	stop_and_unset_philos(&shared, rules.nb_philos, false);
	unset_shared(&shared, &rules);
	return (0);
}

static void	_check_philos(t_rules rules, t_philo *philos)
{
	int		cpt;
	int		i;
	size_t	t_now;

	cpt = 0;
	while (cpt >= 0 && (rules.nb_meals < 0 || cpt < rules.nb_meals))
	{
		cpt = 0;
		i = -1;
		while (cpt >= 0 && ++i < rules.nb_philos)
		{
			t_now = get_time_ms();
			pthread_mutex_lock(&philos[i].data_eat_m);
			if (rules.nb_meals >= 0 && philos[i].cpt_eat >= rules.nb_meals)
				cpt ++;
			if (t_now >= rules.t_die + philos[i].t_last_eat)
				(print_msg(&philos[i], t_now, MSG_DIED), cpt = -1);
			pthread_mutex_unlock(&philos[i].data_eat_m);
		}
	}
}
