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


#ifndef _ALPHANUM_H
#define _ALPHANUM_H

#define crk_tolower(c)		((c>='A'&&c<='Z')?(c+('a'-'A')):c)
#define crk_isspace(c)		((c>=0x9&&c<=0xd)||(c==0x20))
#define crk_isodigit(c)		(c>='0'&&c<='7')
#define crk_isxdigit(c)		((c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F'))
#define crk_istext(c)		(c>=32&&c<=126)

#ifdef _WIN32
	#define crk_isslash(c)	(c=='\\')
#else
	#define crk_isslash(c)	(c=='/')
#endif

#endif
