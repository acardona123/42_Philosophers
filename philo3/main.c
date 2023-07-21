/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:37:57 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 16:54:14 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	_check_philos(t_rules rules, t_philo *philos);

int	main(int ac, char **av)
{
	t_rules		rules;
	t_shared	shared;

	rules = (t_rules){0};
	shared = (t_shared){0};
	if (init_and_start(ac, av, &rules, &shared))
		return (1);
	if (!_check_philos(rules, (t_philo *)(shared.philos)) && DISPLAY_END_MSG)
		printf_end_msg(&shared);
	stop_and_unset_philos(&shared, rules.nb_philos, rules.nb_philos, false);
	unset_shared(&shared, &rules);
	return (0);
}

static int	_check_philos(t_rules rules, t_philo *philos)
{
	int		cpt;
	int		i;
	size_t	t_now;

	cpt = 0;
	while (rules.nb_meals < 0 || cpt < rules.nb_philos)
	{
		(usleep(USLEEP_CHECKER_US), cpt = 0);
		i = -1;
		while (++i < rules.nb_philos)
		{
			t_now = get_time_ms();
			pthread_mutex_lock(&philos[i].data_eat_m);
			if (rules.nb_meals >= 0 && philos[i].data_eat_cpt >= rules.nb_meals)
				cpt ++;
			if (t_now >= philos[i].data_eat_t_last + rules.t_die)
			{
				print_msg(&philos[i], t_now, MSG_DIED);
				pthread_mutex_unlock(&philos[i].data_eat_m);
				return (1);
			}
			pthread_mutex_unlock(&philos[i].data_eat_m);
		}
	}
	return (0);
}
