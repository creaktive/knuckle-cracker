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


#include "crk_parser.h"

int thunk_record(char *line, unsigned long *offset, char *data)
{
	int i, len;
	char nibble, *p, patch[256];

	for (p = line, i = 0; crk_isxdigit(*p) || crk_isspace(*p); p++, i++);
	if (i < 2)
		return 0;

	memset(patch, '\0', sizeof(patch));
	len = 0;
	if (*p == '"')
	{
		// HACK! '#' is supposed to never appear here...
		sscanf(line, "%lx \"%250[^#]", offset, patch);
		// HACK! prerequisite sizeof(patch)==sizeof(data)
		if ((len = crk_uncescape(patch, data, sizeof(patch))) != 0)
		{
			// HACK HACK HACK!!! strip trailing '"'
			data[--len] = '\0';
		}
	}
	else
	{
		for (p = line; *p != '\0'; p++)
		{
			*p = crk_tolower(*p);
			if (!crk_isspace(*p) && !crk_isxdigit(*p))
			{
				p = NULL;
				break;
			}
		}
		if (!p)
			return 0;

		sscanf(line, "%lx %250[ 0-9a-f]", offset, patch);

		for (p = patch, i = 0; *p != '\0'; p++)
		{
			if (!crk_isspace(*p))
			{
				p += asc2chr(p, 16, &nibble);
				data[i++] = nibble;
				len++;
			}
		}
	}

	return len;
}

crk_list *crk_parser(FILE *f, comment_callback crk_comment)
{
	char buf[2048], data[256];
	char *line, *p;
	int i, j, len, num = 0;
	crk_list *head = NULL, *tail = NULL;
	crk_list *file = NULL;
	crk_thunk *thunk, *thunk_tail = NULL;
	unsigned char nibble, checksum[16];
	unsigned long offset;

	memset(buf, '\0', sizeof(buf));
	while (fgets(buf, sizeof(buf) - 1, f) != NULL)
	{
		++num;

		for (line = buf; crk_isspace(*line); line++);
		if ((p = strchr(line, '#')) != NULL)
			*p = '\0';
		for (len = strlen(line) - 1; len > 0 && crk_isspace(line[len]); len--)
			line[len] = '\0';
		if (!++len)
			continue;

		if (quote_test(line, '[', ']'))
		{
			quote_adjust(line);

			for (i = 0, j = 0; i < 32; i += 2, j++)
			{
				if (asc2chr(line + i, 16, &nibble) != 1)
					break;
				checksum[j] = nibble;
			}

			line += i;
			for (; *line == '*' || crk_isspace(*line); line++);

			if (j != 16 || *line == '\0')
			{
				crk_print("malformed file record");
				continue;
			}

			file = (crk_list *) malloc(sizeof(crk_list));
			memset(file, 0, sizeof(crk_list));
			strncpy(file->filename, line, PATH_MAX - 1);
			memcpy(file->checksum, checksum, sizeof(checksum));
			file->head = NULL;

			if (head == NULL)
			{
				head = file;
				tail = file;
			}
			else
			{
				tail->next = file;
				tail = file;
			}
		}
		else if (quote_test(line, '"', '"'))
		{
			quote_adjust(line);
			memset(data, '\0', sizeof(data));
			crk_uncescape(line, data, sizeof(data));
			crk_print(data);
		}
		else
		{
			if (file == NULL)
			{
				crk_print("orphaned patch thunk");
				continue;
			}

			if ((len = thunk_record(line, &offset, data)) == 0)
			{
				crk_print("malformed patch record");
				continue;
			}

			thunk = (crk_thunk *) malloc(sizeof(crk_thunk));
			memset(thunk, 0, sizeof(crk_thunk));
			thunk->data = (char *) malloc(len + 1);
			memset(thunk->data, '\0', len + 1);

			thunk->offset = offset;
			memcpy(thunk->data, data, len);
			thunk->len = len;
			thunk->next = NULL;

			if (file->head == NULL)
			{
				file->head = thunk;
				thunk_tail = thunk;
			}
			else
			{
				thunk_tail->next = thunk;
				thunk_tail = thunk;
			}
		}
	}

	return head;
}
