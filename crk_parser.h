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


#ifndef _CRK_PARSER_H
#define _CRK_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "alphanum.h"
#include "escape.h"

#ifdef _WIN32
	#define _POSIX_
	#define snprintf	_snprintf
#endif

#include <limits.h>

#define crk_print(s) 			\
	if (crk_comment)		\
		crk_comment(num, s);

#define quote_test(s,ql,qr)	(s[0]==ql && s[len-1]==qr)
#define quote_adjust(s)		s++;len--;s[len-1]= '\0';

typedef struct _crk_thunk
{
	unsigned long offset;
	char *data;
	int len;
	struct _crk_thunk *next;
} crk_thunk;

typedef struct _crk_file
{
	char filename[PATH_MAX];
	unsigned char checksum[16];
	crk_thunk *head;
	struct _crk_file *next;
} crk_list;

typedef void (*comment_callback) (const int line, const char *comment);

crk_list *crk_parser(FILE *f, comment_callback crk_comment);

#endif
