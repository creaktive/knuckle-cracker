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
#include <time.h>
#include "alphanum.h"
#include "escape.h"
#include "md5.h"

#define LINELEN	40

void print_as_text(FILE *out, unsigned char *data, int len)
{
	int asc = 0;
	int i;
	char *p;

	for (i = 0; i < len; i++)
		if (crk_istext(data[i]))
			asc++;

	if ((float) asc / (float) len >= 0.80)
	{
		p = crk_cescape(data, len);
		fputc('"', out);
		fputs(p, out);
		fputc('"', out);
		free(p);
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			fprintf(out, "%X", data[i]);
			if (i != len - 1)
				fputc(' ', out);
		}
	}

	return;
}

int dump_row(FILE *out, unsigned long index, unsigned char *olddata, unsigned char *newdata, int len)
{
	if (len != 0)
	{
		fprintf(out, "%lx\t", index);
		print_as_text(out, newdata, len);

		if (olddata != NULL)
		{
			fputs("\t\t# was: ", out);
			print_as_text(out, olddata, len);
		}

		fputc('\n', out);
	}

	return len;
}

#define get_size(handle, size)			\
	size = lseek(handle, 0, SEEK_END);	\
	lseek(handle, 0, SEEK_SET);

#define set_me_free()	\
	free(oldb);	\
	free(newb);	\
	free(oldrow);	\
	free(newrow);	\
	close(oldh);	\
	close(newh);


void bin_diff(const int verbose, const char *oldn, const char *newn, const char *outf)
{
	FILE *out;
	int oldh, newh;
	unsigned long olds, news;
	int oldr, newr;
	time_t clock;
	struct MD5Context md5c;
	unsigned char checksum[16];
	char *oldb, *newb, *oldrow = NULL, *newrow;
	unsigned long now, pos, last = -2;
	int i;
	unsigned long rowidx = 0;
	int rowlen = 0;
	int len;
	char *basename;

	if ((oldh = open(oldn, O_RDONLY | O_BINARY, 0644)) == -1)
	{
		perror(oldn);
		return;
	}
	if ((newh = open(newn, O_RDONLY | O_BINARY, 0644)) == -1)
	{
		perror(newn);
		return;
	}

	get_size(oldh, olds);
	get_size(newh, news);

	if (olds <= 0 || news <= 0)
	{
		perror("get file size");
		return;
	}

	if (olds != news)
	{
		fprintf(stderr, "can't compare files with different sizes!\n");
		return;
	}

	if ((out = fopen(outf, "wt")) == NULL)
	{
		perror("output");
		return;
	}

	MD5Init(&md5c);

	oldb = (char *) malloc(BUFSIZE);
	newb = (char *) malloc(BUFSIZE);
	newrow = (char *) malloc(LINELEN);

	if (verbose)
		oldrow = (char *) malloc(LINELEN);

	// groovy ;)
	for ( \
		basename = (char *) oldn + strlen(oldn); \
		basename > oldn && !(crk_isslash(*basename) && basename++); \
		basename-- \
	);
	fprintf(out, "[%-32s %s]\n", "", basename);

	time(&clock);
	fprintf(out, "# Patch generated at %s", asctime(localtime(&clock)));

	do
	{
		now = lseek(oldh, 0, SEEK_CUR);

		oldr = read(oldh, oldb, BUFSIZE);
		newr = read(newh, newb, BUFSIZE);

		if (oldr != newr)
		{
			set_me_free();
			perror("compare");
			return;
		}

		MD5Update(&md5c, oldb, oldr);

		for (i = 0; i < oldr; i++)
		{

			if (oldb[i] != newb[i])
			{
				pos = now + i;

				if (pos - last != 1 || rowlen >= LINELEN)
				{
					dump_row(out, rowidx, oldrow, newrow, rowlen);
					rowidx = pos;
					rowlen = 0;
				}

				if (oldrow != NULL)
					oldrow[rowlen] = oldb[i];

				newrow[rowlen++] = newb[i];

				last = pos;
			}
		}
	} while (newr != 0);

	dump_row(out, rowidx, oldrow, newrow, rowlen);

	MD5Final(checksum, &md5c);
	set_me_free();

	fprintf(out, "# Patch length: %-8s", "");
	len = ftell(out);
	fseek(out, -8, SEEK_END);
	fprintf(out, "%08x", len);

	fseek(out, 1, SEEK_SET);
	for (i = 0; i < 16; i++)
		fprintf(out, "%02x", checksum[i]);

	fclose(out);

	return;
}

int main(int argc, char *argv[])
{
	int i = 0, verbose = 0;
	char *oldf, *newf, *outf;

	if (argc == 5 && strcmp(argv[1], "-v") == 0)
	{
		i++;
		verbose++;
	}
	else if (argc != 4)
	{
		printf("usage: %s [-v] old_file new_file out_patch\n", argv[0]);
		return 0;
	}

	oldf = argv[++i];
	newf = argv[++i];
	outf = argv[++i];

	if (access(outf, 0) != -1)
		printf("output file already exists\n");
	else
	{
		bin_diff(verbose, oldf, newf, outf);
		printf("done!\n");
	}

	return 0;
}
