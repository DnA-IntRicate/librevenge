/* libwpd
 * Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 * Copyright (C) 2007 Novell Inc. (http://www.novell.com)
s *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 * For further information visit http://libwpd.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by 
 * Corel Corporation or Corel Corporation Limited."
 */

#include "WP6BoxGroup.h"
#include "WP6Listener.h"
#include "libwpd_internal.h"
#include "WP6FileStructure.h"
#include "WPXFileStructure.h"
#include "WP6GraphicsFilenamePacket.h"

WP6BoxGroup::WP6BoxGroup(WPXInputStream *input) :
	WP6VariableLengthGroup()
{
	_read(input);
}

void WP6BoxGroup::_readContents(WPXInputStream *input)
{
	switch (getSubGroup())	
	{
		case WP6_BOX_GROUP_CHARACTER_ANCHORED_BOX:
			break;
		case WP6_BOX_GROUP_PARAGRAPH_ANCHORED_BOX:
			break;
		case WP6_BOX_GROUP_PAGE_ANCHORED_BOX:
			break;
		case WP6_BOX_GROUP_GRAPHICS_RULE:
			break;
		default: /* something else we don't support, since it isn't in the docs */
			break;
	}
}

void WP6BoxGroup::parse(WP6Listener *listener)
{
	WPD_DEBUG_MSG(("WordPerfect: handling a Column group\n"));

	if (getFlags() & 0x40)  // Ignore function flag
		return;
		
	std::vector<uint16_t> graphicsDataIds;
	std::vector<uint16_t>::iterator gdiIter;
	for (int i=0; i<getNumPrefixIDs(); i++)
	{
		if (const WP6GraphicsFilenamePacket *gfPacket = dynamic_cast<const WP6GraphicsFilenamePacket *>(listener->getPrefixDataPacket(getPrefixIDs()[i]))) 
		{
			graphicsDataIds = gfPacket->getChildIds();
			break;
		}
	}
	
	switch (getSubGroup())
	{
		case WP6_BOX_GROUP_CHARACTER_ANCHORED_BOX:
			for (gdiIter = graphicsDataIds.begin(); gdiIter != graphicsDataIds.end(); gdiIter++)
				listener->insertGraphicsData((*gdiIter), WPX_CHARACTER);
			break;
		case WP6_BOX_GROUP_PARAGRAPH_ANCHORED_BOX:
			for (gdiIter = graphicsDataIds.begin(); gdiIter != graphicsDataIds.end(); gdiIter++)
				listener->insertGraphicsData((*gdiIter), WPX_PARAGRAPH);
			break;
		case WP6_BOX_GROUP_PAGE_ANCHORED_BOX:
			for (gdiIter = graphicsDataIds.begin(); gdiIter != graphicsDataIds.end(); gdiIter++)
				listener->insertGraphicsData((*gdiIter), WPX_PAGE);
			break;
		case WP6_BOX_GROUP_GRAPHICS_RULE:
			break;
		default: /* something else we don't support, since it isn't in the docs */
			break;
	}
}