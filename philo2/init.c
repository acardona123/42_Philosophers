/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:40:53 by acardona          #+#    #+#             */
/*   Updated: 2023/07/24 16:54:01 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	_init_rules(int ac, char **av, t_rules *rules);
static int	_init_shared(t_shared *shared, t_rules *rules);
static int	_init_forks(t_shared *shared, t_rules *rules);
static int	_init_and_start_philos(t_shared *shared, t_rules *rules);

int	init_and_start(int ac, char **av, t_rules *rules, t_shared *shared)
{
	if (_init_rules(ac, av, rules))
		return (1);
	if (_init_shared(shared, rules))
		return (1);
	if (_init_and_start_philos(shared, rules))
		return (unset_shared(shared, rules), 1);
	return (0);
}

/**
 * @brief Initialisation of the set of rules
 * 
 * @param ac
 * @param av 
 * @param rules 
 * @return int	success : 0
 *				failure : 1, wrong arg, err msg displayed
 */
static int	_init_rules(int ac, char **av, t_rules *rules)
{
	if (ac != 5 && ac != 6)
		return (printf("Invalid number of arguments, expects :\n\
 ./philo nb_of_philo t_die t_eat t_sleep [nb_meals_per_philo]\n"), 1);
	if (ft_atoi_ptr(av[1], &rules->nb_philos) || rules->nb_philos < 1)
		return (printf("Invalid number of philosophers\n"), 1);
	if (ft_atozu_ptr(av[2], &rules->t_die) || rules->t_die < T_MIN)
		return (printf("Invalid time to die\n"), 1);
	if (ft_atozu_ptr(av[3], &rules->t_eat) || rules->t_eat < T_MIN)
		return (printf("Invalid time to eat\n"), 1);
	if (ft_atozu_ptr(av[4], &rules->t_sleep) || rules->t_sleep < T_MIN)
		return (printf("Invalid time to sleep\n"), 1);
	if (rules->nb_philos % 2 == 0)
		rules->t_cycle = 2 * rules->t_eat;
	else
		rules->t_cycle = 3 * rules->t_eat;
	rules->t_cycle = ft_max_zu(rules->t_eat + rules->t_sleep, rules->t_cycle);
	if (ac == 5)
		rules->nb_meals = -1;
	else if (ft_atoi_ptr(av[5], &rules->nb_meals) || rules->nb_meals <= 0)
		return (printf("Invalid number of time each philosopher must eat\n")
			, 1);
	return (0);
}

static int	_init_shared(t_shared *shared, t_rules *rules)
{
	if (pthread_mutex_init(&shared->start_m, 0))
		return (printf("shared mutex error\n"), 1);
	if (pthread_mutex_init(&shared->disp_m, 0))
		return (printf("shared mutex error\n"),
			pthread_mutex_destroy(&shared->start_m));
	shared->disp_ok = true;
	if (_init_forks(shared, rules))
		return (pthread_mutex_destroy(&shared->disp_m), 1);
	shared->philos = malloc(sizeof(t_philo) * rules->nb_philos);
	if (!shared->philos)
	{
		pthread_mutex_destroy(&shared->start_m);
		pthread_mutex_destroy(&shared->disp_m);
		unset_forks(shared, rules->nb_philos);
		printf("malloc error\n");
		return (1);
	}
	return (0);
}

static int	_init_forks(t_shared *shared, t_rules *rules)
{
	int	i;

	shared->fork_ok = malloc (sizeof(bool) * rules->nb_philos);
	if (!shared->fork_ok)
		return (printf("malloc error\n"), 1);
	shared->fork_m = malloc(sizeof(pthread_mutex_t) * rules->nb_philos);
	if (!shared->fork_m)
		return (printf("malloc error\n"), free(shared->fork_ok), 1);
	i = 0;
	while (i < rules->nb_philos)
	{
		shared->fork_ok[i] = true;
		if (pthread_mutex_init(&shared->fork_m[i], NULL))
			return (printf("forks mutex error\n"), unset_forks(shared, i),
				free(shared->fork_ok), free(shared->fork_m), 1);
		i++;
	}
	return (0);
}

static int	_init_and_start_philos(t_shared *shared, t_rules *rules)
{
	int		i;
	t_philo	*in_philo;

	shared->start_t = -1;
	pthread_mutex_lock(&shared->start_m);
	i = 0;
	while (i < rules->nb_philos)
	{
		in_philo = &((t_philo *)shared->philos)[i];
		*in_philo = (t_philo){i + 1, shared, *rules, 0, S_THINK, 0, 0, 0,
			LC_HUNGRY, (pthread_mutex_t){0}};
		if (pthread_mutex_init(&in_philo->life_ctrl_m, 0))
			return (printf("philo mutex error\n"),
				stop_and_unset_philos(shared, i, true), 1);
		if (pthread_create(&in_philo->thread, 0, &thread_routine_start,
				(void *)in_philo))
			return (printf("forks thread error\n"),
				pthread_mutex_destroy(&in_philo->life_ctrl_m),
				stop_and_unset_philos(shared, i, true), 1);
		i++;
	}
	usleep(200000);
	shared->start_t = get_time_ms();
	pthread_mutex_unlock(&shared->start_m);
	return (0);
}
