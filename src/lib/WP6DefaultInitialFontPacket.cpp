/* libwpd2
 * Copyright (C) 2002 William Lachance (wlach@interlog.com)
 * Copyright (C) 2002 Marc Maurer (j.m.maurer@student.utwente.nl)
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * For further information visit http://libwpd.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by 
 * Corel Corporation or Corel Corporation Limited."
 */

#include "WP6DefaultInitialFontPacket.h"
#include "libwpd_internal.h"

WP6DefaultInitialFontPacket::WP6DefaultInitialFontPacket(FILE *stream, int id, guint32 dataOffset, guint32 dataSize)
	: WP6PrefixDataPacket(stream, id)
{
	_read(stream, dataOffset, dataSize);
}

void WP6DefaultInitialFontPacket::_readContents(FILE *stream)
{
   WPD_CHECK_FILE_READ_ERROR(fread(&m_numPrefixIDs, sizeof(guint16), 1, stream), 1);
   WPD_CHECK_FILE_READ_ERROR(fread(&m_initialFontDescriptorPID, sizeof(guint16), 1, stream), 1);
   WPD_CHECK_FILE_READ_ERROR(fread(&m_pointSize, sizeof(guint16), 1, stream), 1);
   WPD_DEBUG_MSG(("WordPerfect: Read default initial font packet (initial font descriptor pid: %i, point size: %i)\n", 
		  (int) m_initialFontDescriptorPID, (int) m_pointSize));

}
