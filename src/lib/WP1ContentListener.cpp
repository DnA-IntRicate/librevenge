/* libwpd
 * Copyright (C) 2003 William Lachance (william.lachance@sympatico.ca)
 * Copyright (C) 2003-2004 Marc Maurer (uwog@uwog.net)
 * Copyright (C) 2005-2006 Fridrich Strba (fridrich.strba@bluewin.ch)
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 * For further information visit http://libwpd.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */

#include "WP1ContentListener.h"
#include "WP1FileStructure.h"
#include "libwpd_internal.h"

#define WP1_NUM_TEXT_COLUMS_PER_INCH 12

_WP1ContentParsingState::_WP1ContentParsingState()
{
	m_textBuffer.clear();
}

_WP1ContentParsingState::~_WP1ContentParsingState()
{
	m_textBuffer.clear();
}


WP1ContentListener::WP1ContentListener(std::list<WPXPageSpan> &pageList, std::vector<WP1SubDocument *> &subDocuments, WPXHLListenerImpl *listenerImpl) :
	WP1Listener(),
	WPXContentListener(pageList, listenerImpl),
	m_subDocuments(subDocuments),
	m_parseState(new WP1ContentParsingState)
{
// Default line is 6 lpi, it means that the caracters are of 12 points
	*(m_ps->m_fontName) = "Courier";
	m_ps->m_fontSize = 12.0f;
}

WP1ContentListener::~WP1ContentListener() 
{
	delete m_parseState;
}


void WP1ContentListener::insertCharacter(const uint16_t character)
{
	if (!isUndoOn())
	{
		if (!m_ps->m_isSpanOpened)
			_openSpan();
		appendUCS4(m_parseState->m_textBuffer, (uint32_t)character);
	}
}

void WP1ContentListener::insertTab(const uint8_t tabType, float tabPosition)
{
	if (!isUndoOn())
	{
		if (!m_ps->m_isSpanOpened)
			_openSpan();
		else
			_flushText();

		m_listenerImpl->insertTab();
	}
}

void WP1ContentListener::insertEOL()
{
	if (!isUndoOn())
	{
		if (!m_ps->m_isParagraphOpened && !m_ps->m_isListElementOpened)
			_openSpan();

		if (m_ps->m_isParagraphOpened)
			_closeParagraph();
		if (m_ps->m_isListElementOpened)
			_closeListElement();
	}
}

void WP1ContentListener::attributeChange(const bool isOn, const uint8_t attribute)
{
	_closeSpan();

	uint32_t textAttributeBit = 0;

	// FIXME: handle all the possible attribute bits
	switch (attribute)
	{
	/*case WP1_ATTRIBUTE_SUBSCRIPT:
		textAttributeBit = WPX_SUBSCRIPT_BIT;
		break;
	case WP1_ATTRIBUTE_SUPERSCRIPT:
		textAttributeBit = WPX_SUPERSCRIPT_BIT;
		break;
	case WP1_ATTRIBUTE_OUTLINE:
		textAttributeBit = WPX_OUTLINE_BIT;
		break;*/
	case WP1_ATTRIBUTE_ITALICS:
		textAttributeBit = WPX_ITALICS_BIT;
		break;
	case WP1_ATTRIBUTE_SHADOW:
		textAttributeBit = WPX_SHADOW_BIT;
		break;
	case WP1_ATTRIBUTE_REDLINE:
		textAttributeBit = WPX_REDLINE_BIT;
		break;
	/*case WP1_ATTRIBUTE_DOUBLE_UNDERLINE:
		textAttributeBit = WPX_DOUBLE_UNDERLINE_BIT;
		break;			*/
	case WP1_ATTRIBUTE_BOLD:
		textAttributeBit = WPX_BOLD_BIT;
		break;
	case WP1_ATTRIBUTE_STRIKE_OUT:
		textAttributeBit = WPX_STRIKEOUT_BIT;
		break;
	case WP1_ATTRIBUTE_UNDERLINE:
		textAttributeBit = WPX_UNDERLINE_BIT;
		break;
	}

	if (isOn)
		m_ps->m_textAttributeBits |= textAttributeBit;
	else
		m_ps->m_textAttributeBits ^= textAttributeBit;
}

void WP1ContentListener::marginReset(const uint8_t leftMargin, const uint8_t rightMargin)
{
#if 0
	if (!isUndoOn())
	{
		float leftMarginInch = (float)(leftMargin/WP1_NUM_TEXT_COLUMS_PER_INCH);
		float rightMarginInch = m_ps->m_pageFormWidth - (float)((rightMargin + 1)/WP1_NUM_TEXT_COLUMS_PER_INCH);
		m_ps->m_leftMarginByPageMarginChange = leftMarginInch - m_ps->m_pageMarginLeft;
		m_ps->m_rightMarginByPageMarginChange = rightMarginInch - m_ps->m_pageMarginRight;
	}
#endif
}

void WP1ContentListener::headerFooterGroup(const uint8_t headerFooterDefinition, WP1SubDocument *subDocument)
{
	if (subDocument)
		m_subDocuments.push_back(subDocument);			
}	

void WP1ContentListener::_handleSubDocument(const WPXSubDocument *subDocument, const bool isHeaderFooter, WPXTableList tableList, int nextTableIndice)
{
	// save our old parsing state on our "stack"
	WP1ContentParsingState *oldParseState = m_parseState;

	m_parseState = new WP1ContentParsingState();

	if (subDocument)
	{
		static_cast<const WP1SubDocument *>(subDocument)->parse(this);
	}
	else
		_openSpan();

	// Close the sub-document properly
	if (m_ps->m_isParagraphOpened)
		_closeParagraph();
	if (m_ps->m_isListElementOpened)
		_closeListElement();

	m_ps->m_currentListLevel = 0;
	_changeList();
#if 0
	_closeSection();
#endif

	// restore our old parsing state
	delete m_parseState;
	m_parseState = oldParseState;
}

/****************************************
 private functions
*****************************************/

void WP1ContentListener::_flushText()
{
	if (m_parseState->m_textBuffer.len())
		m_listenerImpl->insertText(m_parseState->m_textBuffer);
	m_parseState->m_textBuffer.clear();
}