/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reverse_bits.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <franciscogama99@outlook.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:58:12 by rochimuc          #+#    #+#             */
/*   Updated: 2026/02/20 13:44:30 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

unsigned char ft_reverse_bits(unsigned char octet) {
  unsigned char res = 0;
  int i = 8;

  while (i > 0) {
    res = (res << 1) | (octet & 1);
    octet = octet >> 1;
    i--;
  }
  return (res);
}
