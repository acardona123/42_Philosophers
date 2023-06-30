/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexcardona <alexcardona@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 16:54:25 by alexcardona       #+#    #+#             */
/*   Updated: 2023/06/30 04:09:37 by alexcardona      ###   ########.fr       */
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

typedef enum e_rules
{
	nb_philos,
	time_to_die,
	time_to_eat,
	time_to_sleep,
	nb_of_meals
}	t_rules;

typedef struct s_philo_data
{
	int				*rules;
	char			*forks;
	pthread_mutex_t	*mutexes;
	int				*id;
}	t_philo_data;

// tools.c
size_t	ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_atoi_ptr(const char *nptr, int *dst);

#endif
