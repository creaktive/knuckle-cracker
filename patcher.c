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


#include "patcher.h"

int test_file(const char *filename, const unsigned char *checksum)
{
	int handle;
	struct MD5Context md5c;
	unsigned char keybuf[16];
	char *buf;
	int nread;

	handle = open(filename, O_RDONLY | O_BINARY, 0644);
	if (handle == -1)
		return -1;

	MD5Init(&md5c);
	buf = (char *) malloc(BUFSIZE);

	while ((nread = read(handle, buf, BUFSIZE)) != 0)
		MD5Update(&md5c, buf, nread);

	close(handle);
	MD5Final(keybuf, &md5c);
	free(buf);

	return (memcmp(keybuf, checksum, sizeof(keybuf)) == 0) ? 1 : 0;
}

unsigned long backup_file(const char *filename)
{
	int from, to;
	char backup[PATH_MAX], *buf;
	int nread, nwrite;
	unsigned long bytes = 0;

	strncpy(backup, filename, sizeof(backup) - 5);
	strcat(backup, ".bak");

	from = open(filename, O_RDONLY | O_BINARY, 0644);
	to = open(backup, O_CREAT | O_WRONLY | O_BINARY, 0644);

	if (from == -1 || to == -1)
		return -1;

	buf = (char *) malloc(BUFSIZE);

	while ((nread = read(from, buf, BUFSIZE)) != 0)
	{
		nwrite = write(to, buf, nread);

		if (nread == nwrite)
			bytes += nwrite;
		else
		{
			bytes = 0;
			break;
		}
	}

	close(from);
	close(to);

	free(buf);

	if (!bytes)
		unlink(backup);

	return bytes;
}

int patch_file(int handle, unsigned long offset, char *data, int len)
{
	int nwrite;

	if (lseek(handle, offset, SEEK_SET) == -1)
		return 0;

	nwrite = write(handle, data, len);

	return (len == nwrite) ? 1 : 0;
}
