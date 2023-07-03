/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 16:55:16 by alexcardona       #+#    #+#             */
/*   Updated: 2023/07/03 05:04:53 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


int	main(int ac, char **av)
{
	t_rules	rules;
	t_fork	*forks;
	t_philo	*philos;
	int		controle;
	int		i;

	if (init_rules(ac, av, &rules) || init_forks(&forks, rules.nb_philos))
		return (1);
	printf("Rules and forks initialized\n");//
	philos = NULL;
	if (init_philos(&philos, &rules, forks))
		return (unset_forks(&forks, rules.nb_philos), 1);
	printf ("\n All philos lunched\n\n");//
	controle = 0;
	while (controle >= 0 && controle != rules.nb_meals)
	{
		controle = 0;
		i = rules.nb_philos;
		while (controle >= 0 && --i >= 0)
		{
			pthread_mutex_lock(&philos[i].life_ctrl_m);
			if (philos[i].life_ctrl == LC_DEAD)
				controle = -1;
			else if (philos[i].life_ctrl == LC_FED)
				controle++;
			pthread_mutex_unlock(&philos[i].life_ctrl_m);
		}
	}
	printf("\e[103mONE OF THEM DIED\e[0m\n");//
	i = rules.nb_philos;
	while (--i >= 0)
	{
		pthread_mutex_lock(&philos[i].life_ctrl_m);
		philos[i].life_ctrl = LC_DEAD;
		pthread_mutex_unlock(&philos[i].life_ctrl_m);
	}
	unset_philos(&philos, rules.nb_philos);
	unset_forks(&forks, rules.nb_philos);
	return (0);
}


ssize_t	get_time_ms(void)
{
	t_timer	now;

	gettimeofday(&now.tv, &now.tz);
	return (1000 * now.tv.tv_sec + now.tv.tv_usec / 1000);
}

void	get_status(t_philo *philo, ssize_t t_last_meal, ssize_t t_now,
	t_status *status)
{
	ssize_t	delay;

	delay = t_now - t_last_meal;
	if (delay < philo->rules->t_eat)
		return ;
	if (*status == S_EAT && delay >= philo->rules->t_eat)
	{
		release_forks(philo);
		printf("\e[1;3%dm%zu %d is sleeping\e[0m\n",philo->id, t_now - philo->t0, philo->id);
		*status = s_SLEEP;
		philo->cpt_meal++;
	}
	if (*status == s_SLEEP
		&& delay >= philo->rules->t_eat + philo->rules->t_sleep)
		(printf("\e[1;3%dm%zu %d is thinking\e[0m\n",philo->id, t_now - philo->t0, philo->id),
			*status = S_THINK);
	if (*status == S_THINK && delay >= philo->rules->t_die)
		(printf("\e[1;3%dm%zu %d died\e[0m\n",philo->id, t_now - philo->t0, philo->id),
			*status = S_DEAD);
}

bool	get_forks(t_philo *philo, t_hand hand, ssize_t t_now)
{
	bool	take_fork;
	t_fork	*target_fork;

	// printf("philo %d entering to take fork %d\n", philo->id, philo->id - (hand == H_LEFT));//
	target_fork = &philo->forks[(philo->id - (hand == H_LEFT))
		% philo->rules->nb_philos];
	pthread_mutex_lock(&target_fork->fork_m);
	if (target_fork->fork_free == true)
	{
		take_fork = true;
		target_fork->fork_free = false;
	}
	else
		take_fork = false;
	pthread_mutex_unlock(&target_fork->fork_m);
	if (take_fork)
		// printf("%zu %d has taken a fork\n", t_now - philo->t0, philo->id);
		printf("\t%zu %d HAS TAKEN THE FORK %d\n", t_now - philo->t0, philo->id, (philo->id - (hand == H_LEFT)) % philo->rules->nb_philos);//
	return (take_fork);
}

void	release_forks(t_philo *philo)
{
	t_fork	*one_fork;

	one_fork = &philo->forks[philo->id - 1];
	pthread_mutex_lock(&one_fork->fork_m);
	one_fork->fork_free = true;
	pthread_mutex_unlock(&one_fork->fork_m);
	one_fork = &philo->forks[philo->id % philo->rules->nb_philos];
	pthread_mutex_lock(&one_fork->fork_m);
	one_fork->fork_free = true;
	pthread_mutex_unlock(&one_fork->fork_m);
}

void	*_ph_routine(void *philo)
{
	return (_ph_routine_sub((t_philo *) philo));
}

void	*_ph_routine_sub(t_philo *philo)
{
	ssize_t		t_now;
	ssize_t		t_last_eat;
	t_status	status;

	printf("\e[101m\nI'm philo %d and i'm awake\e[0m\n\n", philo->id);
	status = S_THINK;
	t_last_eat = philo->t0;
	while (status != S_DEAD)
	{
		usleep(5);
		t_now = get_time_ms();
		get_status(philo, t_last_eat, t_now, &status);
		if (status == S_THINK)
		{
			if (get_forks(philo, philo->id % 2, t_now))
			{
				if (get_forks(philo, (philo->id + 1) % 2, t_now))
				{
					printf("\e[1;3%dm%zu %d is eating\e[0m\n",philo->id, t_now - philo->t0, philo->id);
					t_last_eat = t_now;
					status = S_EAT;
				}
				else
					release_forks(philo);
			}
		}
		pthread_mutex_lock(&philo->life_ctrl_m);
		if (philo->life_ctrl == LC_DEAD)
			break ;
		else if (status == S_DEAD)
			philo->life_ctrl = LC_DEAD;
		else if (philo->rules->nb_meals >= 0 && philo->life_ctrl != LC_FED
			&& philo->cpt_meal >= philo->rules->nb_meals)
			philo->life_ctrl = LC_FED;
		pthread_mutex_unlock(&philo->life_ctrl_m);
	}
	printf("\e[101mEnd of %d\e[0m\n", philo->id);
	return (NULL);
}
