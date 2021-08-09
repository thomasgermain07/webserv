/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base64.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/27 17:27:39 by tboisnie          #+#    #+#             */
/*   Updated: 2021/01/29 17:59:46 by tboisnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Base64.hpp"

/**
 * Static
 */
std::string
Base64::encode(
	const std::string & in_str
)
{
	// Encoding table
	static const unsigned char table[65] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
		"abcdefghijklmnopqrstuvwxyz" \
		"0123456789" \
		"+/"
	;

	// Input
	const std::string::size_type	in_len = in_str.length();
	const unsigned char * 			in_end = (unsigned char *)&in_str[0] + in_len;
	const unsigned char * 			in = (unsigned char *)in_str.c_str();

	// Output
	std::string			out_str(4 * ((in_len + 2) / 3), '\0');
	unsigned char *		out = (unsigned char *)out_str.c_str();

	// Encoding
	while (in_end - in >= 3)
	{
		*out++ = table[  in[0] >> 2];
		*out++ = table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*out++ = table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*out++ = table[  in[2] & 0x3f];

		in += 3;
	}

	if (in_end - in > 0)
	{
		*out++ = table[in[0] >> 2];

		if (in_end - in == 1)
		{
			*out++ = table[ (in[0] & 0x03) << 4];
			*out++ = '=';
		}
		else
		{
			*out++ = table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
			*out++ = table[ (in[1] & 0x0f) << 2];
		}

		*out++ = '=';
	}

	return out_str;
}

std::string
Base64::decode(
	const std::string & in_str
)
{
	// Decoding table
	static const int table[256] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,
		 0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
		 7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
		19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0, 63,
		 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
		37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
		49, 50, 51
	};

	// Input
	const std::string::size_type	in_len = in_str.length();
	const unsigned char *			in = (unsigned char *)&in_str[0];

	const bool has_pad = (
		in_len > 0 && ( in_len % 4 || in[in_len - 1] == '=')
	);

	// Output
	const std::string::size_type	out_len = ((in_len + 3) / 4 - has_pad) * 4;
	std::string 					out_str(out_len / 4 * 3 + has_pad, '\0');

	// Decoding
	for (size_t i = 0, j = 0; i < out_len; i += 4)
	{
		int n = table[in[i + 0]] << 18 |
				table[in[i + 1]] << 12 |
				table[in[i + 2]] <<  6 |
				table[in[i + 3]]
		;

		out_str[j++] = n >> 16;
		out_str[j++] = n >> 8 & 0xFF;
		out_str[j++] = n      & 0xFF;
	}

	if (has_pad)
	{
		int n = table[in[out_len + 0]] << 18 |
				table[in[out_len + 1]] << 12
		;

		out_str[out_str.size() - 1] = n >> 16;

		if (in_len > out_len + 2 && in[out_len + 2] != '=')
		{
			n |= table[in[out_len + 2]] << 6;
			out_str += (n >> 8 & 0xFF);
		}
	}

	return out_str;
}
