/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 02:28:45 by acardona          #+#    #+#             */
/*   Updated: 2023/07/07 03:38:32 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	_sub_thread_routine_loop(t_philo *philo);
static void	_update_status(t_philo *philo, size_t t_now, size_t t_last_eat,
				char *have_forks);
static bool	_eat(t_philo *philo, size_t t_now, size_t *t_last_eat,
				char *have_forks);
static bool	take_a_fork(t_shared *shared, int id_fork);
static bool	give_a_fork(t_shared *shared, int id_fork);

void	*thread_routine_start(void *philo_)
{
	t_philo	*philo;

	philo = (t_philo *)philo_;
	pthread_mutex_lock(&philo->shared->start_m);
	philo->t0 = philo->shared->start_t;
	pthread_mutex_unlock(&philo->shared->start_m);
	if (philo->t0 < 0)
		return (NULL);
	philo->t_last_eat = philo->t0;
	if (philo->rules.nb_philos % 2 == 0)
		usleep(((philo->id + 1) % 2) * philo->rules.t_eat * 1000);
	else if (philo->rules.nb_philos != 1 && philo->id != philo->rules.nb_philos)
		usleep ((1 + (philo->id % 2)) * philo->rules.t_eat * 1000);// + 1000 * (philo->id + 1) % 2);
	_sub_thread_routine_loop(philo);
	return (NULL);
}

static void	_sub_thread_routine_loop(t_philo *philo)
{
	size_t	t_now;
	char	*have_fork;
	size_t	delay;

	have_fork = (char [2]){0, 0};
	t_now = get_time_ms();
	_eat(philo, t_now, &philo->t_last_eat, have_fork);
	while (philo->status != S_DEAD)
	{
		t_now = get_time_ms();
		delay = t_now - philo->t_last_eat;
		_update_status(philo, t_now, delay, have_fork);
		if (philo->status == S_THINK && delay >= philo->rules.t_cycle)
			_eat(philo, t_now, &philo->t_last_eat, have_fork);
		pthread_mutex_lock(&philo->life_ctrl_m);
		if (philo->life_ctrl == LC_DEAD)
			philo->status = S_DEAD;
		else if (philo->status == S_DEAD)
			philo->life_ctrl = LC_DEAD;
		else if (philo->rules.nb_meals >= 0
			&& philo->cpt_eat >= philo->rules.nb_meals)
			philo->life_ctrl = LC_FED;
		pthread_mutex_unlock(&philo->life_ctrl_m);
	}
}

static void	_update_status(t_philo *philo, size_t t_now, size_t delay,
	char *have_forks)
{
	if (philo->status == S_EAT && delay >= philo->rules.t_eat)
	{
				// print_msg_fork(philo, t_now, MSG_DEBUG, -2);//
		give_a_fork(philo->shared, philo->id % philo->rules.nb_philos);
							// printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s %d\e[0m\n", t_now - philo->t0, philo->id, philo->id, MSG_DEBUG, "released fork ", philo->id % philo->rules.nb_philos + 1);
				// print_msg_fork(philo, t_now, MSG_DEBUG, philo->id % philo->rules.nb_philos);//
		give_a_fork(philo->shared, philo->id - 1);
							// printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s %d\e[0m\n", t_now - philo->t0, philo->id, philo->id, MSG_DEBUG, "released fork ", philo->id);
				// print_msg_fork(philo, t_now, MSG_DEBUG, philo->id - 1);//
		print_msg(philo, t_now, MSG_SLEEP);
		philo->status = S_SLEEP;
		philo->cpt_eat++;
		have_forks[0] = 0;
		have_forks[1] = 0;
	}
	if (philo->status == S_SLEEP && delay >= philo->rules.t_eat
		+ philo->rules.t_sleep)
	{
		philo->status = S_THINK;
		print_msg(philo, t_now, MSG_THINK);
	}
	if (philo->status == S_THINK && delay >= philo->rules.t_die)
	{
		philo->status = S_DEAD;
		print_msg(philo, t_now, MSG_DIED);
	}
}

static bool	_eat(t_philo *philo, size_t t_now, size_t *t_last_eat,
	char *have_forks)
{
	int	i;

	i = -1;
	usleep (100);
	while (++i < 5)
	{
		usleep(200);
		if (!have_forks[1] && take_a_fork(philo->shared,philo->id - 1)) //ajouter comme premiere condition have_fork[0] ?
		{
			// print_msg_fork(philo, t_now, MSG_FORK, philo->id - 1);
			print_msg(philo, t_now, MSG_FORK);
			have_forks[1] = 1;
		}
		if (!have_forks[0] && take_a_fork(philo->shared,
				philo->id % philo->rules.nb_philos))
		{
			// print_msg_fork(philo, t_now, MSG_FORK, philo->id % philo->rules.nb_philos);
			print_msg(philo, t_now, MSG_FORK);
			have_forks[0] = 1;
		}
		if (have_forks[0] && have_forks[1])
		{
			philo->status = S_EAT;
			print_msg(philo, t_now, MSG_EAT);
			*t_last_eat = t_now;
			return (true);
		}
	}
	return (false);
}

static bool	take_a_fork(t_shared *shared, int id_fork)
{
	pthread_mutex_lock(shared->fork_m + id_fork);
	if (shared->fork_ok[id_fork])
	{
		shared->fork_ok[id_fork] = false;
		pthread_mutex_unlock(shared->fork_m + id_fork);
		return (true);
	}
	pthread_mutex_unlock(shared->fork_m + id_fork);
	// printf("------------------  try fork %d  -----------------------\n", id_fork + 1);//
	return (false);
}

static bool	give_a_fork(t_shared *shared, int id_fork)
{
	pthread_mutex_lock(shared->fork_m + id_fork);
	shared->fork_ok[id_fork] = true;
	pthread_mutex_unlock(shared->fork_m + id_fork);
	return (false);
}
