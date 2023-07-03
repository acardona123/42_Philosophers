/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initunset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 22:50:32 by acardona          #+#    #+#             */
/*   Updated: 2023/07/02 22:54:39 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @brief Initialisation of the set of rules
 * 
 * @param ac
 * @param av 
 * @param rules 
 * @return int	success : 0
 *				failure : 1, wrong arg, err msg displayed
 */
int	init_rules(int ac, char **av, t_rules *rules)
{
	if (ac != 5 && ac != 6)
		return (printf("Invalid number of arguments, expects :\n\
 ./philo nb_of_philo t_die t_eat t_sleep [nb_meals_per_philo]\n"), 1);
	if (ft_atoi_ptr(av[1], &rules->nb_philos) || rules->nb_philos < 1)
		return (printf("Incorrect number of philosophers\n"), 1);
	if (ft_atoi_ptr(av[2], &rules->t_die) || rules->t_die < 0)
		return (printf("Incorrect time to die\n"), 1);
	if (ft_atoi_ptr(av[3], &rules->t_eat) || rules->t_eat < 0)
		return (printf("Incorrect time to eat\n"), 1);
	if (ft_atoi_ptr(av[4], &rules->t_sleep) || rules->t_sleep < 0)
		return (printf("Incorrect time to sleep\n"), 1);

	printf("\e[103mRules :\n  nb_philos: %d\n  time_to_die: %d\n  time_to_eat: %d\n  time_to_sleep: %d\e[0m\n", rules->nb_philos, rules->t_die, rules->t_eat, rules->t_sleep);//
	if (ac == 5)
		return (rules->nb_meals = -1, 0);
	if (ft_atoi_ptr(av[5], &rules->nb_meals) || rules->nb_meals < 0)
		return (printf("Incorrect number of time each philosopher must eat\n"),
			1);
	printf("\e[103m  nb_of_meals: %d\e[0m\n", rules->nb_meals);
	return (0);
}

/**
 * @brief builds generates forks and the associated mutexes
 * 
 * @param nb_fork 
 * @param mutexes 
 * @param forks 
 * @return int	success : 0
 *				failure : 1, mutex init error, err msg displayed
 */
int	init_forks(t_fork **forks, int nb_philos)
{
	int	i;

	*forks = malloc(sizeof(t_fork) * nb_philos);
	if (!(*forks))
		return (printf("malloc error\n"), 1);
	i = -1;
	while (++i < nb_philos)
	{
		(*forks)[i].fork_free = true;
		if (pthread_mutex_init(&(*forks)[i].fork_m, NULL))
			return (printf("mutex init error\n"),
				unset_forks(forks, i), 1);
	}
	return (0);
}

void	unset_forks(t_fork **forks, int nb_fork_tounset)
{
	while (--nb_fork_tounset >= 0)
	{
		if (pthread_mutex_destroy(&(*forks)[nb_fork_tounset].fork_m))
			printf("error in thread %d destuction\n", nb_fork_tounset + 1);
	}
	free(*forks);
	*forks = NULL;
}

int	init_philos(t_philo **philos, t_rules *rules, t_fork *forks)
{
	int		i;

	*philos = malloc(sizeof(t_philo) * rules->nb_philos);
	if (!*philos)
		return (printf("malloc error\n"), 1);
	if (pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr))
	t0 = get_time_ms();
	i = -1;
	while (++i < rules->nb_philos)
	{
		(*philos)[i] = (t_philo){i + 1, *rules, forks, 0, LC_HUNGRY,
			(pthread_mutex_t){0}, 0, (pthread_t){0}};
		if (pthread_mutex_init(&(*philos)[i].life_controle_m, NULL))
			return (printf("mutex init error\n"), unset_philos(philos, i), 1);
		if (pthread_create(&(*philos)[i].thread, NULL, &_ph_routine,
			(void *)((*philos) + i)))
			return (printf("thread creation error\n"),
				pthread_mutex_destroy(&(*philos)[i].life_controle_m),
				unset_philos(philos, i), 1);
	}
	return (0);
}

void	unset_philos(t_philo **philos, int nb_philos)
{
	while (--nb_philos >= 0)
	{
		printf ("\e[101mReady to unset philo %d\e[0m\n", nb_philos);//
		pthread_mutex_lock(&(*philos)[nb_philos].life_controle_m);
		(*philos)[nb_philos].life_controle = -1;
		pthread_mutex_unlock(&(*philos)[nb_philos].life_controle_m);
		pthread_join((*philos)[nb_philos].thread, NULL);
		if (pthread_mutex_destroy(&(*philos)[nb_philos].life_controle_m))
			printf("philo %d : life mutex closing error\n", nb_philos + 1);
	}
	free(*philos);
	*philos = NULL;
}