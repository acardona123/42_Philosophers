/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexcardona <alexcardona@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 16:55:16 by alexcardona       #+#    #+#             */
/*   Updated: 2023/06/30 04:19:29 by alexcardona      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	_init_rules(int ac, char **av, int rules[5]);
static int	_init_forks_and_mutexes(int nb_fork, pthread_mutex_t **mutexes,
				char **forks);
static void	_unset_forks_and_mutexes(int nb_fork, pthread_mutex_t *mutexes,
				char *forks);
static int	_threads_init(	int rules[5], char *forks, pthread_mutex_t *mutexes,
				pthread_t **philos);
static void	*_philo_routine(void *arg);

int	main(int ac, char **av)
{
	int				rules[5];
	char			*forks;
	pthread_mutex_t	*mutexes;
	pthread_t		*philos;

	mutexes = NULL;
	forks = NULL;
	philos = NULL;
	if (_init_rules(ac, av, rules)
		|| _init_forks_and_mutexes(rules[nb_philos], &mutexes, &forks))
		return (1);
	return (0);
}

/**
 * @brief Initialisation of the set of rules
 * 
 * @param av 
 * @param rules 
 * @return int	success : 0
 *				failure : 1, wrong arg, err msg displayed
 */
static int	_init_rules(int ac, char **av, int rules[5])
{
	if (ac != 5 && ac != 6)
		return (printf("Invalid number of arguments\n"), 1);
	if (ft_atoi_ptr(av[1], rules + nb_philos) || rules[nb_philos] < 1)
		return (printf("Incorrect number of philosophers\n"), 1);
	if (ft_atoi_ptr(av[2], rules + time_to_die) || rules[time_to_die] < 0)
		return (printf("Incorrect time to die\n"), 1);
	if (ft_atoi_ptr(av[3], rules + time_to_eat) || rules[time_to_eat] < 0)
		return (printf("Incorrect time to eat\n"), 1);
	if (ft_atoi_ptr(av[4], rules + time_to_sleep) || rules[time_to_sleep] < 0)
		return (printf("Incorrect time to sleep\n"), 1);
	if (!av[5])
		return (rules[4] = 0, 0);
	if (ft_atoi_ptr(av[5], rules + nb_of_meals) || rules[nb_of_meals] < 0)
		return (printf("Incorrect number of time each philosopher must eat\n"),
			1);
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
static int	_init_forks_and_mutexes(int nb_fork, pthread_mutex_t **mutexes,
	char **forks)
{
	int	i;

	*forks = malloc(sizeof(char) * nb_fork);
	if (!forks)
		return (printf("malloc error\n"), 1);
	*mutexes = malloc(sizeof(pthread_mutex_t *) * nb_fork);
	if (!mutexes)
		return (free(forks), printf("malloc error\n"), 1);
	i = -1;
	while (++i < nb_fork)
	{
		(*forks)[i] = 1;
		if (pthread_mutex_init(*mutexes + i, NULL))
		{
			while (--i >= 0)
				pthread_mutex_destroy(*mutexes + i);
			return (free(*mutexes), free(*forks), printf("mutex init error\n"),
				1);
		}
	}
	return (0);
}

static void	_unset_forks_and_mutexes(int nb_fork, pthread_mutex_t *mutexes,
	char *forks)
{
	free(forks);
	while (--nb_fork >= 0)
	{
		if (pthread_mutex_destroy(mutexes + nb_fork))
			printf("error in thread %d destuction\n", nb_fork + 1);
	}
	free(mutexes);
}

static int	_threads_init(	int rules[5], char *forks, pthread_mutex_t *mutexes,
	pthread_t **philos)
{
	int				i;
	t_philo_data	philo_data;

	*philos = malloc(sizeof(pthread_t *) * rules[nb_philos]);
	if (!*philos)
		return (printf("malloc error\n"), 1);
	philo_data = (t_philo_data){rules, forks, mutexes, NULL};
	i = -1;
	while (++i < rules[nb_philos])
	{
		philo_data.id = malloc (sizeof(int));
		if (!philo_data.id)
		{
			while (--i >= 0)
				;//destrucion de (*philos)[i]
			return (free(*philos), 1);
		}
		*(philo_data.id) = i + 1;
		if (pthread_create((*philos) + i, NULL, &_philo_routine,
				(void *)&philo_data))
		{
			while (--i >= 0)
				;//destrucion de (*philos)[i]
			return (free(*philos), free(philo_data.id), 1);
		}
	}
	return (0);
}

static void	*_philo_routine(void *arg)
{
	
}