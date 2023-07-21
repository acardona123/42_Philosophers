/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:40:53 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 18:42:23 by acardona         ###   ########.fr       */
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
	if (ft_atozu_ptr(av[2], &rules->t_die))
		return (printf("Invalid time to die\n"), 1);
	if (ft_atozu_ptr(av[3], &rules->t_eat))
		return (printf("Invalid time to eat\n"), 1);
	if (ft_atozu_ptr(av[4], &rules->t_sleep))
		return (printf("Invalid time to sleep\n"), 1);
	if (rules->nb_philos % 2 == 0)
		rules->t_cycle = 2 * rules->t_eat;
	else
		rules->t_cycle = 3 * rules->t_eat;
	rules->t_cycle = ft_max_zu(rules->t_eat + rules->t_sleep, rules->t_cycle);
	if (ac == 5)
		rules->nb_meals = -1;
	else if (ft_atoi_ptr(av[5], &rules->nb_meals) || rules->nb_meals < 0)
		return (printf("Invalid number of time each philosopher must eat\n")
			, 1);
	// printf("RULES :\n nb_philo : %d\n t_die = %zu\n t_eat = %zu\n t_sleep = %zu\n t_cycle = %zu\n nb_meals = %d\n\n", rules->nb_philos, rules->t_die, rules->t_eat, rules->t_sleep, rules->t_cycle, rules->nb_meals);//
	return (0);
}

static int	_init_shared(t_shared *shared, t_rules *rules)
{
	if (pthread_mutex_init(&shared->start_m, 0))
		return (printf("shared mutex error\n"), 1);
	if (pthread_mutex_init(&shared->all_alive_m, 0))
		return (printf("shared mutex error\n"),
			pthread_mutex_destroy(&shared->start_m));
	shared->all_alive = true;
	if (_init_forks(shared, rules))
		return (pthread_mutex_destroy(&shared->all_alive_m),
			pthread_mutex_destroy(&shared->start_m), 1);
	shared->philos = malloc(sizeof(t_philo) * rules->nb_philos);
	if (!shared->philos)
	{
		printf("malloc error\n");
		pthread_mutex_destroy(&shared->start_m);
		pthread_mutex_destroy(&shared->all_alive_m);
		unset_forks(shared, rules->nb_philos);
		return (1);
	}
	return (0);
}

static int	_init_forks(t_shared *shared, t_rules *rules)
{
	int	i;

	shared->fork_m = malloc(sizeof(pthread_mutex_t) * rules->nb_philos);
	if (!shared->fork_m)
		return (printf("malloc error\n"), 1);
	i = 0;
	while (i < rules->nb_philos)
	{
		if (pthread_mutex_init(&shared->fork_m[i], NULL))
			return (printf("forks mutex error\n"), unset_forks(shared, i),
				free(shared->fork_m), 1);
		i++;
	}
	return (0);
}

static int	_init_and_start_philos(t_shared *shared, t_rules *rules)
{
	int		i;

	shared->start_t = 0;
	i = -1;
	while (++i < rules->nb_philos)
	{
		((t_philo *)shared->philos)[i] = (t_philo){i + 1, i % 2 + ((i + 1) % 2
				&& rules->nb_philos > 1 && i + 1 == rules->nb_philos) * 2,
			shared, *rules, 0,
			(i + (i == rules->nb_philos)) % rules->nb_philos,
			(i + (i != rules->nb_philos)) % rules->nb_philos,
			0, 0, (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER, 0, 0};
		// printf("philo %d : l = %d (%p)  && r = %d (%p)\n", ((t_philo *)shared->philos)[i].id, ((t_philo *)shared->philos)[i].fork_id_l, &((t_philo *)shared->philos)[i].fork_id_l, ((t_philo *)shared->philos)[i].fork_id_r, &((t_philo *)shared->philos)[i].fork_id_r);//
	}
	i = -1;
	while (++i < rules->nb_philos)
		if (pthread_create(&((t_philo *)(shared->philos))[i].thread, 0,
				&thread_routine, (void *)&((t_philo *)(shared->philos))[i]))
			return (print_msg(&((t_philo *)(shared->philos))[i], 0, MSG_ERROR),
				stop_and_unset_philos(shared, i, rules->nb_philos, true), 1);
	pthread_mutex_lock(&shared->start_m);
	shared->start_t = get_time_ms();
	pthread_mutex_unlock(&shared->start_m);
	usleep(30000);
	return (0);
}
