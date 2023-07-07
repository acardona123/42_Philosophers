/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 02:28:45 by acardona          #+#    #+#             */
/*   Updated: 2023/07/06 01:37:09 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	_sub_thread_routine_loop(t_philo *philo);
static void	_update_status(t_philo *philo, ssize_t t_now, ssize_t t_last_eat);
static void	_print_msg(t_philo *philo, ssize_t t_now, t_msg_type msg_type);
static bool	_try_eating(t_philo	*philo, ssize_t t_now, ssize_t *t_last_eat,
				int *first_hand);
static bool	take_a_fork(t_shared *shared, int id_fork);
static bool	give_a_fork(t_shared *shared, int id_fork);

void	*thread_routine_start(void *philo)
{
	printf("\e[101mphilo %d lunched\e[0m\n", ((t_philo *)philo)->id);
	pthread_mutex_lock(&((t_philo *)philo)->shared->start_m);
	((t_philo *)philo)->t0 = ((t_philo *)philo)->shared->start_t;
	pthread_mutex_unlock(&((t_philo *)philo)->shared->start_m);
	if (((t_philo *)philo)->t0 < 0)
		return (NULL);
	// printf("\e[33m\tphilo %d start looping with t0 = %zu\e[0m\n", ((t_philo *)philo)->id, ((t_philo *)philo)->t0);
	_sub_thread_routine_loop((t_philo *)philo);
	return (NULL);
}

static void	_sub_thread_routine_loop(t_philo *philo)
{
	ssize_t	t_last_eat;
	ssize_t	t_now;
	int		first_hand;

	t_last_eat = philo->t0;
	first_hand = philo->id % 2;
	while (philo->status != S_DEAD)
	{
		t_now = get_time_ms();
		_update_status(philo, t_now, t_last_eat);
		if (philo->status == S_THINK)
			_try_eating(philo, t_now, &t_last_eat, &first_hand);
		pthread_mutex_lock(&philo->life_ctrl_m);
		if (philo->life_ctrl == LC_DEAD)
			philo->status = S_DEAD;
		else if (philo->status == S_DEAD)
			philo->life_ctrl = LC_DEAD;
		else if (philo->rules.nb_meals >= 0
			&& philo->cpt_eat >= philo->rules.nb_meals)
			philo->life_ctrl = LC_FED;
		pthread_mutex_unlock(&philo->life_ctrl_m);
		// usleep(15);//
	}
}

static void	_update_status(t_philo *philo, ssize_t t_now, ssize_t t_last_eat)
{
	if (philo->status == S_EAT && t_now - t_last_eat > philo->rules.t_eat)
	{
		philo->status = S_SLEEP;
		philo->cpt_eat++;
		_print_msg(philo, t_now, MSG_SLEEP);
		give_a_fork(philo->shared, philo->id % philo->rules.nb_philos);
		give_a_fork(philo->shared, philo->id - 1);
		_print_msg(philo, t_now, MSG_DEBUG);//
		// usleep(100);
	}
	if (philo->status == S_SLEEP && t_now - t_last_eat > philo->rules.t_eat
		+ philo->rules.t_sleep)
	{
		philo->status = S_THINK;
		_print_msg(philo, t_now, MSG_THINK);
	}
	if (philo->status == S_THINK && t_now - t_last_eat > philo->rules.t_die)
	{
		philo->status = S_DEAD;
		_print_msg(philo, t_now, MSG_DIED);
	}
}
/*//no colors
static void	_print_msg(t_philo *philo, ssize_t t_now, t_msg_type msg_type)
{
	char	*msg;

	if (msg_type == MSG_DIED)
		msg = "died";
	else if (msg_type == MSG_EAT)
		msg = "is eating";
	else if (msg_type == MSG_SLEEP)
		msg = "is sleeping";
	else if (msg_type == MSG_THINK)
		msg = "is thinking";
	else
		msg = "has taken a fork";
	pthread_mutex_lock(&philo->shared->disp_m);
	if (philo->shared->disp_ok)
		printf("%zu %d %s\n", t_now - philo->t0, philo->id, msg);
	if (msg_type == MSG_DIED)
		philo->shared->disp_ok = false;
	pthread_mutex_unlock(&philo->shared->disp_m);
}
*/
static void	_print_msg(t_philo *philo, ssize_t t_now, t_msg_type msg_type)
{
	char	*msg;

	if (msg_type == MSG_DEBUG)
		msg = "release forks";
	else if (msg_type == MSG_DIED)
		msg = "died";
	else if (msg_type == MSG_EAT)
		msg = "is eating";
	else if (msg_type == MSG_SLEEP)
		msg = "is sleeping";
	else if (msg_type == MSG_THINK)
		msg = "is thinking";
	else
		msg = "has taken a fork";
	pthread_mutex_lock(&philo->shared->disp_m);
	if (philo->shared->disp_ok)
		printf("%zu\e[1;10%dm %d \e[0m\e[3%dm%s\e[0m\n", t_now - philo->t0,
			philo->id, philo->id, msg_type, msg);
	if (msg_type == MSG_DIED)
		philo->shared->disp_ok = false;
	pthread_mutex_unlock(&philo->shared->disp_m);
}

static bool	_try_eating(t_philo	*philo, ssize_t t_now, ssize_t *t_last_eat,
	int *first_hand)
{
	int	i;

	if (take_a_fork(philo->shared, (philo->id - *first_hand)
			% philo->rules.nb_philos))
	{
		_print_msg(philo, t_now, MSG_FORK);
		// printf("\t\t#%d\n", (philo->id - *first_hand) % philo->rules.nb_philos);//
		*first_hand = (*first_hand + 1) % 2;
		i = 4;
		while (--i)
		{
			if (take_a_fork(philo->shared, (philo->id - *first_hand)
					% philo->rules.nb_philos))
			{
				_print_msg(philo, t_now, MSG_FORK);
				_print_msg(philo, t_now, MSG_EAT);
				*t_last_eat = t_now;
				philo->status = S_EAT;
				return (true);
			}
		}
		give_a_fork(philo->shared, (philo->id - (*first_hand + 1 ) % 2)
			% philo->rules.nb_philos);
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
	return (false);
}

static bool	give_a_fork(t_shared *shared, int id_fork)
{
	pthread_mutex_lock(shared->fork_m + id_fork);
	shared->fork_ok[id_fork] = true;
	pthread_mutex_unlock(shared->fork_m + id_fork);
	return (false);
}
