/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acardona <acardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 15:09:25 by acardona          #+#    #+#             */
/*   Updated: 2023/07/21 12:52:25 by acardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	get_time_ms(void)
{
	t_timer	now;

	gettimeofday(&now.tv, &now.tz);
	return (1000 * now.tv.tv_sec + now.tv.tv_usec / 1000);
}

size_t	get_time_us(void)
{
	t_timer	now;

	gettimeofday(&now.tv, &now.tz);
	return (1000000 * now.tv.tv_sec + now.tv.tv_usec);
}

void	my_usleep(int delay)
{
	size_t	t0;
	size_t	t_now;

	if (delay < 0)
		return ;
	t0 = get_time_us();
	t_now = t0;
	while (t_now < delay + t0)
	{
		usleep(USLEEP_RESOLUTION_US);
		t_now = get_time_us();
	}
}
