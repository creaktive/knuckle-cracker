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
#include "crk_parser.h"
#include "unbind.h"

void parser_output(const int line, const char *string)
{
	printf("%3d: %s\n", line, string);
	fflush(stdout);
	return;
}

int main(int argc, char *argv[])
{
	int arg;
	int verbose = 0;
	char *crack = NULL;
	char *over[16];
	int count = 0;
	FILE *script;
	int handle;
	crk_list *list, *file;
	crk_thunk *patch;
	int patched;

	if ((script = unbind()) != NULL)
		crack = argv[0];

	memset(over, 0, sizeof(over));
	for (arg = 1; arg < argc; arg++)
	{
		if (argv[arg][0] == '-')
			switch (argv[arg][1])
			{
				case 'v': verbose = 1; break;
			}
		else if (crack == NULL)
			crack = argv[arg];
		else
		{
			over[count++] = argv[arg];
			if (count >= sizeof(over) / sizeof(char *))
				break;
		}
	}

	if (crack == NULL)
	{
		printf("usage: %s [-v] <crack_script> [override file1,2,3...]\n", argv[0]);
		return 0;
	}

	arg = count;
	count = 0;

	if (script == NULL && (script = fopen(crack, "rt")) == NULL)
	{
		perror("fopen()");
		return -1;
	}

	printf("LINE COMMENT\n");
	fflush(stdout);

	if ((list = crk_parser(script, parser_output)) == NULL)
	{
		printf("couldn't read info\n");
		fflush(stdout);
		return -1;
	}

	fclose(script);

	for (file = list; file != NULL; file = file->next)
	{
		if (count <= arg && over[count] != NULL)
			strncpy(file->filename, over[count], PATH_MAX - 1);
		count++;

		printf("\n * file '%s'\n    ", file->filename);
		fflush(stdout);

		switch (test_file(file->filename, file->checksum))
		{
			case  1:
			{
				printf("file checksum ok!\n");
				fflush(stdout);
				break;
			}
			case  0:
			{
				printf("file checksum failed!\n");
				fflush(stdout);
				continue;
			}
			default:
			{
				perror("unable to check file");
				continue;
			}
		}

		if (!backup_file(file->filename))
		{
			perror("unable to backup file");
			continue;
		}

		handle = open(file->filename, O_WRONLY | O_BINARY, 0644);
		if (handle == -1)
		{
			perror("unable to open file");
			continue;
		}

		for (patch = file->head, patched = 0; patch != NULL; patch = patch->next)
		{
			if (verbose)
			{
				printf("\t0x%08lx: %4d bytes\n", patch->offset, patch->len);
				fflush(stdout);
			}

			if (patch_file(handle, patch->offset, patch->data, patch->len))
				patched += patch->len;
			else
				perror("patch failed");
		}

		close(handle);

		printf("\tpatched %d bytes\n", patched);
		fflush(stdout);
	}

	printf("\nhave a fun!\n");
	fflush(stdout);

	return 0;
}
