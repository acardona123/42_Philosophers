/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 23:24:51 by acardona          #+#    #+#             */
/*   Updated: 2023/07/03 06:17:53 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <pthread.h>
# include <string.h>
# include <limits.h>
# include <stdbool.h>

typedef enum e_life_control
{
	LC_HUNGRY,
	LC_FED,
	LC_DEAD
}	t_life_controle;

typedef struct s_rules
{
	int	nb_philos;
	int	t_die;
	int	t_eat;
	int	t_sleep;
	int	nb_meals;
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
	ssize_t			start_t;
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
	int				cpt_eat;
	ssize_t			t0;
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
	MSG_THINK
}	t_msg_type;

// tools.c
size_t	ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_atoi_ptr(const char *nptr, int *dst);
ssize_t	get_time_ms(void);

//	init.c
int		init_and_start(int ac, char **av, t_rules *rules, t_shared *shared);

//unset.c
void	unset_forks(t_shared *shared, int nb_of_forks);
void	stop_and_unset_philos(t_shared *shared, int nb_of_philo, bool init);
void	unset_shared(t_shared *shared, t_rules *rules);

//routine.c
void	*thread_routine_start(void *philo);

#endif