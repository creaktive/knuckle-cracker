/****************************************************************************
	This file is part of Knuckle Cracker binary diff utility.
	Copyright (C) 2004  Stanislaw Y. Pusep

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	E-Mail:	cr@cker.com.br
	Site:	http://sysd.org/
****************************************************************************/


#include "escape.h"

int ascdigit(char chr, int base)
{
	int x;
	chr = crk_tolower(chr);

	if (chr >= '0' && chr <= '9')
		x = (chr - '0');
	else if (chr >= 'a' && chr <= 'f')
		x = (chr - 'a') + 10;
	else
		return -1;

	if (x < base)
		return x;
	else
		return -1;
}

int asc2chr(char *str, int base, unsigned char *result)
{
	int i, j;
	int x;
	int pow = 1;
	int num = 0;
	int max = 0;
	int len = strlen(str);

	switch (base)
	{
		case 0x02: max = 7; break;
		case 0x08: max = 2; break;
		case 0x0a: max = 2; break;
		case 0x10: max = 1; break;
	}

	if (max > len)
		max = len;

	for (i = max, j = 0; i >= 0; i--, j++)
	{
		x = ascdigit(str[i], base);
		if (x != -1)
		{
			num += pow * x;
			pow *= base;
		}
		else
		{
			num = 0;
			pow = 1;
			j = -1;
		}
	}

	if (j > 0)
	{
		*result = num > 0xff ? 0xff : (unsigned char) num;
		return j - 1;
	}
	else
		return -1;
}

int crk_uncescape(char *str, char *buf, int buflen)
{
	char *p;
	char x;
	int i, len;

	for (p = str, i = 0; *p != '\0' && i < buflen; p++, i++)
	{
		if (*p == '\\')
		{
			p++;
			x = '\0';

			switch (*p)
			{
				/* octal */
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					if ((len = asc2chr(p, 8, &x)) >= 0)
						p += len;
					else
						continue;
					break;

				/* hex */
				case 'x':
					p ++;
					if ((len = asc2chr(p, 16, &x)) >= 0)
						p += len;
					else
					{
						p -= 2;
						buf[i] = '\\';
						continue;
					}
					break;

				/* binary */
				case '!':
					p ++;
					if ((len = asc2chr(p, 2, &x)) >= 0)
						p += len;
					else
					{
						p -= 2;
						buf[i] = '\\';
						continue;
					}
					break;

				/* common escapes */
				case '\\': x = '\\'; break;
				case '"': x = '"'; break;
				case 't': x = 0x09; break;
				case 'n': x = 0x0a; break;
				case 'r': x = 0x0d; break;
				case 'f': x = 0x0c; break;
				case 'b': x = 0x08; break;
				case 'a': x = 0x07; break;
				case 'e': x = 0x1b; break;

				/* asshole-proof */
				default: buf[i] = *p; continue;
			}

			buf[i] = x;
		}
		else
		{
			buf[i] = *p;
		}
	}

	return i;
}

char *crk_cescape(unsigned char *buf, int buflen)
{
	int i, len = (buflen + 1) * 4;
	unsigned char x, y[5], z, *str = (char *) malloc(len);
	char *fmt;

	memset(str, '\0', len);

	for (i = 0; i < buflen; i++)
	{
		memset(y, '\0', sizeof(y));
		y[0] = '\\';
		x = buf[i];

		// next character
		if (i + 1 < buflen)
			z = buf[i + 1];
		else
			z = 0;

		switch (x)
		{
			case '\\': y[1] = '\\'; break;
			case  '"': y[1] = '"'; break;
			case 0x09: y[1] = 't'; break;
			case 0x0a: y[1] = 'n'; break;
			case 0x0d: y[1] = 'r'; break;
			case 0x0c: y[1] = 'f'; break;
			case 0x08: y[1] = 'b'; break;
			case 0x07: y[1] = 'a'; break;
			case 0x1b: y[1] = 'e'; break;
			default:
				// printable ASCII
				if (x >= 32 && x <= 126)
					y[0] = x;
				else
				{
					if (x < 8)
						// oct, pad if necessary
						fmt = crk_isodigit(z) ? "\\%03o" : "\\%o";
					else
						// hex, pad if necessary
						fmt = crk_isxdigit(z) ? "\\x%02x" : "\\x%x";

					sprintf(y, fmt, x);
				}
		}
		strcat(str, y);
	}

	return str;
}
