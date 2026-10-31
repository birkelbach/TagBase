#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string.h>

/*  TagBase - An open source Tag Data Storage Library
 *  Copyright (c) 2026 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Some convenience debugging functions.  Most of this goes away in release mode */

/* This is a conditionally compiled formatted debug macro that also prints
 * the filename and line number of the function call. */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef DEBUG
# define DF(...) do {fprintf(stderr, __VA_ARGS__); fprintf(stderr, " : %s:%d --> %s()\n", __FILENAME__, __LINE__, __func__);} while(0)
#else
#define DF(...)
#endif

#endif