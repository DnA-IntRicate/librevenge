/* libwpd
 * Copyright (C) 2002 William Lachance (william.lachance@sympatico.ca)
 * Copyright (C) 2002 Marc Maurer (j.m.maurer@student.utwente.nl)
 *  
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

#include "WP6FootnoteEndnoteGroup.h"
#include "libwpd_internal.h"

WP6FootnoteEndnoteGroup::WP6FootnoteEndnoteGroup(WPXInputStream *input) :
	WP6VariableLengthGroup()
{
	_read(input);
}

void WP6FootnoteEndnoteGroup::_readContents(WPXInputStream *input)
{
}

void WP6FootnoteEndnoteGroup::parse(WP6Listener *listener)
{
	WPD_DEBUG_MSG(("WordPerfect: handling a footnote/endnote group\n"));
	
	switch (getSubGroup())
	{
	case WP6_FOOTNOTE_ENDNOTE_GROUP_FOOTNOTE_ON:
	case WP6_FOOTNOTE_ENDNOTE_GROUP_ENDNOTE_ON:
		if (getNumPrefixIDs() < 1)
			throw ParseException();
		listener->noteOn(getPrefixIDs()[0]);
		break;
	case WP6_FOOTNOTE_ENDNOTE_GROUP_FOOTNOTE_OFF:
		listener->noteOff(FOOTNOTE);
		break;
	case WP6_FOOTNOTE_ENDNOTE_GROUP_ENDNOTE_OFF:
		listener->noteOff(ENDNOTE);
		break;
	}
}
