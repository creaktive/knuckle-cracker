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


#ifndef _PATCHER_H
#define _PATCHER_H

#ifdef _WIN32
	#include <io.h>
	#define _POSIX_
#else
	#include <unistd.h>
	#define O_BINARY	0
#endif

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"

#define BUFSIZE 32768

int test_file(const char *filename, const unsigned char *checksum);
unsigned long backup_file(const char *filename);
int patch_file(int handle, unsigned long offset, char *data, int len);

#endif
