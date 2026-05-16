/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_bits.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <franciscogama99@outlook.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 14:28:59 by rochimuc          #+#    #+#             */
/*   Updated: 2026/02/23 14:30:16 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

unsigned char ft_swap_bits(unsigned char octet) {
  return ((octet >> 4 | octet << 4));
}
