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


#include "unbind.h"

char *findbin(void)
{
	static char bin[PATH_MAX];

#ifdef _WIN32
	memset(bin, '\0', sizeof(bin));
	if (GetModuleFileName(NULL, bin, sizeof(bin) - 1) == 0)
		return NULL;
#else
	char buf[PATH_MAX];

	memset(bin, '\0', sizeof(bin));
	memset(buf, '\0', sizeof(buf));

	#if defined(__linux__)
		#define LINK "/proc/%d/exe"
	#elif defined(__FreeBSD__)
		#define LINK "/proc/%d/file"
	#else
		#error Can not handle GetModuleFileName() on this platform
	#endif

	snprintf(buf, sizeof(buf) - 1, LINK, getpid());

	if (readlink(buf, bin, sizeof(bin) - 1) == -1)
		return NULL;
#endif

	return bin;
}

FILE *unbind(void)
{
	FILE *bin;
	unsigned long size, offset;
	char buf[16];
	int i;

	if ((bin = fopen(findbin(), "rt")) == NULL)
		return NULL;

	if (fseek(bin, -8, SEEK_END))
		goto bad;

	size = ftell(bin);
	memset(buf, '\0', sizeof(buf));

	if (fread(buf, 8, 1, bin) == 0)
		goto bad;

	for (i = 0; i < 8; i++)
		if (!(crk_isxdigit(buf[i])))
			goto bad;

	if (sscanf(buf, "%lx", &offset) == 0)
		goto bad;

	if (offset < 1 || offset > size)
		goto bad;

	if (fseek(bin, offset * (-1), SEEK_END) == 0)
		return bin;

bad:
	fclose(bin);
	return NULL;
}
