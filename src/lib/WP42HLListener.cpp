/* libwpd
 * Copyright (C) 2003 William Lachance (william.lachance@sympatico.ca)
 * Copyright (C) 2003-2004 Marc Maurer (j.m.maurer@student.utwente.nl)
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

#include "WP42HLListener.h"
#include "WP42FileStructure.h"

WP42HLListener::WP42HLListener(vector<WPXPageSpan *> *pageList, WPXHLListenerImpl *listenerImpl) : 
	WPXHLListener(pageList, listenerImpl),
	WP42LLListener()
{
	m_textBuffer.clear();
}

/****************************************
 public 'HLListenerImpl' functions
*****************************************/

void WP42HLListener::insertCharacter(const guint16 character)
{
	m_textBuffer.append(character);
}

void WP42HLListener::insertTab(const guint8 tabType)
{
	_flushText();
	m_listenerImpl->insertTab();
}

void WP42HLListener::insertEOL()
{
	_flushText();
	m_ps->m_numDeferredParagraphBreaks++;
}

void WP42HLListener::endDocument()
{
	// corner case: document ends in a list element
	/*if (m_parseState->m_styleStateSequence.getCurrentState() != NORMAL)
	{
		_flushText(); // flush the list text
		m_parseState->m_styleStateSequence.setCurrentState(NORMAL);
		_flushText(true); // flush the list exterior (forcing a line break, to make _flushText think we've exited a list)
	}*/
	// corner case: document contains no end of lines
	/*else*/ if (!m_ps->m_isParagraphOpened && !m_ps->m_isParagraphClosed)
	{
		_flushText();       
	}
	// NORMAL(ish) case document ends either inside a paragraph or outside of one,
	// but not inside an object
	else if (!m_ps->m_isParagraphClosed || !m_ps->m_isParagraphOpened)
	{
		_flushText();
	}
	
	// the only other possibility is a logical contradiction: a paragraph
	// may not be opened and closed at the same time

	// close the document nice and tight
	_closeSection();
	_closePageSpan();
	m_listenerImpl->endDocument();
}


/****************************************
 public 'parser' functions
*****************************************/

void WP42HLListener::attributeChange(const bool isOn, const guint8 attribute)
{
	
	// flush everything which came before this change
	_flushText();
	
	guint32 textAttributeBit = 0;
	
	// FIXME: handle all the possible attribute bits
	switch (attribute)
	{
	/*case WP42_ATTRIBUTE_SUBSCRIPT:
		textAttributeBit = WPX_SUBSCRIPT_BIT;
		break;
	case WP42_ATTRIBUTE_SUPERSCRIPT:
		textAttributeBit = WPX_SUPERSCRIPT_BIT;
		break;
	case WP42_ATTRIBUTE_OUTLINE:
		textAttributeBit = WPX_OUTLINE_BIT;
		break;*/
	case WP42_ATTRIBUTE_ITALICS:
		textAttributeBit = WPX_ITALICS_BIT;
		break;
	case WP42_ATTRIBUTE_SHADOW:
		textAttributeBit = WPX_SHADOW_BIT;
		break;
	case WP42_ATTRIBUTE_REDLINE:
		textAttributeBit = WPX_REDLINE_BIT;
		break;
	/*case WP42_ATTRIBUTE_DOUBLE_UNDERLINE:
		textAttributeBit = WPX_DOUBLE_UNDERLINE_BIT;
		break;			*/
	case WP42_ATTRIBUTE_BOLD:
		textAttributeBit = WPX_BOLD_BIT;
		break;
	case WP42_ATTRIBUTE_STRIKE_OUT:
		textAttributeBit = WPX_STRIKEOUT_BIT;
		break;
	case WP42_ATTRIBUTE_UNDERLINE:
		textAttributeBit = WPX_UNDERLINE_BIT;
		break;
	}
	
	if (isOn) 
		m_ps->m_textAttributeBits |= textAttributeBit;
	else
		m_ps->m_textAttributeBits ^= textAttributeBit;

	m_ps->m_textAttributesChanged = true;
}

/****************************************
 private functions
*****************************************/

/*
TODO: this function needs to be fleshed out
*/
void WP42HLListener::_flushText()
{
	// create a new section, and a new paragraph, if our section attributes have changed and we have inserted
	// something into the document (or we have forced a break, which assumes the same condition)
	if (m_ps->m_sectionAttributesChanged && (m_textBuffer.getLen() > 0 || m_ps->m_numDeferredParagraphBreaks > 0/* || fakeText*/))
	{
		_openSection();
		//if (fakeText)
			_openParagraph();
	}
	
	if (m_ps->m_numDeferredParagraphBreaks > 0)
	{
		if (!m_ps->m_isParagraphOpened //&&
			// !(m_parseState->m_isTableOpened && !m_parseState->m_isTableCellOpened) // don't allow paragraphs to be opened when we have already opened a table, but no cell yet. - MARCM (is it really correct, or should this be fixed elsewhere??)
		)
			m_ps->m_numDeferredParagraphBreaks++;

		while (m_ps->m_numDeferredParagraphBreaks > 1) 
			_openParagraph(); 			
		_closeParagraph(); 
		m_ps->m_numDeferredParagraphBreaks = 0; // compensate for this by requiring a paragraph to be opened
	}
	else if (m_ps->m_textAttributesChanged && m_textBuffer.getLen())
	{
		_openSpan();
		m_ps->m_textAttributesChanged = false;
	}
	
	if (m_textBuffer.getLen())
	{
		if (!m_ps->m_isParagraphOpened)
		{
			_openParagraph();
			_openSpan();
		}
		
		m_listenerImpl->insertText(m_textBuffer);
		m_textBuffer.clear();
	}
}

/*
TODO: this function needs to be fleshed out
*/
void WP42HLListener::_openParagraph()
{
	_closeParagraph();
	/*guint8 paragraphJustification;
	(m_parseState->m_tempParagraphJustification != 0) ? paragraphJustification = m_parseState->m_tempParagraphJustification :
		paragraphJustification = m_parseState->m_paragraphJustification;
	m_parseState->m_tempParagraphJustification = 0;*/
	
	m_listenerImpl->openParagraph(0, m_ps->m_textAttributeBits,
				      m_ps->m_paragraphMarginLeft, m_ps->m_paragraphMarginRight,
				      m_ps->m_fontName->str, m_ps->m_fontSize, 
				      1.0f, 
				      false, false);
	if (m_ps->m_numDeferredParagraphBreaks > 0) 
		m_ps->m_numDeferredParagraphBreaks--;

	m_ps->m_isParagraphOpened = true;
}