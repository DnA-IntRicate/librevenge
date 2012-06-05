/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* libwpd
 * Copyright (C) 2012 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02111-1301 USA
 *
 * For further information visit http://libwpd.sourceforge.net
 */

#ifndef __WPXZIPSTREAM_H__
#define __WPXZIPSTREAM_H__

#include <vector>
#include "WPXStream.h"

class WPXZipStream
{
public:
	static bool isZipFile(WPXInputStream *input);

	static WPXInputStream *getSubstream(WPXInputStream *input, const char *name);
};

#endif // __WPXZIPSTREAM_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */