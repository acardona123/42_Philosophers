/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:24:51 by acardona          #+#    #+#             */
/*   Updated: 2023/07/24 15:52:49 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <stdbool.h>
# include <pthread.h>
# include <sys/time.h>

# define T_MIN 60
# define COLOR 0

typedef enum e_life_controle
{
	LC_HUNGRY,
	LC_FED,
	LC_DEAD
}	t_life_controle;

typedef struct s_rules
{
	int		nb_philos;
	size_t	t_die;
	size_t	t_eat;
	size_t	t_sleep;
	size_t	t_cycle;
	int		nb_meals;
}	t_rules;

typedef enum e_status
{
	S_EAT,
	S_SLEEP,
	S_THINK,
	S_DEAD
}	t_status;

typedef struct s_shared
{
	size_t			start_t;
	pthread_mutex_t	start_m;
	bool			disp_ok;
	pthread_mutex_t	disp_m;
	bool			*fork_ok;
	pthread_mutex_t	*fork_m;
	void			*philos;
}	t_shared;

typedef struct s_philo
{
	int				id;
	t_shared		*shared;
	t_rules			rules;
	pthread_t		thread;
	t_status		status;
	size_t			t0;
	size_t			t_last_eat;
	int				cpt_eat;
	t_life_controle	life_ctrl;
	pthread_mutex_t	life_ctrl_m;
}	t_philo;

typedef struct s_timer
{
	struct timeval	tv;
	struct timezone	tz;
}	t_timer;

typedef enum e_msg_type
{
	MSG_FORK,	
	MSG_DIED,
	MSG_EAT,
	MSG_SLEEP,
	MSG_THINK,
	MSG_DEBUG,
}	t_msg_type;

// tools.c
size_t	ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_atoi_ptr(const char *nptr, int *dst);
int		ft_atozu_ptr(const char *nptr, size_t *dst);
size_t	ft_max_zu(size_t n1, size_t n2);

// init.c
int		init_and_start(int ac, char **av, t_rules *rules, t_shared *shared);

//unset.c
void	unset_forks(t_shared *shared, int nb_of_forks);
void	stop_and_unset_philos(t_shared *shared, int nb_of_philo, bool init);
void	unset_shared(t_shared *shared, t_rules *rules);

//routine.c
void	*thread_routine_start(void *philo);

//routine1.c
bool	eat(t_philo *philo, size_t t_now, size_t *t_last_eat,
			char *have_forks);
bool	take_a_fork(t_shared *shared, int id_fork);
bool	give_a_fork(t_shared *shared, int id_fork);
size_t	get_time_ms(void);
void	print_msg(t_philo *philo, size_t t_now, t_msg_type msg_type);

#endif