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


#ifndef _ESCAPE_H
#define _ESCAPE_H

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alphanum.h"

int asc2chr(char *str, int base, unsigned char *result);
int crk_uncescape(char *str, char *buf, int buflen);
char *crk_cescape(unsigned char *buf, int buflen);

#endif
